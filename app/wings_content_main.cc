#include "build/build_config.h"
#include "content/public/app/content_main.h"

#include "wings/app/wings_content_main.h"
#include "wings/app/wings_main_delegate.h"

#if defined(OS_MACOSX)
int ContentMain(int argc,
                const char** argv) {
  wings::WingsMainDelegate delegate;
  content::ContentMainParams params(&delegate);
  params.argc = argc;
  params.argv = argv;
  return content::ContentMain(params);
}
#endif  // OS_MACOSX
