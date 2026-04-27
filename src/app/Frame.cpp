module;
// clang-format off
#include "Gfx/wx.h"
#include "Core/Core.h"
#include "Core/CoreData.h"
#include "Gfx/GlobalIDs.h"
#include "Gfx/Widgets.h"

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

#include "Core/TraceLevel.h"

module MC.Frame;
import Frame;
import Document;

import DB.Field;

import DDT;
import HtmlDialog;
import StringUtil;
import SystemSignals;
import Types;
import Util;
import wxUtil;

import MC.DBManager;
import MC.DBase;
import MC.Document;
import MC.View;

import NewFile.Wizard;

namespace mc {

Frame::Frame() : CFrame() {
   TraceCall;
   wx::pFrame (this);
   wxTheApp->SetTopWindow (this);
}

auto Frame::initMenuBar(int menuID) -> void {

   if (menuID == gid::fileMenu && (!m_menuBuilt.contains (menuID) || m_menuBuilt[menuID] == false)) {

      ASSERT_MSG (m_menus.contains (gid::fileMenu), "File menu was not found");
      auto fileMenu = m_menus.at (gid::fileMenu);
      ASSERT_MSG (fileMenu != nullptr, "File menu was not found");

      m_uiManager.addUIList (fileMenu, wx::pDocManager<db::DBManager<DBase>>()->getUIItems (fileMenu));

      // Add the MRU menu
      m_menus.insert_or_assign (gid::mruMenu, new wxMenu (str::mruMenu));
      fileMenu->AppendSubMenu (m_menus.at (gid::mruMenu), wx::to_wxString(str::mruMenu));
      fileMenu->AppendSeparator();

      // Deliberately NOT setting m_menuBuilt flag - we need to let the base class handle it, too
   }

   CFrame::initMenuBar(menuID);
}
auto Frame::initToolBar() -> void {

   // This call needs to be in the overridden method in the App's Frame class

   auto tb = getAuiToolbar ("Main"s);
   auto items = wx::pDocManager<db::DBManager<DBase>>()->getUIItems (tb, 0x01 | 0x02);
   m_uiManager.addUIList (tb, items);

   CFrame::initToolBar();
}

Frame::Frame (wxDocManager *manager, wxFrame *parent, wxWindowID id, const wxString &title, const wxPoint &pos,
              const wxSize &size, long style, const wxString &name)
   : CFrame (manager, parent, wx::nextID(), title, pos, size, style, name) {
   TraceCall;
   wx::pFrame (this);
   wxTheApp->SetTopWindow (this);
   doCreate();
}

bool Frame::Create (wxDocManager *manager, wxFrame *parent, wxWindowID id, const wxString &title, const wxPoint &pos,
                    const wxSize &size, long style, const wxString &name) {
   TraceCall;
   wx::pFrame (this);

   if ( !CFrame::Create (manager, parent, wx::nextID(), title, pos, size, style, name) )
      return false;

   return doCreate();
}


void Frame::onSearch (wxCommandEvent &event) { TraceCall; }
void Frame::uiSearch (wxUpdateUIEvent &event) { event.Skip(); }
void Frame::onSearchCanceled (wxCommandEvent &event) { TraceCall; }

bool Frame::doCreate() {
   TraceCall;

   // wxIcon appIcon = wx::appIcon();
   // if ( appIcon.IsOk() ) {
   //    SetIcon (appIcon);
   // }

   // Create a template relating drawing documents to their views
   // There may be multiple description/filter elements in mdiDescription and mdiFilter
   // They are separated from each other with a semi-colon ";"
   // There must be the ame number of descriptions as there are filters.

   hs::string_v descriptions = hs::split (hs::mdiDescription(), ";"s);
   hs::string_v filters = hs::split (hs::mdiFilter(), ";"s);
   ASSERT_MSG (descriptions.size() == filters.size(), "MDI Description/Filter mis-match");

   for ( size_t i = 0; i < descriptions.size(); ++i ) {
      // clang-format off
        wx::pDocManager()->AssociateTemplate (new DocTemplate (
            wx::pDocManager<DBManager>(),
            descriptions[i],
            filters[i],
            "",
            filters[i],
            hs::mdiDocType(),
            hs::mdiViewType(),
            CLASSINFO (Document),
            CLASSINFO (View)));
      // clang-format on

      dout << "MDI template added: " << descriptions[i] << " (" << filters[i] << ")" << std::endl;
   }

   VERIFY_MSG (wx::pDocManager<DBManager>()->LoadSettings(), "FileHandlerData could not be loaded");
   CreateBits doWhatJohn {CreateBits::Open};
   // TODO: CreateBits doWhatJohn {ShiftIsDown() ? CreateBits::Ask : CreateBits::Open};

   auto doc2Open = hs::cmdDocumentFile();
   wxString doc2OpenAsWxString;
   if (doc2Open.empty()) {
      doc2OpenAsWxString = wxEmptyString;
   } else {
      doc2OpenAsWxString = wx::to_wxString(doc2Open);
      // Only use wxFileName if we actually have a path to normalize
      if (!doc2OpenAsWxString.IsEmpty()) {
         doc2OpenAsWxString = wxFileName(doc2OpenAsWxString).GetFullPath();
      }
   }

   if ( wx::pDocManager()->CreateDocument (doc2OpenAsWxString, createBitsToLong (doWhatJohn)) == nullptr ) {
      return false;
   }

   if constexpr ( isLinux ) {
   } else if constexpr ( isWin ) {
   } else {
   }

   auto rc = initFrame(false);

   if ( rc == true ) {
      int userID;
      *wx::pDocument<Document>()->DB()->getSession() << "SELECT rUserID FROM settings WHERE id = 1;",
         soci::into (userID);
      wx::pDocument<CDocument>()->loadUsers (ID::Type(userID));
      wx::pDocManager<DBManager>()->fileHistoryUseMenu (m_menus.at (gid::mruMenu));
      wx::pDocManager<DBManager>()->updateMRUMenu();
   }
   return rc;
}

wxIMPLEMENT_DYNAMIC_CLASS (Frame, CFrame);
} // namespace mc
