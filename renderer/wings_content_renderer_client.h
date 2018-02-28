#ifndef WINGS_WINGS_CONTENT_RENDERER_CLIENT_H_
#define WINGS_WINGS_CONTENT_RENDERER_CLIENT_H_

#include <memory>
#include <string>

#include "base/compiler_specific.h"
#include "build/build_config.h"
#include "content/public/renderer/content_renderer_client.h"
#include "media/mojo/features.h"

namespace web_cache {
class WebCacheImpl;
}

namespace wings {

class WingsContentRendererClient : public content::ContentRendererClient {

public:
  WingsContentRendererClient();
  ~WingsContentRendererClient() override;

  // ContentRendererClient implementation.
  void RenderThreadStarted() override;
  void RenderViewCreated(content::RenderView* render_view) override;
  bool HasErrorPage(int http_status_code) override;
  void PrepareErrorPage(content::RenderFrame* render_frame,
                        const blink::WebURLRequest& failed_request,
                        const blink::WebURLError& error,
                        std::string* error_html,
                        base::string16* error_description) override;  
  void PrepareErrorPageForHttpStatusError(
      content::RenderFrame* render_frame,
      const blink::WebURLRequest& failed_request,
      const GURL& unreachable_url,
      int http_status,
      std::string* error_html,
      base::string16* error_description) override;  

  // TODO(mkwst): These toggle based on the kEnablePepperTesting flag. Do we
  // need that outside of layout tests?
  bool IsPluginAllowedToUseCompositorAPI(const GURL& url) override;
  bool IsPluginAllowedToUseDevChannelAPIs() override;

  void DidInitializeWorkerContextOnWorkerThread(
      v8::Local<v8::Context> context) override;

private:
  std::unique_ptr<web_cache::WebCacheImpl> web_cache_impl_;
};

} // namespace wings

#endif
