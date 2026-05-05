module;
// clang-format off
// #include "Gfx/wx.h"
// #include "Core/Core.h"
#include "Core/CoreData.h"

#include "Gfx/GlobalIDs.h"
#include <wx/event.h>

// clang-format on

export module MC.Document;

import Document;
import MC.DBase;
import Types;

export namespace mc {
class Document : public CDocument {

   // std::shared_ptr<mc::DBase> db;

 public:
   Document();
   ~Document() override;
   //
   // auto DB() { return db; }

 protected:
   auto doNewDocument() -> std::string override;
   auto doOpenDocument (const std::string &file) -> bool override;
   auto doSaveDocument (const std::string &newFile) -> bool override { return true; }
   auto doCloseDocument() -> bool override;

   auto loadUsers (ID::Type ourID) -> void override;
   auto onUserSelect (wxCommandEvent &event) -> void override;
   auto onUserDropdown (wxCommandEvent &event) -> void override;
   auto onUserCloseup (wxCommandEvent &event) -> void override;

   auto uiUser (wxUpdateUIEvent &event) -> void override;
   auto defaults (hs::DefaultsMode mode) -> void override {};

 private:
   bool atFirst {};
   bool atLast {};

   wxDECLARE_DYNAMIC_CLASS (Document);
};
} // namespace mc
