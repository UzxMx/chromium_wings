#include "wings/browser/wings_url_request_context_getter.h"
#include "wings/browser/wings_network_delegate.h"
#include "wings/common/wings_content_client.h"

#include <memory>
#include <utility>

#include "base/command_line.h"
#include "base/logging.h"
#include "base/memory/ptr_util.h"
#include "base/single_thread_task_runner.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_split.h"
#include "base/strings/string_util.h"
#include "base/task_scheduler/post_task.h"
#include "build/build_config.h"
#include "components/network_session_configurator/browser/network_session_configurator.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/cookie_store_factory.h"
#include "content/public/common/content_switches.h"
#include "content/shell/common/shell_switches.h"
#include "net/cert/cert_verifier.h"
#include "net/cert/ct_policy_enforcer.h"
#include "net/cert/ct_policy_status.h"
#include "net/cert/do_nothing_ct_verifier.h"
#include "net/cookies/cookie_store.h"
#include "net/dns/host_resolver.h"
#include "net/dns/mapped_host_resolver.h"
#include "net/http/http_network_session.h"
#include "net/net_features.h"
#include "net/proxy_resolution/proxy_config_service.h"
#include "net/proxy_resolution/proxy_service.h"
#include "net/ssl/channel_id_service.h"
#include "net/ssl/default_channel_id_store.h"
#include "net/url_request/url_request_context.h"
#include "net/url_request/url_request_context_builder.h"
#include "services/network/public/cpp/features.h"
#include "services/network/public/cpp/network_switches.h"
#include "url/url_constants.h"

namespace wings {

// TODO
WingsURLRequestContextGetter::WingsURLRequestContextGetter(
    bool ignore_certificate_errors,
    bool off_the_record,
    const base::FilePath& base_path,
    scoped_refptr<base::SingleThreadTaskRunner> io_task_runner,
    content::ProtocolHandlerMap* protocol_handlers,
    content::URLRequestInterceptorScopedVector request_interceptors,
    net::NetLog* net_log)
    : shut_down_(false) {
  // Must first be created on the UI thread.
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);

  std::swap(protocol_handlers_, *protocol_handlers);

  // We must create the proxy config service on the UI loop on Linux because it
  // must synchronously run on the glib message loop. This will be passed to
  // the URLRequestContextStorage on the IO thread in GetURLRequestContext().
  proxy_config_service_ = GetProxyConfigService();
}

WingsURLRequestContextGetter::~WingsURLRequestContextGetter() {}

net::URLRequestContext* WingsURLRequestContextGetter::GetURLRequestContext() {
  DCHECK_CURRENTLY_ON(content::BrowserThread::IO);

  if (shut_down_)
    return nullptr;

  if (!url_request_context_) {
    net::URLRequestContextBuilder builder;
    builder.set_network_delegate(CreateNetworkDelegate());

    std::unique_ptr<net::CookieStore> cookie_store =
        CreateCookieStore(content::CookieStoreConfig());
    std::unique_ptr<net::ChannelIDService> channel_id_service =
        std::make_unique<net::ChannelIDService>(
            new net::DefaultChannelIDStore(nullptr));
    cookie_store->SetChannelIDServiceID(channel_id_service->GetUniqueID());
    builder.SetCookieAndChannelIdStores(std::move(cookie_store),
                                        std::move(channel_id_service));
    builder.set_accept_language("en-us,en");
    builder.set_user_agent(GetWingsUserAgent());

    for (auto& protocol_handler : protocol_handlers_) {
      builder.SetProtocolHandler(
          protocol_handler.first,
          base::WrapUnique(protocol_handler.second.release()));
    }
    protocol_handlers_.clear();

    builder.set_data_enabled(true);    

#if !BUILDFLAG(DISABLE_FILE_SUPPORT)
    builder.set_file_enabled(true);
#endif    

    url_request_context_ = builder.Build();
  }

  return url_request_context_.get();
}

std::unique_ptr<net::NetworkDelegate>
WingsURLRequestContextGetter::CreateNetworkDelegate() {
  return base::WrapUnique(new WingsNetworkDelegate);
}

std::unique_ptr<net::CertVerifier>
WingsURLRequestContextGetter::GetCertVerifier() {
  return net::CertVerifier::CreateDefault();
}

std::unique_ptr<net::ProxyConfigService>
WingsURLRequestContextGetter::GetProxyConfigService() {
  // return net::ProxyResolutionService::CreateSystemProxyConfigService(
  //     io_task_runner_);
    return nullptr;
}

std::unique_ptr<net::ProxyResolutionService>
WingsURLRequestContextGetter::GetProxyService() {
  // TODO(jam): use v8 if possible, look at chrome code.
  return nullptr;
}

scoped_refptr<base::SingleThreadTaskRunner>
    WingsURLRequestContextGetter::GetNetworkTaskRunner() const {
  return content::BrowserThread::GetTaskRunnerForThread(content::BrowserThread::IO);
}

net::HostResolver* WingsURLRequestContextGetter::host_resolver() {
  return url_request_context_->host_resolver();
}

} // namespace wings
