module;
#include "Core/Core.h"
#include <wx/wx.h>
#include <wx/wizard.h>
#include "Gfx/WidgetsFwd.h"

#include <complex>

export module Welcome.Page;
export import WizardPage;

import MarkupText;
import DDT;
import Types;
import wxTypes;
import wxUtil;
import Interface;

namespace wx {
export class WelcomePage : public WizardPage {
public:
   ~WelcomePage() override = default;

   explicit WelcomePage(wxWizard *parent, std::string pageName = "", std::string hdrText = "",
                        const anymap &args = nullanymap)
      : WizardPage(parent, pageName, hdrText, args) {

      TraceCall;

      anymap a = args;
      a["wrap"] = 400;

      auto l1 = new MarkupText(UICreateFlags::Null, "dc", this, "We hope you enjoy the experience!");
      GetPageSizer().Add(l1->ctl(), 0, wxALL, borderWidth);
      GetPageSizer().AddStretchSpacer(1);
      auto l2 = new MarkupText(UICreateFlags::Null, "dc", this,
                               "Just a couple of questions and we'll be on our way. Because this is a new "
                               "document, you will need to fill in your name and datafile details.",
                               a);
      GetPageSizer().Add(l2->ctl(), 0, wxALL, borderWidth);
      GetPageSizer().AddStretchSpacer(1);
      auto l3 = new MarkupText(UICreateFlags::Null, "dc", this,
                               "<big>Please click on <b>Next</b> to begin.</big>");
      GetPageSizer().Add(l3->ctl(), 0, wxALL, borderWidth);

      SetSizerAndFit(&GetPageSizer(), false);
   }
};
} // namespace wx
