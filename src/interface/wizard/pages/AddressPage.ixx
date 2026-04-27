module;
#include "Core/CoreData.h"

#include "Gfx/GlobalIDs.h"
#include "Core/Core.h"
#include "Core/Util.h"
#include "Gfx/Widgets.h"
#include "Gfx/wx.h"

export module Address.Page;
export import WizardPage;

import Ctrl;
import Validator;
import DDT;
import Types;
import wxTypes;
import Util;
import CtrlSignals;

import AddressSelector.Group;
import Address.Group;

import CheckBox;
import Combo;
import Markup;
import RadioButton;
import Button;
import TextCtrl;
import InfoBar;

namespace wx {
export class AddressPage : public WizardPage {

   /*
    * Args required by AddressGroup
    *
    * arg              type                    use
    * ---------------- ----------------------- --------------------------------------
    * bInitialRun      bool                    this is the first run of this program
    * layoutPath       std::filesystem::path   the layout file for the controls
    * bResidential     bool                    true if residential, false if business
    * idUser           ID::Type                the db iid for this user or ID::New
    * idAddress        ID::Type                the db iid for this address or ID::New
    * bPostalAddrSame  bool                    true if postal same as street address
    * bAllowSkip       bool                    user can elect to skip entering address
    *
    */

   bool bInitialRun {};
   std::filesystem::path layoutPath;
   bool bResidential {};
   ID::Type idUser {};
   ID::Type idAddress {};
   bool bPostalAddrSame {};
   bool bAllowSkip {};

   // clang-format off
    AddressSelectorGroup*   m_addressSelectorGroup {};
    AddressGroup*           m_streetAddressGroup {};
    CheckBox*               m_postalAddrSame {};
    AddressGroup*           m_postalAddressGroup {};
    InfoBar*                m_InfoBar {};
    wxWizard*               m_wizard {};
    bool                    m_skipped {};

   // clang-format on

   auto OnPageKillActive (wxWizardEvent &event) -> void override;
   auto OnPageSetActive (wxWizardEvent &event) -> void override;
   auto OnWizardHelp (wxWizardEvent &event) -> void override;

 public:
   ~AddressPage() override = default;
   explicit AddressPage (wxWizard *parent, const std::string &pageName = "", const std::string &introduction = "",
                         const anymap &parameters = {})
      : WizardPage (parent, pageName, introduction, parameters), m_wizard (parent) {
      TraceCall;

      bInitialRun = hs::param (parameters, "bInitialRun", false);
      layoutPath = hs::param (parameters, "layoutPath", "AddressPage.yaml");
      bResidential = hs::param (parameters, "bResidential", false);
      idUser = hs::param (parameters, "idUser", ID::New);
      idAddress = hs::param (parameters, "idAddress", ID::New);
      bPostalAddrSame = hs::param (parameters, "bPostalAddrSame", true);
      bAllowSkip = hs::param (parameters, "bAllowSkip", true);

      anymap addressSelectorGroupArgs = parameters;
      addressSelectorGroupArgs["layoutPath"] = "AddressSelectorGroup.yaml";
      addressSelectorGroupArgs["bIsPostalPage"] = false;
      m_addressSelectorGroup =
         new AddressSelectorGroup (UICreateFlags::Null, "Address Selector", this, gid::AddressSelectorGroupID,
                                   wxVERTICAL, " Address Selector ", addressSelectorGroupArgs);
      addGroup (m_addressSelectorGroup);

      anymap streetAddressGroupArgs = parameters;
      streetAddressGroupArgs["layoutPath"] = "AddressGroup.yaml";
      streetAddressGroupArgs["bIsPostalPage"] = false;
      m_streetAddressGroup =
         new AddressGroup (UICreateFlags::Null /*NoPlacement*/, "Physical Address", this, gid::PhysicalAddressGroupID,
                           wxVERTICAL, " Physical Address ", streetAddressGroupArgs);
      addGroup (m_streetAddressGroup);

      anymap postalAddressGroupArgs = parameters;
      postalAddressGroupArgs["layoutPath"] = "AddressGroup.yaml";
      postalAddressGroupArgs["bIsPostalPage"] = true;
      m_postalAddressGroup = new AddressGroup (UICreateFlags::Null, "Postal Address", this, gid::PostalAddressGroupID,
                                               wxVERTICAL, " Postal Address ", postalAddressGroupArgs);
      addGroup (m_postalAddressGroup);

      m_InfoBar = new InfoBar (UICreateFlags::ExcludeFromColour, "InfoBar", this, NewControlId());
      addControl (m_InfoBar);

      // Try to load the layout file and dynamically place the controls. If that fails, fall back to the
      // manual placement.

      bool autoLayoutSuccessful {};

#if defined(USING_YAML)

      auto layoutFile {
         hs::resourceName (UIType::FormLayout, "AddressPage.yaml", false, nullptr)}; // toml")}; // .json")};
      if ( !layoutFile.empty() ) {

         autoLayoutSuccessful = loadLayout (m_layout, getControlMap(), layoutFile);
      }
#endif

      if ( !autoLayoutSuccessful ) {
         m_layout.sizer->Clear(); // Fresh start

         // clang-format off
            auto bc = borderWidth;
            int cntrlFlags = stdCtrlFlags;

            placeControl (m_layout, m_addressSelectorGroup, 0,  0,  1,  1,  stdCtrlFlags, bc);
            placeControl (m_layout, m_streetAddressGroup,   1,  0,  1,  1,  stdCtrlFlags, bc);
            placeControl (m_layout, m_postalAddressGroup,   2,  0,  1,  1,  stdCtrlFlags, bc);
            placeControl (m_layout, m_InfoBar,              3,  0,  1,  1,  cntrlFlags, bc);

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
      GetPageSizer().Layout();
      SetSizerAndFit (&GetPageSizer());

      ctrlSignal().DetachFromSizer (gid::AddressSelectorGroupID, //      the Address Selector Group
                                  m_layout.sizer);             //      the wxGridBagSizer

      ctrlSignal().DetachFromSizer (gid::PhysicalAddressGroupID, //      the Physical Address Group
                                  m_layout.sizer);             //      the wxGridBagSizer

      ctrlSignal().DetachFromSizer (gid::PostalAddressGroupID,   //      the Postal Address Group
                                  m_layout.sizer);             //      the wxGridBagSizer
   }
};

auto AddressPage::OnPageKillActive (wxWizardEvent &event) -> void {
   TraceCall;

   m_wizard->Freeze();

   WizardPage::OnPageKillActive (event);

   if ( bInitialRun ) {
      auto button = m_wizard->FindWindow (wxID_HELP);
      if ( button ) {
         button->Show (false);
      }
   }

   m_wizard->Layout();
   m_wizard->Thaw();
}

auto AddressPage::OnPageSetActive (wxWizardEvent &event) -> void {
   TraceCall;

   m_wizard->Freeze();

   WizardPage::OnPageSetActive (event);

   if ( bInitialRun ) {
      auto button = dynamic_cast<wxButton *> (m_wizard->FindWindow (wxID_HELP));

      if ( button ) {
         button->Show (true);
         button->SetLabel (m_skipped ? "Unskip" : "Skip");
      }
   }

   m_wizard->Layout();
   m_wizard->Thaw();
}

auto AddressPage::OnWizardHelp (wxWizardEvent &event) -> void {
   TraceCall;

   m_skipped = !m_skipped;
   auto button = dynamic_cast<wxButton *> (m_wizard->FindWindow (wxID_HELP));

   if ( button ) {
      button->SetLabel (m_skipped ? "Unskip" : "Skip");
   }

   ctrlSignal().Notify (nid::Skipped, wx::nextID(), m_skipped);

   m_InfoBar->ShowMessageWithTimeout (m_skipped ? "Skipping address details for now. You can add it later."
                                                : "You can now enter the address details.",
                                      5000ms);

   WizardPage::OnWizardHelp (event);
}

} // namespace wx
