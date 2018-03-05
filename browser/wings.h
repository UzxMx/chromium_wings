#ifndef WINGS_BROWSER_WINGS_
#define WINGS_BROWSER_WINGS_

#include <stdint.h>

#include <memory>
#include <vector>

#include "base/callback_forward.h"
#include "base/memory/ref_counted.h"
#include "base/strings/string_piece.h"
#include "build/build_config.h"
#include "content/public/browser/web_contents_delegate.h"
#include "content/public/browser/web_contents_observer.h"
#include "ipc/ipc_channel.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gfx/native_widget_types.h"

class GURL;

namespace wings {

class WingsWebFrontend;

class Wings : public content::WebContentsDelegate,
              public content::WebContentsObserver {
public:
  ~Wings() override;

  void LoadURL(const GURL& url);
  void LoadURLForFrame(const GURL& url, const std::string& frame_name);

  void ShowDevTools();

  // Do one time initialization at application startup.
  static void Initialize();

  static Wings* CreateMainWindow(
      content::BrowserContext* browser_context,
      const GURL& url,
      const scoped_refptr<content::SiteInstance>& site_instance,
      const gfx::Size& initial_size);

  static Wings* GetMainWindow();

  static Wings* CreateNewWindow(
      content::BrowserContext* browser_context,
      const GURL& url,
      const scoped_refptr<content::SiteInstance>& site_instance,
      const gfx::Size& initial_size);

#if defined(OS_MACOSX)
  // Public to be called by an ObjC bridge object.
  void ActionPerformed(int control);
  void URLEntered(const std::string& url_string);
#endif  

  static gfx::Size GetWingsDefaultSize();

  void SetWebFrontend(WingsWebFrontend* web_frontend);
  content::WebContents* CreatePreviewerContents();

  void OpenPreviewer(std::string& url_string);
  void ClosePreviewer();

  void GoBackOrForward(int offset);
  void Reload();
  void UpdateNavigationControls(bool to_different_document);
  
  void LoadingStateChanged(content::WebContents* source,
                           bool to_different_document) override;  
  void DidNavigateMainFramePostCommit(content::WebContents* web_contents) override;

private:
  enum UIControl {
    BACK_BUTTON,
    FORWARD_BUTTON,
    STOP_BUTTON
  };

  explicit Wings(content::WebContents* web_contents);

  // Helper to create a new Wings given a newly created WebContents.
  static Wings* CreateWings(content::WebContents* web_contents,
                            const gfx::Size& initial_size);  

  // Helper for one time initialization of application
  static void PlatformInitialize(const gfx::Size& default_window_size);

  // Adjust the size when Blink sends 0 for width and/or height.
  // This happens when Blink requests a default-sized window.
  static gfx::Size AdjustWindowSize(const gfx::Size& initial_size);

  // All the methods that begin with Platform need to be implemented by the
  // platform specific Wings implementation.
  // Called from the destructor to let each platform do any necessary cleanup.
  void PlatformCleanUp();
  // Creates the main window GUI.
  void PlatformCreateWindow(int width, int height);
  // Links the WebContents into the newly created window.
  void PlatformSetContents();
  // Resize the content area and GUI.
  void PlatformResizeSubViews();
  // Enable/disable a button.
  void PlatformEnableUIControl(UIControl control, bool is_enabled);
  // Updates the url in the url bar.
  void PlatformSetAddressBarURL(const GURL& url);
  // Sets whether the spinner is spinning.
  void PlatformSetIsLoading(bool loading);
  // Set the title of wings window
  void PlatformSetTitle(const base::string16& title);      
  void PlatformSetPreviewerContents();
  void LoadURLForPreviewer(const GURL& url);

#if defined(OS_MACOSX)
  void LayoutSubviews();
#endif   
  
  std::unique_ptr<WingsWebFrontend> web_frontend_;
  std::unique_ptr<content::WebContents> web_contents_;
  std::unique_ptr<content::WebContents> previewer_web_contents_;

  gfx::NativeWindow window_;
#if defined(OS_MACOSX)
  NSView* contents_container_view_;
  NSView* previewer_container_view_;
  NSTextField* url_edit_view_;
#endif  
};

} // namespace wings

#endif // WINGS_BROWSER_WINGS_