#ifndef WINGS_BROWSER_WINGS_PERMISSION_MANAGER_H_
#define WINGS_BROWSER_WINGS_PERMISSION_MANAGER_H_

#include "base/callback_forward.h"
#include "base/macros.h"
#include "content/public/browser/permission_manager.h"

namespace wings {

class WingsPermissionManager : public content::PermissionManager {
 public:
  WingsPermissionManager();
  ~WingsPermissionManager() override;

  // PermissionManager implementation.
  int RequestPermission(
      content::PermissionType permission,
      content::RenderFrameHost* render_frame_host,
      const GURL& requesting_origin,
      bool user_gesture,
      const base::Callback<void(blink::mojom::PermissionStatus)>& callback)
      override;
  int RequestPermissions(
      const std::vector<content::PermissionType>& permission,
      content::RenderFrameHost* render_frame_host,
      const GURL& requesting_origin,
      bool user_gesture,
      const base::Callback<
          void(const std::vector<blink::mojom::PermissionStatus>&)>& callback)
      override;
  void ResetPermission(content::PermissionType permission,
                       const GURL& requesting_origin,
                       const GURL& embedding_origin) override;
  blink::mojom::PermissionStatus GetPermissionStatus(
      content::PermissionType permission,
      const GURL& requesting_origin,
      const GURL& embedding_origin) override;
  int SubscribePermissionStatusChange(
      content::PermissionType permission,
      const GURL& requesting_origin,
      const GURL& embedding_origin,
      const base::Callback<void(blink::mojom::PermissionStatus)>& callback)
      override;
  void UnsubscribePermissionStatusChange(int subscription_id) override;

 private:
  DISALLOW_COPY_AND_ASSIGN(WingsPermissionManager);
};

} // namespace wings

#endif
