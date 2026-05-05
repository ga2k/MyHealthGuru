module;
// clang-format off
#include "Gfx/wx.h"
#include "Core/TraceLevel.h"

// #include "Core/Core.h"
#include "Core/CoreData.h"

// #include "Gfx/GlobalIDs.h"
#include "Gfx/Widgets.h"
// clang-format on

module MC.Document;

// import Document;
// import Frame;
import UserSignals;
//
// import User.Bar;
import DDT;
import Types;
import Util;
// import MC.DBase;
//
// import NewFile.Wizard;
// import Settings.RS;
// import Users.RS;
// import Years.RS;

namespace mc {
Document::Document() {
   TraceCall;
   wx::pDocument(this);
   hs::isFileOpen(false);
}

Document::~Document() { TraceCall; }

auto Document::doNewDocument() -> std::string {
   doCloseDocument();
   return "";
}

auto Document::doOpenDocument(const std::string &name) -> bool {
   doCloseDocument();
   db = std::make_shared<DBase>();
   bool rcode = db->openDB(name, hs::dataMagic(), hs::minDataVersion());
   if (rcode) {
      docName(name);
      if (hs::isRunning()) {
         int userID{ID::None};
         *db->getSession() << "SELECT rUserID FROM settings WHERE id = 1;", soci::into(userID);
         loadUsers(ID::Type(userID));
      }
   }
   return rcode;
};

auto Document::doCloseDocument() -> bool {
   if (db) {
      db->closeDB();
      db.reset();
   }
   docName();
   return true;
};

auto Document::onUserSelect(wxCommandEvent &event) -> void {
   event.Skip();
   dout << "onUserSelect" << std::endl;
   ID::Type userID = ID::Type(event.GetInt());
   wx::userSignal().NewUser(userID);
}

auto Document::onUserDropdown(wxCommandEvent &event) -> void {
   event.Skip();
   dout << "onUserDropdown" << std::endl;
}

auto Document::onUserCloseup(wxCommandEvent &event) -> void {
   event.Skip();
   dout << "onUserCloseup" << std::endl;
}

auto Document::uiUser(wxUpdateUIEvent &event) -> void { event.Skip(); }

wxIMPLEMENT_DYNAMIC_CLASS(Document, CDocument);
} // namespace mc
