#ifndef WINGS_APP_WINGS_CONTENT_MAIN_H_
#define WINGS_APP_WINGS_CONTENT_MAIN_H_

#include "build/build_config.h"

#if defined(OS_MACOSX)
extern "C" {
__attribute__((visibility("default")))
int ContentMain(int argc,
                const char** argv);
}  // extern "C"
#endif  // OS_MACOSX

#endif // WINGS_APP_WINGS_CONTENT_MAIN_H_
