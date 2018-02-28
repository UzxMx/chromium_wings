#ifndef WINGS_BROWSER_WINGS_BROWSER_MAIN_
#define WINGS_BROWSER_WINGS_BROWSER_MAIN_

#include <memory>

namespace content {
class BrowserMainRunner;
struct MainFunctionParams;
}

int WingsBrowserMain(
    const content::MainFunctionParams& parameters,
    const std::unique_ptr<content::BrowserMainRunner>& main_runner);

#endif // WINGS_BROWSER_WINGS_BROWSER_MAIN_
