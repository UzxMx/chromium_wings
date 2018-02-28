#ifndef WINGS_BROWSER_WINGS_BROWSER_MAIN_PARTS_
#define WINGS_BROWSER_WINGS_BROWSER_MAIN_PARTS_

#include "wings/browser/wings_browser_context.h"

#include <memory>

#include "base/macros.h"
#include "base/metrics/field_trial.h"
#include "build/build_config.h"
#include "content/public/browser/browser_main_parts.h"
#include "content/public/common/main_function_params.h"

namespace wings {

class WingsBrowserMainParts : public content::BrowserMainParts {
public:
  explicit WingsBrowserMainParts(const content::MainFunctionParams& parameters);
  ~WingsBrowserMainParts() override;

  int PreEarlyInitialization() override;
  int PreCreateThreads() override;
  void PostCreateThreads() override;
  void PreMainMessageLoopStart() override;
  void PostMainMessageLoopStart() override;
  void PreMainMessageLoopRun() override;
  bool MainMessageLoopRun(int* result_code) override;

  WingsBrowserContext* browser_context() { return browser_context_.get(); }
  WingsBrowserContext* off_the_record_browser_context() {
    return off_the_record_browser_context_.get();
  }  

protected:
  virtual void InitializeBrowserContexts();
  virtual void InitializeMessageLoopContext();

  void set_browser_context(WingsBrowserContext* context) {
    browser_context_.reset(context);
  }
  void set_off_the_record_browser_context(WingsBrowserContext* context) {
    off_the_record_browser_context_.reset(context);
  }  

private:
  std::unique_ptr<WingsBrowserContext> browser_context_;
  std::unique_ptr<WingsBrowserContext> off_the_record_browser_context_;

  const content::MainFunctionParams parameters_;
  bool run_message_loop_;

  DISALLOW_COPY_AND_ASSIGN(WingsBrowserMainParts);
};

} // namespace wings

#endif // WINGS_BROWSER_WINGS_BROWSER_MAIN_PARTS_
