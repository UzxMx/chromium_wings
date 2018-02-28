#include "wings/browser/wings_browser_context.h"
#include "wings/browser/wings_permission_manager.h"

#include <utility>

#include "base/bind.h"
#include "base/command_line.h"
#include "base/environment.h"
#include "base/files/file_util.h"
#include "base/logging.h"
#include "base/path_service.h"
#include "base/threading/thread.h"
#include "build/build_config.h"
#include "components/keyed_service/content/browser_context_dependency_manager.h"
#include "components/network_session_configurator/common/network_switches.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/storage_partition.h"
#include "content/public/common/content_switches.h"

namespace wings {

WingsBrowserContext::WingsResourceContext::WingsResourceContext()
    : getter_(nullptr) {}

WingsBrowserContext::WingsResourceContext::~WingsResourceContext() {
}

net::HostResolver*
WingsBrowserContext::WingsResourceContext::GetHostResolver() {
  CHECK(getter_);
  return getter_->host_resolver();
}

net::URLRequestContext*
WingsBrowserContext::WingsResourceContext::GetRequestContext() {
  CHECK(getter_);
  return getter_->GetURLRequestContext();
}

WingsBrowserContext::WingsBrowserContext(
  bool off_the_record,
  net::NetLog* net_log)
    : resource_context_(new WingsResourceContext),
      off_the_record_(off_the_record) {
  InitWhileIOAllowed();
  BrowserContextDependencyManager::GetInstance()->
      CreateBrowserContextServices(this);
}

WingsBrowserContext::~WingsBrowserContext() {
  BrowserContextDependencyManager::GetInstance()->
      DestroyBrowserContextServices(this);
}

base::FilePath WingsBrowserContext::GetPath() const {
  return path_;
}

#if !defined(OS_ANDROID)
std::unique_ptr<content::ZoomLevelDelegate> WingsBrowserContext::CreateZoomLevelDelegate(
    const base::FilePath&) {
  return std::unique_ptr<content::ZoomLevelDelegate>();
}
#endif  // !defined(OS_ANDROID)

void WingsBrowserContext::InitWhileIOAllowed() {
#if defined(OS_WIN)
  CHECK(PathService::Get(base::DIR_LOCAL_APP_DATA, &path_));
  path_ = path_.Append(std::wstring(L"wings"));
#elif defined(OS_LINUX)
  std::unique_ptr<base::Environment> env(base::Environment::Create());
  base::FilePath config_dir(
      base::nix::GetXDGDirectory(env.get(),
                                 base::nix::kXdgConfigHomeEnvVar,
                                 base::nix::kDotConfigDir));
  path_ = config_dir.Append("wings");
#elif defined(OS_MACOSX)
  CHECK(PathService::Get(base::DIR_APP_DATA, &path_));
  path_ = path_.Append("wings");
#else
  NOTIMPLEMENTED();
#endif

  if (!base::PathExists(path_))
    base::CreateDirectory(path_);
  BrowserContext::Initialize(this, path_);
}

bool WingsBrowserContext::IsOffTheRecord() const {
  return off_the_record_;
}

content::DownloadManagerDelegate* WingsBrowserContext::GetDownloadManagerDelegate()  {
  LOG(INFO) << "unimplemented";
  return nullptr;
}

content::ResourceContext* WingsBrowserContext::GetResourceContext()  {
  return resource_context_.get();
}

// TODO
content::BrowserPluginGuestManager* WingsBrowserContext::GetGuestManager() {
  LOG(INFO) << "unimplemented";
  return nullptr;
}

storage::SpecialStoragePolicy* WingsBrowserContext::GetSpecialStoragePolicy() {
  return nullptr;
}

content::PushMessagingService* WingsBrowserContext::GetPushMessagingService() {
  return nullptr;
}

content::SSLHostStateDelegate* WingsBrowserContext::GetSSLHostStateDelegate() {
  return nullptr;
}

content::PermissionManager* WingsBrowserContext::GetPermissionManager() {
  if (!permission_manager_.get()) {
    permission_manager_.reset(new WingsPermissionManager());
  }
  return permission_manager_.get();
}

content::BackgroundFetchDelegate* WingsBrowserContext::GetBackgroundFetchDelegate() {
  return nullptr;
}

content::BackgroundSyncController* WingsBrowserContext::GetBackgroundSyncController() {
  LOG(INFO) << "unimplemented, no need";
  return nullptr;
}

content::BrowsingDataRemoverDelegate*
WingsBrowserContext::GetBrowsingDataRemoverDelegate() {
  LOG(INFO) << "unimplemented";
  return nullptr;
}

WingsURLRequestContextGetter*
WingsBrowserContext::CreateURLRequestContextGetter(
    content::ProtocolHandlerMap* protocol_handlers,
    content::URLRequestInterceptorScopedVector request_interceptors) {
  return new WingsURLRequestContextGetter(
      true, off_the_record_, GetPath(),
      content::BrowserThread::GetTaskRunnerForThread(content::BrowserThread::IO),
      protocol_handlers, std::move(request_interceptors), nullptr);
}


net::URLRequestContextGetter* WingsBrowserContext::CreateRequestContext(
    content::ProtocolHandlerMap* protocol_handlers,
    content::URLRequestInterceptorScopedVector request_interceptors) {
  DCHECK(!url_request_getter_.get());
  url_request_getter_ = CreateURLRequestContextGetter(
      protocol_handlers, std::move(request_interceptors));
  resource_context_->set_url_request_context_getter(url_request_getter_.get());
  return url_request_getter_.get();
}

net::URLRequestContextGetter* WingsBrowserContext::CreateRequestContextForStoragePartition(
    const base::FilePath& partition_path,
    bool in_memory,
    content::ProtocolHandlerMap* protocol_handlers,
    content::URLRequestInterceptorScopedVector request_interceptors) {
  LOG(INFO) << "unimplemented";
  return nullptr;
}

net::URLRequestContextGetter*
    WingsBrowserContext::CreateMediaRequestContext()  {
  DCHECK(url_request_getter_.get());
  return url_request_getter_.get();
}

net::URLRequestContextGetter*
    WingsBrowserContext::CreateMediaRequestContextForStoragePartition(
        const base::FilePath& partition_path,
        bool in_memory) {
  LOG(INFO) << "unimplemented";
  return nullptr;
}

} // namespace wings
