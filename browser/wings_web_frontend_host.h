#ifndef WINGS_BROWSER_WINGS_WEB_FRONTEND_HOST_H_
#define WINGS_BROWSER_WINGS_WEB_FRONTEND_HOST_H_

#include <string>

#include "base/macros.h"
#include "base/callback.h"
#include "base/strings/string_piece.h"
#include "content/common/content_export.h"
#include "mojo/public/cpp/bindings/associated_binding.h"
#include "third_party/WebKit/public/web/devtools_frontend.mojom.h"

namespace content {

class RenderFrameHost;

}

namespace wings {

class WingsWebFrontendHost : public blink::mojom::DevToolsFrontendHost {
public:
  using HandleMessageCallback = base::Callback<void(const std::string&)>;

  static WingsWebFrontendHost* Create(
      content::RenderFrameHost* frontend_main_frame,
      const HandleMessageCallback& handle_message_callback);  
  WingsWebFrontendHost(content::RenderFrameHost* frame_host, const HandleMessageCallback& handle_message_callback);
  ~WingsWebFrontendHost() override;
  void BadMessageRecieved();

  static base::StringPiece GetFrontendResource(
      const std::string& path);

private:
  // blink::mojom::DevToolsFrontendHost implementation.
  void DispatchEmbedderMessage(const std::string& message) override;
    
  HandleMessageCallback handle_message_callback_;
  mojo::AssociatedBinding<blink::mojom::DevToolsFrontendHost> binding_;
};

} // namespace wings

#endif
