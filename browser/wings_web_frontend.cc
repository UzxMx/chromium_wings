#include "wings/browser/wings_web_frontend.h"
#include "wings/browser/wings_web_manager.h"
#include "wings/browser/wings_web_bindings.h"

#include "base/strings/string_number_conversions.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_observer.h"
#include "wings/browser/wings.h"
#include "wings/browser/wings_browser_context.h"

namespace wings {

GURL GetFrontendURL() {
  int port = WingsWebManager::GetHttpHandlerPort();
  return GURL(
      base::StringPrintf("http://127.0.0.1:%d/wings/index.html", port));
}

WingsWebFrontend::WingsWebFrontend(Wings* frontend_wings, content::WebContents* inspected_contents)
  : content::WebContentsObserver(frontend_wings->web_contents()),
    frontend_wings_(frontend_wings),
    bindings_(new WingsWebBindings(frontend_wings->web_contents(), inspected_contents)) {}

WingsWebFrontend::~WingsWebFrontend() {
}

WingsWebFrontend* WingsWebFrontend::Show(content::WebContents* inspected_contents) {
  Wings* wings = Wings::CreateNewWindow(
    inspected_contents->GetBrowserContext(),
    GURL(),
    nullptr, gfx::Size());
  WingsWebFrontend* devtools_frontend = new WingsWebFrontend(wings, inspected_contents);
  wings->LoadURL(GetFrontendURL());
  return devtools_frontend;
}

void WingsWebFrontend::Activate() {
  LOG(INFO) << "unimplemented";
}

void WingsWebFrontend::Focus() {
  LOG(INFO) << "unimplemented";
}

void WingsWebFrontend::InspectElementAt(int x, int y) {
  LOG(INFO) << "unimplemented";
}

void WingsWebFrontend::Close() {
  LOG(INFO) << "unimplemented";
}

void WingsWebFrontend::DocumentAvailableInMainFrame() {
  bindings_->Attach();
}

void WingsWebFrontend::WebContentsDestroyed() {
  LOG(INFO) << "unimplemented";
}

} // namespace wings