#include "wings/browser/wings.h"

#include <stddef.h>

#include "base/auto_reset.h"
#include "base/command_line.h"
#include "base/location.h"
#include "base/macros.h"
#include "base/message_loop/message_loop.h"
#include "base/run_loop.h"
#include "base/single_thread_task_runner.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"
#include "base/threading/thread_task_runner_handle.h"
#include "build/build_config.h"
#include "content/public/browser/devtools_agent_host.h"
#include "content/public/browser/navigation_controller.h"
#include "content/public/browser/navigation_entry.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/render_widget_host.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_observer.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/renderer_preferences.h"
#include "content/public/common/webrtc_ip_handling_policy.h"
#include "media/media_features.h"
#include "third_party/WebKit/public/web/WebPresentationReceiverFlags.h"

namespace wings {

Wings::Wings(content::WebContents* web_contents)
    : content::WebContentsObserver(web_contents),
      web_contents_(web_contents) {
  web_contents_->SetDelegate(this);
}

Wings::~Wings() {
  // PlatformCleanUp();

  // for (size_t i = 0; i < windows_.size(); ++i) {
  //   if (windows_[i] == this) {
  //     windows_.erase(windows_.begin() + i);
  //     break;
  //   }
  // }

  // if (windows_.empty() && quit_message_loop_) {
  //   if (headless_)
  //     PlatformExit();
  //   base::ThreadTaskRunnerHandle::Get()->PostTask(
  //       FROM_HERE, base::MessageLoop::QuitWhenIdleClosure());
  // }

  web_contents_->SetDelegate(nullptr);
}

void Wings::LoadURL(const GURL& url) {
  LoadURLForFrame(url, std::string());
}

void Wings::LoadURLForFrame(const GURL& url, const std::string& frame_name) {
  LOG(INFO) << url.GetContent();
  content::NavigationController::LoadURLParams params(url);
  params.transition_type = ui::PageTransitionFromInt(
      ui::PAGE_TRANSITION_TYPED | ui::PAGE_TRANSITION_FROM_ADDRESS_BAR);
  params.frame_name = frame_name;
  web_contents_->GetController().LoadURLWithParams(params);
  web_contents_->Focus();
}

gfx::Size Wings::GetWingsDefaultSize() {
  static gfx::Size default_shell_size;
  if (!default_shell_size.IsEmpty())
    return default_shell_size;

  default_shell_size = gfx::Size(800, 600);
  return default_shell_size;
}

gfx::Size Wings::AdjustWindowSize(const gfx::Size& initial_size) {
  if (!initial_size.IsEmpty())
    return initial_size;
  return GetWingsDefaultSize();
}

void Wings::Initialize() {
  PlatformInitialize(GetWingsDefaultSize());
}

Wings* Wings::CreateNewWindow(content::BrowserContext* browser_context,
                              const GURL& url,
                              const scoped_refptr<content::SiteInstance>& site_instance,
                              const gfx::Size& initial_size) {
  content::WebContents::CreateParams create_params(browser_context, site_instance);
  create_params.initial_size = AdjustWindowSize(initial_size);
  content::WebContents* web_contents = content::WebContents::Create(create_params);
  Wings* wings = CreateWings(web_contents, create_params.initial_size);
  if (!url.is_empty())
    wings->LoadURL(url);
  return wings;
}

Wings* Wings::CreateWings(content::WebContents* web_contents,
                          const gfx::Size& initial_size) {
  Wings *wings = new Wings(web_contents);
  wings->PlatformCreateWindow(initial_size.width(), initial_size.height());

  wings->PlatformSetContents();

  wings->PlatformResizeSubViews();
  return wings;
}

void Wings::ShowDevTools() {
  LOG(INFO) << "unimplemented";
}

} // namespace wings
