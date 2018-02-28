#include "wings/browser/wings_permission_manager.h"

#include "base/callback.h"
#include "base/command_line.h"
#include "content/public/browser/permission_type.h"
#include "content/public/common/content_switches.h"
#include "content/shell/common/shell_switches.h"
#include "media/base/media_switches.h"

namespace wings {

WingsPermissionManager::WingsPermissionManager() : content::PermissionManager() {

}
WingsPermissionManager::~WingsPermissionManager() {
}

int WingsPermissionManager::RequestPermission(
    content::PermissionType permission,
    content::RenderFrameHost* render_frame_host,
    const GURL& requesting_origin,
    bool user_gesture,
    const base::Callback<void(blink::mojom::PermissionStatus)>& callback) {
  LOG(INFO) << "unimplemented";
  return 0;
}

int WingsPermissionManager::RequestPermissions(
    const std::vector<content::PermissionType>& permission,
    content::RenderFrameHost* render_frame_host,
    const GURL& requesting_origin,
    bool user_gesture,
    const base::Callback<
        void(const std::vector<blink::mojom::PermissionStatus>&)>& callback) {
  LOG(INFO) << "unimplemented";
  return 0;
}

void WingsPermissionManager::ResetPermission(content::PermissionType permission,
                     const GURL& requesting_origin,
                     const GURL& embedding_origin) {
  LOG(INFO) << "unimplemented";
}

blink::mojom::PermissionStatus WingsPermissionManager::GetPermissionStatus(
    content::PermissionType permission,
    const GURL& requesting_origin,
    const GURL& embedding_origin) {
  LOG(INFO) << "unimplemented";
  return blink::mojom::PermissionStatus::GRANTED;
}

int WingsPermissionManager::SubscribePermissionStatusChange(
    content::PermissionType permission,
    const GURL& requesting_origin,
    const GURL& embedding_origin,
    const base::Callback<void(blink::mojom::PermissionStatus)>& callback) {
  LOG(INFO) << "unimplemented";
  return 0;  
}

void WingsPermissionManager::UnsubscribePermissionStatusChange(int subscription_id) {
  LOG(INFO) << "unimplemented";
}

} // namespace wings
