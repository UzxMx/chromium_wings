#include "wings/browser/wings_web_frontend_host.h"
#include "wings/web/grit/wings_web_resources_map.h"
#include "wings/common/wings_content_client.h"

#include <stddef.h>
#include <string>

#include "content/browser/bad_message.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/devtools_frontend_host.h"
#include "third_party/WebKit/common/associated_interfaces/associated_interface_provider.h"

namespace wings {

namespace {
const char kCompatibilityScript[] = "devtools_compatibility.js";
const char kCompatibilityScriptSourceURL[] =
    "\n//# "
    "sourceURL=chrome-devtools://devtools/bundled/devtools_compatibility.js";
}  

WingsWebFrontendHost* WingsWebFrontendHost::Create(
    content::RenderFrameHost* frontend_main_frame,
    const HandleMessageCallback& handle_message_callback) {
  DCHECK(!frontend_main_frame->GetParent());
  return new WingsWebFrontendHost(frontend_main_frame, handle_message_callback);
}

WingsWebFrontendHost::WingsWebFrontendHost(content::RenderFrameHost* frame_host, const HandleMessageCallback& handle_message_callback)
    : handle_message_callback_(handle_message_callback),
      binding_(this) {
  blink::mojom::DevToolsFrontendAssociatedPtr frontend;
  frame_host->GetRemoteAssociatedInterfaces()->GetInterface(&frontend);
  std::string api_script =
      content::DevToolsFrontendHost::GetFrontendResource(kCompatibilityScript)
          .as_string() +
      kCompatibilityScriptSourceURL;
  blink::mojom::DevToolsFrontendHostAssociatedPtrInfo host;
  binding_.Bind(mojo::MakeRequest(&host));
  frontend->SetupDevToolsFrontend(api_script, std::move(host));
}

WingsWebFrontendHost::~WingsWebFrontendHost() {
}

void WingsWebFrontendHost::BadMessageRecieved() {
  LOG(INFO) << "unimplemented";
}

base::StringPiece WingsWebFrontendHost::GetFrontendResource(
    const std::string& path) {
  for (size_t i = 0; i < kWingsWebResourcesSize; ++i) {
    if (path == kWingsWebResources[i].name) {
      return WingsContentClient::GetInstance()->GetDataResource(
          kWingsWebResources[i].value, ui::SCALE_FACTOR_NONE);
    }
  }
  return std::string();
}

void WingsWebFrontendHost::DispatchEmbedderMessage(
    const std::string& message) {
  handle_message_callback_.Run(message);
}

} // namespace wings
