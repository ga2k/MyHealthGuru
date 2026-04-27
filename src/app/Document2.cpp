module;
// clang-format off
#include "Gfx/wx.h"
// #include "Core/Core.h"
// #include "Core/CoreData.h"
#include "Core/Types.h"
#include "Gfx/GlobalIDs.h"
#include "Gfx/Widgets.h"
// clang-format on

module MC.Document;

// import Document;
import Frame;
// import UserSignals;
//
import User.Bar;
// import DDT;
import Types;
import Util;
// import MC.DBase;
//
//
// import NewFile.Wizard;
// import Settings.RS;
import Users.RS;
// import Years.RS;

auto mc::Document::loadUsers(ID::Type ourID) -> void {
   auto frame = wx::pFrame<CFrame>();
   auto fillAndSelect = [](wxComboBox *userCombo, ID::Type ourID, shared_ptr<db::Database> db) {
      userCombo->Clear();
      int ourIndex = wxID_NONE;
      if (ourID != ID::None) {

         try {
            auto userTable = db->getTable("users");
            UsersRS userRS(*userTable);

            auto users = userRS.getRowset();
            userCombo->Append("Everybody", Util::getInstance().intToVoidPtr((int)ID::All));

            for (auto &user : users) {
               auto thisID = user.get<int>("id");
               auto givenName = user.get<std::string>("fGivenName");
               auto familyName = user.get<std::optional<std::string>>("fFamilyName").value_or(std::string{});
               string familyInit;
               if (!familyName.empty()) {
                  familyInit = familyName.substr(0, 1);
                  if (familyName.length() > 1) {
                     familyInit += ".";
                  }
               }

               std::string displayName;
               if (!familyInit.empty()) {
                  if (user.get<std::optional<hs_bool>>("bFNF").value_or(hs_bool(false)).get() == true)
                     displayName = familyInit + " " + givenName;
                  else
                     displayName = givenName + " " + familyInit;
               } else {
                  displayName = givenName;
               }
               auto thisIndex = userCombo->Append(displayName, Util::getInstance().intToVoidPtr(thisID));
               if (thisID == (int)ourID) {
                  ourIndex = thisIndex;
               }
            }
            userCombo->SetSelection(ourIndex >= 0 ? ourIndex : 0);
         } catch (soci::soci_error &ex) {
            std::cerr << "SOCI exception in Document::loadUsers: " << ex.what() << std::endl;
         } catch (...) {
            std::cerr << "Unknown exception in Document::loadUsers" << std::endl;
         }
      }
   };

   if (frame) {
      auto toolbar = CFrame::getAuiToolbar(frame, "User");
      if (toolbar) {
         if (auto userCombo = dynamic_cast<wx::UserBar *>(toolbar)->GetUserCtrl()) {
            fillAndSelect(userCombo, ourID, db);
         }
      }
      return;
   }

   auto userWinPtr = CFrame::getAuiToolbar(wx::pFrame<CFrame>(), "Main")->FindControl(gid::TBUserComboBoxID);
   if (userWinPtr && dynamic_cast<wxComboBox *>(userWinPtr)) {
      auto userCombo = dynamic_cast<wxComboBox *>(userWinPtr);
      fillAndSelect(userCombo, ourID, db);
   }
}
