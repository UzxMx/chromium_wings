#ifndef WINGS_APP_PATHS_MAC_
#define WINGS_APP_PATHS_MAC_

namespace base {
class FilePath;
}

// Sets up base::mac::FrameworkBundle.
void OverrideFrameworkBundlePath();

// Sets up base::DIR_SOURCE_ROOT to properly point to the source directory.
void OverrideSourceRootPath();

// Gets the path to the wings's pak file.
base::FilePath GetResourcesPakFilePath();

#endif // WINGS_APP_PATHS_MAC_
