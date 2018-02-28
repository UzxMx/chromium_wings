#ifndef WINGS_BROWSER_WINGS_CONTENT_BROWSER_CLIENT_
#define WINGS_BROWSER_WINGS_CONTENT_BROWSER_CLIENT_

#include <memory>
#include <string>

#include "wings/browser/wings_browser_context.h"

#include "base/compiler_specific.h"
#include "base/files/file_path.h"
#include "build/build_config.h"
#include "content/public/browser/content_browser_client.h"
#include "services/service_manager/public/cpp/binder_registry.h"

namespace wings {

class WingsBrowserMainParts;

class WingsContentBrowserClient : public content::ContentBrowserClient {
public:
  WingsContentBrowserClient();
  ~WingsContentBrowserClient() override;

  content::BrowserMainParts* CreateBrowserMainParts(
      const content::MainFunctionParams& parameters) override;
  bool DoesSiteRequireDedicatedProcess(content::BrowserContext* browser_context,
                                       const GURL& effective_site_url) override;
  bool IsHandledURL(const GURL& url) override;
  void BindInterfaceRequestFromFrame(
      content::RenderFrameHost* render_frame_host,
      const std::string& interface_name,
      mojo::ScopedMessagePipeHandle interface_pipe) override;
  void RegisterInProcessServices(StaticServiceMap* services) override;
  void RegisterOutOfProcessServices(OutOfProcessServiceMap* services) override;
  bool ShouldTerminateOnServiceQuit(
      const service_manager::Identity& id) override;
  std::unique_ptr<base::Value> GetServiceManifestOverlay(
      base::StringPiece name) override;
  void AppendExtraCommandLineSwitches(base::CommandLine* command_line,
                                      int child_process_id) override;
  void ResourceDispatcherHostCreated() override;
  std::string GetDefaultDownloadName() override;
  content::WebContentsViewDelegate* GetWebContentsViewDelegate(
      content::WebContents* web_contents) override;
  content::QuotaPermissionContext* CreateQuotaPermissionContext() override;
  void GetQuotaSettings(
      content::BrowserContext* context,
      content::StoragePartition* partition,
      storage::OptionalQuotaSettingsCallback callback) override;
  void SelectClientCertificate(
      content::WebContents* web_contents,
      net::SSLCertRequestInfo* cert_request_info,
      net::ClientCertIdentityList client_certs,
      std::unique_ptr<content::ClientCertificateDelegate> delegate) override;
  content::SpeechRecognitionManagerDelegate* CreateSpeechRecognitionManagerDelegate()
      override;
  net::NetLog* GetNetLog() override;
  content::DevToolsManagerDelegate* GetDevToolsManagerDelegate() override;  

  void OpenURL(content::BrowserContext* browser_context,
               const content::OpenURLParams& params,
               const base::Callback<void(content::WebContents*)>& callback) override; 

  WingsBrowserContext* browser_context();

private:
  WingsBrowserMainParts* wings_browser_main_parts_;
};

} // namespace wings

#endif // WINGS_BROWSER_WINGS_CONTENT_BROWSER_CLIENT_
