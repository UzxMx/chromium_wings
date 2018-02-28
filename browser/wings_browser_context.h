#ifndef WINGS_BROWSER_WINGS_BROWSER_CONTEXT_
#define WINGS_BROWSER_WINGS_BROWSER_CONTEXT_

#include "wings/browser/wings_url_request_context_getter.h"

#include <memory>

#include "base/compiler_specific.h"
#include "base/files/file_path.h"
#include "base/macros.h"
#include "base/memory/ref_counted.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/content_browser_client.h"
#include "content/public/browser/resource_context.h"
#include "net/url_request/url_request_job_factory.h"

namespace wings {

class WingsBrowserContext : public content::BrowserContext {
public:
  WingsBrowserContext(bool off_the_record, net::NetLog* net_log);
  ~WingsBrowserContext() override;

  base::FilePath GetPath() const override;
#if !defined(OS_ANDROID)
  std::unique_ptr<content::ZoomLevelDelegate> CreateZoomLevelDelegate(
      const base::FilePath& partition_path) override;
#endif  // !defined(OS_ANDROID)  
  bool IsOffTheRecord() const override;
  content::DownloadManagerDelegate* GetDownloadManagerDelegate() override;
  content::ResourceContext* GetResourceContext() override;
  content::BrowserPluginGuestManager* GetGuestManager() override;
  storage::SpecialStoragePolicy* GetSpecialStoragePolicy() override;
  content::PushMessagingService* GetPushMessagingService() override;  
  content::SSLHostStateDelegate* GetSSLHostStateDelegate() override;
  content::PermissionManager* GetPermissionManager() override;
  content::BackgroundFetchDelegate* GetBackgroundFetchDelegate() override;  
  content::BackgroundSyncController* GetBackgroundSyncController() override;
  content::BrowsingDataRemoverDelegate* GetBrowsingDataRemoverDelegate() override;
  net::URLRequestContextGetter* CreateRequestContext(
      content::ProtocolHandlerMap* protocol_handlers,
      content::URLRequestInterceptorScopedVector request_interceptors) override;
  net::URLRequestContextGetter* CreateRequestContextForStoragePartition(
      const base::FilePath& partition_path,
      bool in_memory,
      content::ProtocolHandlerMap* protocol_handlers,
      content::URLRequestInterceptorScopedVector request_interceptors) override;
  net::URLRequestContextGetter* CreateMediaRequestContext() override;
  net::URLRequestContextGetter* CreateMediaRequestContextForStoragePartition(
      const base::FilePath& partition_path,
      bool in_memory) override;              

protected:
  // Contains URLRequestContextGetter required for resource loading.
  class WingsResourceContext : public content::ResourceContext {
   public:
    WingsResourceContext();
    ~WingsResourceContext() override;

    // ResourceContext implementation:
    net::HostResolver* GetHostResolver() override;
    net::URLRequestContext* GetRequestContext() override;

    void set_url_request_context_getter(WingsURLRequestContextGetter* getter) {
      getter_ = getter;
    }

  private:
    WingsURLRequestContextGetter* getter_;

    DISALLOW_COPY_AND_ASSIGN(WingsResourceContext);
  };

  WingsURLRequestContextGetter* url_request_context_getter() {
    return url_request_getter_.get();
  }

  // Used by WingsBrowserContext to initiate and set different types of
  // URLRequestContextGetter.
  virtual WingsURLRequestContextGetter* CreateURLRequestContextGetter(
      content::ProtocolHandlerMap* protocol_handlers,
      content::URLRequestInterceptorScopedVector request_interceptors);
  void set_url_request_context_getter(WingsURLRequestContextGetter* getter) {
    url_request_getter_ = getter;
  }  

  std::unique_ptr<WingsResourceContext> resource_context_;
  std::unique_ptr<content::PermissionManager> permission_manager_;

private:
  // Performs initialization of the WingsBrowserContext while IO is still
  // allowed on the current thread.
  void InitWhileIOAllowed();

  bool off_the_record_;
  base::FilePath path_;
  scoped_refptr<WingsURLRequestContextGetter> url_request_getter_;
};

} // namespace wings

#endif // WINGS_BROWSER_WINGS_BROWSER_CONTEXT_
