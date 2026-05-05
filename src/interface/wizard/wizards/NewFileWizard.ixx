module;

#include "Core/Core.h"
#include "Core/CoreData.h"
#include "Core/Util.h"
#include <wx/wx.h>
#include <wx/wizard.h>
#include "MyHealthGuru/GlobalIDs.h"
#include "Gfx/WidgetsFwd.h"

export module NewFile.Wizard;
import Wizard;
import WizardPage;

import Welcome.Page;
import NewDatafile.WizardPage;

import Ctrl;

import Util;
import DDT;
import Types;
import CtrlSignals;
import SplashSignals;
import InterfaceController;

namespace wx {
export class NewFileWizard : public Wizard {

   /* Args passed to this wizard

           bInitialRun      true;
   */

   bool bInitialRun{};

   // Get any more you need from Wizard::Wizard
   auto OnWizardCancel(wxWizardEvent &event) -> void override;

public:
   explicit NewFileWizard(wxFrame *frame, std::string title, const anymap &args)
      :  Wizard(frame, title, args) {

      TraceCall;

      bInitialRun = param<bool>(args, "bInitialRun", false);

      if (bInitialRun) {
         addPage(new WelcomePage(this, "Welcome", "<big>Welcome to the <b>First Run</b> wizard.</big>", args));
      }

      addPage(new NewDatafileWizardPage(
         UICreateFlags::ExcludeFromColour | UICreateFlags::Group | UICreateFlags::NoValidation | UICreateFlags::NoDefaultBinds,
         "Basic Details",
         this,
         bInitialRun
            ? "<big>Just the basic facts...</big>"s
            : "<big>Welcome to the <b>New Datafile</b> wizard.</big>"s,
         args,
         0L));

      ctrlSignal().Notify(wx::nextID(), nid::WizardSetupComplete);
      hs::splashSignal().Close();

   }
};

auto NewFileWizard::OnWizardCancel(wxWizardEvent &event) -> void {
   TraceCall;
   event.Skip();
   wxMessageBox("The wizard was canceled.\n\nSetup is incomplete; exiting", "Wizard notification",
                wxICON_ERROR | wxOK | wxCENTER, this);
}
} // namespace wx
