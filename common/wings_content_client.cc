#include "wings/common/wings_content_client.h"

#include "base/command_line.h"
#include "base/strings/string_piece.h"
#include "base/strings/utf_string_conversions.h"
#include "blink/public/resources/grit/blink_image_resources.h"
#include "build/build_config.h"
#include "content/app/resources/grit/content_resources.h"
#include "content/app/strings/grit/content_strings.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/user_agent.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/base/resource/resource_bundle.h"

namespace wings {

std::string GetWingsUserAgent() {
  std::string product = "Wings/" WINGS_VERSION;
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  if (command_line->HasSwitch(switches::kUseMobileUserAgent))
    product += " Mobile";
  return content::BuildUserAgentFromProduct(product);
}

WingsContentClient* WingsContentClient::GetInstance() {
  return base::Singleton<WingsContentClient>::get();
}

WingsContentClient::WingsContentClient() {
}

WingsContentClient::~WingsContentClient() {
}

std::string WingsContentClient::GetUserAgent() const {
  return GetWingsUserAgent();
}

base::string16 WingsContentClient::GetLocalizedString(int message_id) const {
  printf("GetLocalizedString unimplemented\n");
  return base::string16();
}

base::StringPiece WingsContentClient::GetDataResource(
    int resource_id,
    ui::ScaleFactor scale_factor) const {
  return ui::ResourceBundle::GetSharedInstance().GetRawDataResourceForScale(
      resource_id, scale_factor);
}

base::RefCountedMemory* WingsContentClient::GetDataResourceBytes(
    int resource_id) const {
  return ui::ResourceBundle::GetSharedInstance().LoadDataResourceBytes(
      resource_id);
}

gfx::Image& WingsContentClient::GetNativeImageNamed(int resource_id) const {
  printf("GetNativeImageNamed unimplemented\n");
  return ui::ResourceBundle::GetSharedInstance().GetNativeImageNamed(
      resource_id);
}

content::OriginTrialPolicy* WingsContentClient::GetOriginTrialPolicy() {
  printf("GetOriginTrialPolicy unimplemented\n");
  return nullptr;
}

} // namespace wings
