#include "wings/browser/wings_content_browser_client.h"
#include "wings/browser/wings_browser_main_parts.h"
#include "wings/browser/wings_web_contents_view_delegate_creator.h"
#include "wings/browser/wings_devtools_manager_delegate.h"

#include <stddef.h>
#include <utility>

#include "base/base_switches.h"
#include "base/command_line.h"
#include "base/files/file.h"
#include "base/files/file_util.h"
#include "base/json/json_reader.h"
#include "base/macros.h"
#include "base/memory/ptr_util.h"
#include "base/path_service.h"
#include "base/strings/pattern.h"
#include "base/strings/utf_string_conversions.h"
#include "build/build_config.h"
#include "content/public/browser/client_certificate_delegate.h"
#include "content/public/browser/page_navigator.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/resource_dispatcher_host.h"
#include "content/public/browser/storage_partition.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/service_names.mojom.h"
#include "content/public/common/url_constants.h"
#include "content/public/common/web_preferences.h"
#include "content/public/test/test_service.h"
#include "media/mojo/features.h"
#include "net/ssl/client_cert_identity.h"
#include "net/url_request/url_request.h"
#include "net/url_request/url_request_context_getter.h"
#include "services/test/echo/public/interfaces/echo.mojom.h"
#include "storage/browser/quota/quota_settings.h"
#include "ui/base/resource/resource_bundle.h"
#include "url/gurl.h"
#include "url/origin.h"

namespace wings {

namespace {

WingsContentBrowserClient* g_browser_client;

} // namespace

WingsContentBrowserClient::WingsContentBrowserClient() : wings_browser_main_parts_(nullptr) {
  DCHECK(!g_browser_client);
  g_browser_client = this;
}

WingsContentBrowserClient::~WingsContentBrowserClient() {
  g_browser_client = nullptr;
}

content::BrowserMainParts* WingsContentBrowserClient::CreateBrowserMainParts(
    const content::MainFunctionParams& parameters) {
  wings_browser_main_parts_ = new WingsBrowserMainParts(parameters);
  return wings_browser_main_parts_;
}

bool WingsContentBrowserClient::DoesSiteRequireDedicatedProcess(content::BrowserContext* browser_context,
                                     const GURL& effective_site_url) {
  return false;
}

bool WingsContentBrowserClient::IsHandledURL(const GURL& url) {
  if (!url.is_valid())
    return false;
  // Keep in sync with ProtocolHandlers added by
  // ShellURLRequestContextGetter::GetURLRequestContext().
  static const char* const kProtocolList[] = {
      url::kBlobScheme,
      url::kFileSystemScheme,
      content::kChromeUIScheme,
      content::kChromeDevToolsScheme,
      url::kDataScheme,
      url::kFileScheme,
  };
  for (size_t i = 0; i < arraysize(kProtocolList); ++i) {
    if (url.scheme() == kProtocolList[i])
      return true;
  }
  bool result = net::URLRequest::IsHandledProtocol(url.scheme());
  LOG(INFO) << url.GetContent().c_str() << "result: " << result;
  return result;
}

void WingsContentBrowserClient::BindInterfaceRequestFromFrame(
    content::RenderFrameHost* render_frame_host,
    const std::string& interface_name,
    mojo::ScopedMessagePipeHandle interface_pipe) {
  LOG(INFO) << "unimplemented";
}

void WingsContentBrowserClient::RegisterInProcessServices(StaticServiceMap* services) {
  LOG(INFO) << "unimplemented, no need";
}

void WingsContentBrowserClient::RegisterOutOfProcessServices(OutOfProcessServiceMap* services) {
  LOG(INFO) << "unimplemented, no need";
}

bool WingsContentBrowserClient::ShouldTerminateOnServiceQuit(
    const service_manager::Identity& id) {
  LOG(INFO) << "unimplemented";
  return false;
}

std::unique_ptr<base::Value> WingsContentBrowserClient::GetServiceManifestOverlay(
    base::StringPiece name) {
  LOG(INFO) << "unimplemented, no need";
  return std::unique_ptr<base::Value>();
}

void WingsContentBrowserClient::AppendExtraCommandLineSwitches(base::CommandLine* command_line,
                                    int child_process_id) {
  LOG(INFO) << "unimplemented, no need";
}

void WingsContentBrowserClient::ResourceDispatcherHostCreated() {
  LOG(INFO) << "unimplemented, no need";
}

std::string WingsContentBrowserClient::GetDefaultDownloadName() {
  LOG(INFO) << "unimplemented";
  return std::string();
}

content::WebContentsViewDelegate* WingsContentBrowserClient::GetWebContentsViewDelegate(
    content::WebContents* web_contents) {
  return CreateWingsContentsViewDelegate(web_contents);
}

content::QuotaPermissionContext* WingsContentBrowserClient::CreateQuotaPermissionContext() {
  LOG(INFO) << "unimplemented";
  return nullptr;
}

void WingsContentBrowserClient::GetQuotaSettings(
    content::BrowserContext* context,
    content::StoragePartition* partition,
    storage::OptionalQuotaSettingsCallback callback) {
  LOG(INFO) << "unimplemented";
}

void WingsContentBrowserClient::SelectClientCertificate(
    content::WebContents* web_contents,
    net::SSLCertRequestInfo* cert_request_info,
    net::ClientCertIdentityList client_certs,
    std::unique_ptr<content::ClientCertificateDelegate> delegate) {
  LOG(INFO) << "unimplemented";
}

content::SpeechRecognitionManagerDelegate* WingsContentBrowserClient::CreateSpeechRecognitionManagerDelegate() {
  LOG(INFO) << "unimplemented, no need";
  return nullptr;
}

net::NetLog* WingsContentBrowserClient::GetNetLog() {
  LOG(INFO) << "unimplemented, no need";
  return nullptr;
}

content::DevToolsManagerDelegate* WingsContentBrowserClient::GetDevToolsManagerDelegate() {
  return new WingsDevToolsManagerDelegate(browser_context());
}


void WingsContentBrowserClient::OpenURL(content::BrowserContext* browser_context, const content::OpenURLParams& params, const base::Callback<void(content::WebContents*)>& callback) {
  LOG(INFO) << "unimplemented";
}

WingsBrowserContext* WingsContentBrowserClient::browser_context() {
  return wings_browser_main_parts_->browser_context();
}

} // namespace wings
