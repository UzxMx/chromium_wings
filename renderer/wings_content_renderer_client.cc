// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "wings/renderer/wings_content_renderer_client.h"

#include <string>

#include "base/bind.h"
#include "base/command_line.h"
#include "base/logging.h"
#include "base/macros.h"
#include "base/strings/string_number_conversions.h"
#include "components/cdm/renderer/external_clear_key_key_system_properties.h"
#include "components/web_cache/renderer/web_cache_impl.h"
#include "content/public/child/child_thread.h"
#include "content/public/common/service_manager_connection.h"
#include "content/public/common/simple_connection_filter.h"
#include "content/public/test/test_service.mojom.h"
#include "mojo/public/cpp/bindings/binding.h"
#include "mojo/public/cpp/system/message_pipe.h"
#include "net/base/net_errors.h"
#include "ppapi/features/features.h"
#include "services/service_manager/public/cpp/binder_registry.h"
#include "third_party/WebKit/public/platform/WebURLError.h"
#include "third_party/WebKit/public/web/WebTestingSupport.h"
#include "third_party/WebKit/public/web/WebView.h"
#include "v8/include/v8.h"

namespace wings {

WingsContentRendererClient::WingsContentRendererClient() {}
WingsContentRendererClient::~WingsContentRendererClient() {}

void WingsContentRendererClient::RenderThreadStarted() {
  web_cache_impl_.reset(new web_cache::WebCacheImpl());
}

void WingsContentRendererClient::RenderViewCreated(content::RenderView* render_view) {
  LOG(INFO) << "unimplemented, no need";
}

bool WingsContentRendererClient::HasErrorPage(int http_status_code) {
  return http_status_code >= 400 && http_status_code < 600;
}
void WingsContentRendererClient::PrepareErrorPage(content::RenderFrame* render_frame,
                      const blink::WebURLRequest& failed_request,
                      const blink::WebURLError& error,
                      std::string* error_html,
                      base::string16* error_description) {
  if (error_html && error_html->empty()) {
    *error_html =
        "<head><title>Error</title></head><body>Could not load the requested "
        "resource.<br/>Error code: " +
        base::IntToString(error.reason()) +
        (error.reason() < 0 ? " (" + net::ErrorToString(error.reason()) + ")"
                            : "") +
        "</body>";
  }
}  
void WingsContentRendererClient::PrepareErrorPageForHttpStatusError(
    content::RenderFrame* render_frame,
    const blink::WebURLRequest& failed_request,
    const GURL& unreachable_url,
    int http_status,
    std::string* error_html,
    base::string16* error_description) {
  LOG(INFO) << "unimplemented";
}

bool WingsContentRendererClient::IsPluginAllowedToUseCompositorAPI(const GURL& url) {
  LOG(INFO) << "unimplemented";
  return false;
}
bool WingsContentRendererClient::IsPluginAllowedToUseDevChannelAPIs() {
  LOG(INFO) << "unimplemented";
  return false;
}

void WingsContentRendererClient::DidInitializeWorkerContextOnWorkerThread(
    v8::Local<v8::Context> context) {
  LOG(INFO) << "unimplemented";
}

} // namespace wings
