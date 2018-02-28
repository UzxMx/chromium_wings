#ifndef WINGS_COMMON_WINGS_CONTENT_CLIENT_
#define WINGS_COMMON_WINGS_CONTENT_CLIENT_

#include <string>
#include <vector>

#include "base/compiler_specific.h"
#include "base/memory/singleton.h"
#include "content/public/common/content_client.h"

namespace wings {

std::string GetWingsUserAgent();

class WingsContentClient : public content::ContentClient {
public:
  static WingsContentClient* GetInstance();

  WingsContentClient();
  ~WingsContentClient() override;

  std::string GetUserAgent() const override;
  base::string16 GetLocalizedString(int message_id) const override;
  base::StringPiece GetDataResource(
      int resource_id,
      ui::ScaleFactor scale_factor) const override;
  base::RefCountedMemory* GetDataResourceBytes(
      int resource_id) const override;
  gfx::Image& GetNativeImageNamed(int resource_id) const override;
  content::OriginTrialPolicy* GetOriginTrialPolicy() override; 
};

} // namespace wings

#endif // WINGS_COMMON_WINGS_CONTENT_CLIENT_
