#ifndef WINGS_APP_WINGS_MAIN_DELEGATE_H_
#define WINGS_APP_WINGS_MAIN_DELEGATE_H_

#include <memory>

#include "base/compiler_specific.h"
#include "base/macros.h"
#include "build/build_config.h"
#include "content/public/app/content_main_delegate.h"
#include "content/public/gpu/content_gpu_client.h"

namespace content {

class ContentClient;

} // namespace content

namespace wings {

class WingsContentBrowserClient;

class WingsMainDelegate : public content::ContentMainDelegate {
public:
  WingsMainDelegate();
  ~WingsMainDelegate() override;

  bool BasicStartupComplete(int* exit_code) override;
  void PreSandboxStartup() override;
  int RunProcess(const std::string& process_type,
                 const content::MainFunctionParams& main_function_params) override;
  content::ContentBrowserClient* CreateContentBrowserClient() override;
  content::ContentGpuClient* CreateContentGpuClient() override;
  content::ContentRendererClient* CreateContentRendererClient() override;
  content::ContentUtilityClient* CreateContentUtilityClient() override;

  static void InitializeResourceBundle();  

private:
  std::unique_ptr<WingsContentBrowserClient> browser_client_;
  std::unique_ptr<content::ContentClient> content_client_;
  std::unique_ptr<content::ContentGpuClient> gpu_client_;
  std::unique_ptr<content::ContentRendererClient> renderer_client_;

  DISALLOW_COPY_AND_ASSIGN(WingsMainDelegate);
};

} // namespace wings

#endif // WINGS_APP_WINGS_MAIN_DELEGATE_H_
