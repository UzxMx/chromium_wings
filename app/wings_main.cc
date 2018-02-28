#include "build/build_config.h"

#if defined(OS_MACOSX)
#include "wings/app/wings_content_main.h"
#endif

int main(int argc, const char** argv) {
#if defined(OS_MACOSX)
  // Do the delegate work in shell_content_main to avoid having to export the
  // delegate types.
  return ::ContentMain(argc, argv);  
#else
  return 0;
#endif
}
