#include "wings/browser/wings_devtools_frontend.h"
#include "wings/browser/wings_devtools_manager_delegate.h"

#include "base/strings/string_number_conversions.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_observer.h"
#include "wings/browser/wings.h"
#include "wings/browser/wings_browser_context.h"
#include "wings/browser/wings_devtools_bindings.h"

namespace wings {

namespace {
static GURL GetFrontendURL() {
  int port = WingsDevToolsManagerDelegate::GetHttpHandlerPort();
  return GURL(
      base::StringPrintf("http://127.0.0.1:%d/devtools/inspector.html", port));
}
}  // namespace

WingsDevToolsFrontend::WingsDevToolsFrontend(Wings* frontend_wings, content::WebContents* inspected_contents)
  : content::WebContentsObserver(frontend_wings->web_contents()),
    frontend_wings_(frontend_wings),
    devtools_bindings_(new WingsDevToolsBindings(frontend_wings->web_contents(), inspected_contents, this)) {}

WingsDevToolsFrontend::~WingsDevToolsFrontend() {
}

WingsDevToolsFrontend* WingsDevToolsFrontend::Show(content::WebContents* inspected_contents) {
  Wings* wings = Wings::CreateNewWindow(
    inspected_contents->GetBrowserContext(),
    GURL(),
    nullptr, gfx::Size());
  WingsDevToolsFrontend* devtools_frontend = new WingsDevToolsFrontend(wings, inspected_contents);
  wings->LoadURL(GetFrontendURL());
  return devtools_frontend;
}

void WingsDevToolsFrontend::Activate() {
  LOG(INFO) << "unimplemented";
}

void WingsDevToolsFrontend::Focus() {
  LOG(INFO) << "unimplemented";
}

void WingsDevToolsFrontend::InspectElementAt(int x, int y) {
  LOG(INFO) << "unimplemented";
}

void WingsDevToolsFrontend::Close() {
  LOG(INFO) << "unimplemented";
}

void WingsDevToolsFrontend::DocumentAvailableInMainFrame() {
  devtools_bindings_->Attach();
}

void WingsDevToolsFrontend::WebContentsDestroyed() {
  LOG(INFO) << "unimplemented";
}

} // namespace wings
