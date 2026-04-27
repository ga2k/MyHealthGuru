module;
#include "Core/Core.h"
#include "Core/CoreData.h"
#include <wx/wx.h>
#include "Gfx/GlobalIDs.h"
#include "Gfx/WidgetsFwd.h"

#include <filesystem>
#include <fstream>

#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>

#include <sstream>
#include <string>

#if defined(ISWIN)
#   include "MyCare/resource.h"
#endif
// clang-format on

export module MC.Frame;
import Frame;
import Document;

// import DB.Field;

import DDT;
import HtmlDialog;
import StringUtil;
import SystemSignals;
import Types;
import Util;
import wxUtil;
//
import MC.DBManager;
import MC.DBase;
import MC.Document;
//
import NewFile.Wizard;

namespace mc {
export class Frame : public ::CFrame {

 public:
   struct initialData {
      bool valid {};
      std::string title {};
      std::string given {};
      std::string family {};
      std::tm dob {};
      hs_bool fnf {};
      std::filesystem::path filename;
   };
   Frame();
   Frame (wxDocManager *manager, wxFrame *parent, wxWindowID id, const wxString &title,
          const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize,
          long style = wxDEFAULT_FRAME_STYLE, const wxString &name = wxFrameNameStr);
   ~Frame() override { TraceCall; }
   bool Create (wxDocManager *manager, wxFrame *parent, wxWindowID id, const wxString &title,
                const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, long style = 541072960,
                const wxString &name = wxFrameNameStr);

 protected:
   bool doCreate();
   auto FirstRun() -> std::pair<bool, std::filesystem::path>;

   void onSearch (wxCommandEvent &event) override;
   void uiSearch (wxUpdateUIEvent &event) override;
   void onSearchCanceled (wxCommandEvent &event) override;

   auto initMenuBar(int menuID) -> void override;
   auto initToolBar() -> void override;

   wxDECLARE_DYNAMIC_CLASS (Frame);
};
} // namespace mc
