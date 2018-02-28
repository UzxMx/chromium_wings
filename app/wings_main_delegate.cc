#include "wings/app/wings_main_delegate.h"
#include "wings/common/wings_content_client.h"
#include "wings/browser/wings_browser_main.h"
#include "wings/browser/wings_content_browser_client.h"
#include "wings/renderer/wings_content_renderer_client.h"

#include "base/base_switches.h"
#include "base/command_line.h"
#include "base/cpu.h"
#include "base/files/file.h"
#include "base/files/file_path.h"
#include "base/lazy_instance.h"
#include "base/logging.h"
#include "base/path_service.h"
#include "base/trace_event/trace_log.h"
#include "build/build_config.h"
#include "cc/base/switches.h"
#include "content/common/content_constants_internal.h"
#include "content/public/browser/browser_main_runner.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/url_constants.h"
#include "content/public/test/layouttest_support.h"
#include "content/public/test/ppapi_test_utils.h"
#include "gpu/config/gpu_switches.h"
#include "ipc/ipc_features.h"
#include "media/base/media_switches.h"
#include "net/cookies/cookie_monster.h"
#include "ppapi/features/features.h"
#include "services/network/public/cpp/network_switches.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/base/ui_base_paths.h"
#include "ui/base/ui_base_switches.h"
#include "ui/display/display_switches.h"
#include "ui/gl/gl_implementation.h"
#include "ui/gl/gl_switches.h"

#if defined(OS_MACOSX)
#include "content/shell/app/paths_mac.h"
#include "content/shell/app/shell_main_delegate_mac.h"
#endif  // OS_MACOSX

namespace {

void InitLogging(const base::CommandLine& command_line) {
  base::FilePath log_filename;
  std::string filename = command_line.GetSwitchValueASCII(switches::kLogFile);
  if (filename.empty()) {
    PathService::Get(base::DIR_EXE, &log_filename);
    log_filename = log_filename.AppendASCII("wings.log");
  } else {
    log_filename = base::FilePath::FromUTF8Unsafe(filename);
  }

  logging::LoggingSettings settings;
  settings.logging_dest = logging::LOG_TO_ALL;
  settings.log_file = log_filename.value().c_str();
  settings.delete_old = logging::DELETE_OLD_LOG_FILE;
  logging::InitLogging(settings);
  logging::SetLogItems(true /* Process ID */, true /* Thread ID */,
                       true /* Timestamp */, false /* Tick count */);
}

} // namespace

namespace wings {

WingsMainDelegate::WingsMainDelegate() {
}

WingsMainDelegate::~WingsMainDelegate() {
}

bool WingsMainDelegate::BasicStartupComplete(int *exit_code) {
  base::CommandLine& command_line = *base::CommandLine::ForCurrentProcess();

#if defined(OS_MACOSX)
  OverrideFrameworkBundlePath();
  // OverrideChildProcessPath();
  OverrideSourceRootPath();
  // EnsureCorrectResolutionSettings();
#endif  // OS_MACOSX 

  InitLogging(command_line);

  content_client_.reset(WingsContentClient::GetInstance());
  SetContentClient(content_client_.get());

  return false;
}

void WingsMainDelegate::PreSandboxStartup() {
  InitializeResourceBundle();
}

void WingsMainDelegate::InitializeResourceBundle() {
  ui::ResourceBundle::InitSharedInstanceWithPakPath(GetResourcesPakFilePath());
}

int WingsMainDelegate::RunProcess(
    const std::string& process_type,
    const content::MainFunctionParams& main_function_params) {
  if (!process_type.empty())
    return -1;

#if !defined(OS_ANDROID)
  // Android stores the BrowserMainRunner instance as a scoped member pointer
  // on the ShellMainDelegate class because of different object lifetime.
  std::unique_ptr<content::BrowserMainRunner> browser_runner_;
#endif  

  base::trace_event::TraceLog::GetInstance()->set_process_name("Browser");
  base::trace_event::TraceLog::GetInstance()->SetProcessSortIndex(
      content::kTraceEventBrowserProcessSortIndex);

  browser_runner_.reset(content::BrowserMainRunner::Create());

  return WingsBrowserMain(main_function_params, browser_runner_);
}

content::ContentBrowserClient* WingsMainDelegate::CreateContentBrowserClient() {
  browser_client_.reset(new WingsContentBrowserClient);

  return browser_client_.get();
}

content::ContentGpuClient* WingsMainDelegate::CreateContentGpuClient() {
  gpu_client_.reset(new content::ContentGpuClient);
  return gpu_client_.get();
}

content::ContentRendererClient* WingsMainDelegate::CreateContentRendererClient() {
  renderer_client_.reset(new WingsContentRendererClient);
  return renderer_client_.get();
}

content::ContentUtilityClient* WingsMainDelegate::CreateContentUtilityClient() {
  LOG(INFO) << "unimplemented, no need";
  return nullptr;
}

} // namespace wings
