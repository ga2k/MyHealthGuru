module;
#include "Core/CoreData.h"

#include "Core/Core.h"
#include "Core/Util.h"
#include "Gfx/Widgets.h"
#include "Gfx/wx.h"

export module Filename.Page;
export import WizardPage;

import Filename.Group;
import Markup;
import InfoBar;

import DDT;
import Types;
import wxTypes;
import Util;

namespace wx {
export class FilenamePage : public WizardPage {

   /*
    * Args required by FilenamePage
    *
    * arg              type                    use
    * ---------------- ----------------------- --------------------------------------
    * bInitialRun      bool                    this is the first run of this program
    * layoutPath       std::filesystem::path   the layout file for the controls
    * bAllowSkip       bool                    user can elect to skip entering address
    *
    */

   bool bInitialRun {};
   std::filesystem::path layoutPath;
   bool bAllowSkip {};

   // clang-format off
    FilenameGroup*   m_filenameGroup {};
    InfoBar*         m_InfoBar {};
    wxWizard                   *m_wizard {};
   // clang-format on

 public:
   ~FilenamePage() override = default;
   explicit FilenamePage (wxWizard *parent, const std::string &pageName = "", const std::string &introduction = "",
                          anymap parameters = {})
      : WizardPage (parent, pageName, introduction, parameters) {

      bInitialRun = hs::param (parameters, "bInitialRun", false);
      layoutPath = hs::param (parameters, "layoutPath", "AddressPage.yaml");
      bAllowSkip = hs::param (parameters, "bAllowSkip", true);

      anymap filenameGroupArgs = parameters;
      filenameGroupArgs["layoutPath"] = "FilenameGroup.yaml";
      m_filenameGroup = new FilenameGroup (UICreateFlags::Null, "Datafile", this, NewControlId(), wxVERTICAL,
                                           " Datafile ", filenameGroupArgs);
      addGroup (m_filenameGroup);

      m_InfoBar = new InfoBar (UICreateFlags::ExcludeFromColour, "InfoBar", this, NewControlId(), 500,
                               wxSHOW_EFFECT_SLIDE_TO_TOP, wxSHOW_EFFECT_SLIDE_TO_BOTTOM);
      addControl (m_InfoBar);

      m_layout.sizer->SetNonFlexibleGrowMode (wxFLEX_GROWMODE_ALL);

      // Try to load the layout file and dynamically place the controls. If that fails, fall back to the
      // manual placement.

      bool autoLayoutSuccessful {};

#if defined(USING_YAML)

      auto layoutFile {hs::resourceName (UIType::FormLayout, "FilenamePage.yaml", false, nullptr)};
      if ( !layoutFile.empty() ) {

         autoLayoutSuccessful = loadLayout (m_layout, getControlMap(), layoutFile);
      }
#endif

      if ( !autoLayoutSuccessful ) {
         m_layout.sizer->Clear(); // Fresh start

         // clang-format off

            placeControl (m_layout, m_filenameGroup,    0,  0,  1,  1,  stdLabelFlags, borderWidth);
            placeControl (m_layout, m_InfoBar,          1,  0,  1,  1,  stdCtrlFlags, borderWidth);
        }

        for ( auto i : std::views::iota (0, m_layout.sizer->GetEffectiveColsCount()) ) {
            if ( !m_layout.sizer->IsColGrowable (i) )
                m_layout.sizer->AddGrowableCol (i, 1);
        }
        for ( auto i : std::views::iota (0, m_layout.sizer->GetEffectiveRowsCount()) ) {
            if ( !m_layout.sizer->IsRowGrowable (i) )
                m_layout.sizer->AddGrowableRow (i, 1);
        }

        GetPageSizer().Add (m_layout.sizer, 1, stdCtrlFlags, borderWidth);
        SetSizerAndFit (&GetPageSizer());
    }
};
} // namespace wx
