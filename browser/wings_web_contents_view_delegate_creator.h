#ifndef WINGS_BROWSER_WING_WEB_CONTENTS_VIEW_DELEGATE_CREATOR
#define WINGS_BROWSER_WING_WEB_CONTENTS_VIEW_DELEGATE_CREATOR

namespace content {
class WebContents;
class WebContentsViewDelegate;  
}

namespace wings {

content::WebContentsViewDelegate* CreateWingsContentsViewDelegate(
    content::WebContents* web_contents);

}  // namespace wings

#endif