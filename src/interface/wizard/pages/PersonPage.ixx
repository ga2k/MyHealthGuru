module;
#include "Core/CoreData.h"

#include "Core/Core.h"
#include "Core/Util.h"
#include "Gfx/Widgets.h"
#include "Gfx/wx.h"

export module User.Page;
import WizardPage;
import Interface;

import Ctrl;
import Validator;
import DDT;
import Types;
import wxTypes;
import Util;

import Name.Group;
import Contact.Group;

import TextCtrl;
import InfoBar;

namespace wx {
export class PersonPage : public WizardPage {
   TextCtrl *m_GivenName {};
   TextCtrl *m_FamilyName {};

   /*
    * Args required by PersonPage
    *
    * arg              type                    use
    * ---------------- ----------------------- --------------------------------------
    * layoutPath       std::filesystem::path   the layout file for the controls
    * bResidential     bool                    true if residential, false if business
    * idUser           ID::Type                the db iid for this user or ID::New
    *
    */

   std::filesystem::path layoutPath;
   bool bResidential {};
   ID::Type idUser {};

   // clang-format off
    NameGroup*       m_nameGroup {};
    ContactGroup*    m_contactGroup {};
    InfoBar*         m_InfoBar {};
   // clang-format on

   // auto OnPageKillActive (wxWizardEvent &event) -> void override;
   // auto OnPageSetActive (wxWizardEvent &event) -> void override;

 public:
   ~PersonPage() override = default;
   explicit PersonPage (wxWizard *parent, const std::string &pageName = "", const std::string &introduction = "",
                        const anymap &parameters = {})
      : WizardPage (parent, pageName, introduction, parameters) {
      TraceCall;

      layoutPath = hs::param (parameters, "layoutPath", "UserPage.yaml");
      bResidential = hs::param (parameters, "bResidential", false);
      idUser = hs::param (parameters, "idUser", ID::New);

#define DFLT_LABEL_OPTS wxDefaultPosition, wxDefaultSize

      anymap nameGroupArgs = parameters;
      nameGroupArgs["layoutPath"] = "NameGroup.yaml";
      addGroup (m_nameGroup = new NameGroup (UICreateFlags::Null, "Name Details", this, NewControlId(), wxVERTICAL,
                                             " Name Details ", nameGroupArgs));

      // if ( idUser != ID::New ) {
      //     anymap miscUserItemsGroupArgs = parameters;
      //     miscUserItemsGroupArgs["layoutPath"] = "MiscUserItemsGroup.yaml";
      //     addGroup (m_miscUserItemsGroup = new UserGroup (
      //                   idUser == ID::None ? UICreateFlags::Hide | UICreateFlags::NoValidation : UICreateFlags::Null,
      //                   "Misc. User Items", this, NewControlId(), wxVERTICAL, " Misc. User Items ",
      //                   miscUserItemsGroupArgs));
      // }

      anymap contactGroupArgs = parameters;
      contactGroupArgs["layoutPath"] = "ContactGroup.yaml";
      addGroup (m_contactGroup = new ContactGroup (UICreateFlags::Null, "Contact Details", this, NewControlId(),
                                                   wxVERTICAL, " Contact Details ", contactGroupArgs));

      m_InfoBar = new InfoBar (UICreateFlags::ExcludeFromColour, "InfoBar", this, NewControlId(), 500,
                               wxSHOW_EFFECT_SLIDE_TO_TOP, wxSHOW_EFFECT_SLIDE_TO_BOTTOM);
      addControl (m_InfoBar);

      // Try to load the layout file and dynamically place the controls. If that fails, fall back to the
      // manual placement.

      bool autoLayoutSuccessful {};

#if defined(USING_YAML)

      auto layoutFile {hs::resourceName (UIType::FormLayout, "UserPage.yaml", false, nullptr)};
      if ( !layoutFile.empty() ) {

         autoLayoutSuccessful = loadLayout (m_layout, getControlMap(), layoutFile);
      }
#endif
      if ( !autoLayoutSuccessful ) {
         m_layout.sizer->Clear(); // Fresh start

         // clang-format off
             auto bc = borderWidth;
             int cntrlFlags = stdCtrlFlags;

             placeControl (m_layout, m_nameGroup,                0,  0,  1,  1,  cntrlFlags, bc);
             placeControl (m_layout, m_contactGroup,             1,  0,  1,  1,  cntrlFlags, bc);
             placeControl (m_layout, m_InfoBar,                  2,  0,  1,  1,  cntrlFlags, bc);
         // clang-format on
      }

      for ( auto i : std::views::iota (0, m_layout.sizer->GetEffectiveColsCount()) ) {
         if ( !m_layout.sizer->IsColGrowable (i) )
            m_layout.sizer->AddGrowableCol (i, 1);
      }
      for ( auto i : std::views::iota (0, m_layout.sizer->GetEffectiveRowsCount()) ) {
         if ( !m_layout.sizer->IsRowGrowable (i) )
            m_layout.sizer->AddGrowableRow (i, 1);
      }
      GetPageSizer().Add (m_layout.sizer, 1, wxEXPAND | wxALL | wxALIGN_CENTRE_VERTICAL, borderWidth);
      SetSizerAndFit (&GetPageSizer());
   }
};
} // namespace wx
