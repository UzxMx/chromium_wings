#include "wings/browser/wings_browser_main_parts.h"
#include "wings/browser/wings.h"
#include "wings/browser/wings_browser_context.h"
#include "wings/browser/wings_devtools_manager_delegate.h"
#include "wings/browser/wings_web_manager.h"

#include "base/base_switches.h"
#include "base/bind.h"
#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/threading/thread.h"
#include "base/threading/thread_restrictions.h"
#include "build/build_config.h"
#include "cc/base/switches.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/devtools_agent_host.h"
#include "content/public/browser/storage_partition.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/main_function_params.h"
#include "content/public/common/result_codes.h"
#include "content/public/common/url_constants.h"
#include "device/bluetooth/bluetooth_adapter_factory.h"
#include "net/base/filename_util.h"
#include "net/base/net_module.h"
#include "net/grit/net_resources.h"
#include "ui/base/material_design/material_design_controller.h"
#include "ui/base/resource/resource_bundle.h"
#include "url/gurl.h"

namespace wings {

namespace {

// GURL GetStartupURL() {
//   base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
//   const base::CommandLine::StringVector& args = command_line->GetArgs();

// #if defined(OS_ANDROID)
//   // Delay renderer creation on Android until surface is ready.
//   return GURL();
// #endif

//   if (args.empty())
//     return GURL("https://www.baidu.com/");

//   GURL url(args[0]);
//   if (url.is_valid() && url.has_scheme())
//     return url;

//   return net::FilePathToFileURL(
//       base::MakeAbsoluteFilePath(base::FilePath(args[0])));
// }

} // namespace

WingsBrowserMainParts::WingsBrowserMainParts(
    const content::MainFunctionParams& parameters)
    : parameters_(parameters),
      run_message_loop_(true) {
}

WingsBrowserMainParts::~WingsBrowserMainParts() {
}

int WingsBrowserMainParts::PreEarlyInitialization() {
  return content::RESULT_CODE_NORMAL_EXIT;
}

int WingsBrowserMainParts::PreCreateThreads() {
  // SetupFieldTrials();
  return 0;
}

void WingsBrowserMainParts::PostCreateThreads() {
}

void WingsBrowserMainParts::PostMainMessageLoopStart() {
}

void WingsBrowserMainParts::PreMainMessageLoopRun() {
  InitializeBrowserContexts();
  Wings::Initialize();
  // net::NetModule::SetResourceProvider(PlatformResourceProvider);
  WingsDevToolsManagerDelegate::StartHttpHandler(browser_context_.get());
  WingsWebManager::StartHttpHandler(browser_context_.get());
  InitializeMessageLoopContext();
}

bool WingsBrowserMainParts::MainMessageLoopRun(int* result_code)  {
  return !run_message_loop_;
}

void WingsBrowserMainParts::InitializeBrowserContexts() {
  set_browser_context(new WingsBrowserContext(false, nullptr));
  set_off_the_record_browser_context(
      new WingsBrowserContext(true, nullptr));
}

void WingsBrowserMainParts::InitializeMessageLoopContext() {
  ui::MaterialDesignController::Initialize();
}

} // namespace wings
