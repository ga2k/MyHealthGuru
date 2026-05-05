module;
#include "Core/Core.h"
#include "Gfx/wx.h"

#include "Core/CoreData.h"
// #include "Gfx/Core-Code.h"
#include "Core/TraceLevel.h"
#include "Core/Util.h"
#include "Gfx/Widgets.h"

#include <Gfx/GlobalIDs.h>
#include <complex>
#include <unordered_set>

export module MiscUserItems.Group;

import Ctrl;
import DDT;
import Interface;
import ValidateSignals;
import Types;
import wxTypes;
import Util;
import Validator;

import Group;
import CheckBox;
import Combo;
import DateCtrl;
import Markup;
import MaskedEdit;
import Spin;
import TextCtrl;

namespace mc {
using namespace wx;

export class MyCareUserGroup : public Group {

   /*
    * Args required by nameGroup
    *
    * arg              type                    use
    * ---------------- ----------------------- --------------------------------------
    * layoutPath      std::filesystem::path   the layout file for the controls
    * idUser           ID::Type                the db iid for this user or ID::New
    *
    */

   std::filesystem::path layoutPath;
   bool bResidential{};
   ID::Type idType{ID::None};

protected:
   // clang-format off
   DatePicker  *m_DOB{};
   CheckBox    *m_WantPopupAlerts{};
   TextCtrl    *m_mcCardNumber{};
   CheckBox    *m_WantSMSAlerts{};
   SpinCtrl    *m_mcCardIndex{};
   CheckBox    *m_WantEmailAlerts{};
   CheckBox    *m_PBSConcession{};
   MarkupText  *m_ytdCopay{};
   MarkupText  *m_copayThreshold{};
   DatePicker  *m_DateReached{};
   Combo<int>  *m_GP{};
   Combo<int>  *m_Pharmacist{};

   // clang-format on

   ID::Type idUser{ID::None};

   auto OnWantPopupAlerts([[maybe_unused]] wxEvent &event) -> void {
   }

   auto OnWantSMSAlerts([[maybe_unused]] wxEvent &event) -> void {
      TraceCall;
      validateSignal().AllowEmpty(gid::UserGroupMobilePhoneID, !m_WantSMSAlerts->IsChecked());
   }

   auto OnWantEmailAlerts([[maybe_unused]] wxEvent &event) -> void {
      TraceCall;
      validateSignal().AllowEmpty(gid::UserGroupEmailAddressID, !m_WantEmailAlerts->IsChecked());
   }

public:
   auto onKillActive() -> void override;
   auto onSetActive() -> void override;

public:
   ~MyCareUserGroup() override = default;

   explicit MyCareUserGroup(UICreateFlags cflags, const std::string &name, wxWindow *pParent, wxWindowIDRef id, int orient,
                            const std::string &label, const const anymap &args = {})
      : Group(cflags, name, pParent, id, orient, label, args) {

      layoutPath = hs::param(args, "layoutPath", "");
      bResidential = hs::param(args, "bResidential", true);
      idUser = hs::param(args, "idUser", ID::New);

#define LLABEL_DEFAULTS wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL
#define RLABEL_DEFAULTS wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL

      m_WantPopupAlerts = new CheckBox(UICreateFlags::Null, "Want Popup Alerts", this, NewControlId(), false);
      m_WantPopupAlerts->addValidator(new GenericValidator(true, m_WantPopupAlerts->liveAddr<bool>()))
                       .createLabel(UICreateFlags::Null, "[Want Popups]", this, NewControlId(), "Want Popup Alerts",
                                    RLABEL_DEFAULTS)
                       .setToolTip(
                          "If ticked, the user will receive Popup Alerts for notifications about appointments, "
                          "vaccinations, referrals expiring, etc.")
                       .hookAndHandle(wxEVT_CHECKBOX, m_WantPopupAlerts,
                                      [this]([[maybe_unused]] wxEvent &event) { OnWantPopupAlerts(event); })
                       .show(bResidential);

      addControl(m_WantPopupAlerts);

      m_mcCardNumber = new TextCtrl(UICreateFlags::Null, "Medicare Number", this, NewControlId(), "",
                                    wxDefaultPosition, wxSize(200, wxDefaultCoord));
      m_mcCardNumber
         ->addValidator(new CapsValidator(true, m_mcCardNumber->liveAddr<std::string>(),
                                          [] { return settings()->useCaps(); }))
         .createLabel(UICreateFlags::Null, "[Medicare Number]", this, NewControlId(), "Medicare Number",
                      LLABEL_DEFAULTS)
         .setToolTip("The user's Medicare Card Number. Saved for reference but not used by myCare.");
      addControl(m_mcCardNumber);

      m_WantSMSAlerts = new CheckBox(UICreateFlags::Null, "Want SMS Alerts", this, NewControlId(), false);
      m_WantSMSAlerts->addValidator(new GenericValidator(true, m_WantSMSAlerts->liveAddr<bool>()))
                     .createLabel(UICreateFlags::Null, "[Want SMSs]", this, NewControlId(), "Want SMS Alerts",
                                  RLABEL_DEFAULTS)
                     .setToolTip("If ticked, the user will receive SMS Alerts for notifications about appointments, "
                        "vaccinations, referrals expiring, etc.")
                     .hookAndHandle(wxEVT_CHECKBOX, m_WantSMSAlerts,
                                    [this]([[maybe_unused]] wxEvent &event) { OnWantSMSAlerts(event); })
                     .show(bResidential);
      addControl(m_WantSMSAlerts);

      m_mcCardIndex = new SpinCtrl(UICreateFlags::Null, "Medicare Index", this, NewControlId(), wxNOT_FOUND);
      m_mcCardIndex->addValidator(new GenericValidator(true, m_mcCardIndex->liveAddr<int>()))
                   .createLabel(UICreateFlags::Null, "[Medicare Index]", this, NewControlId(), "Medicare Index",
                                LLABEL_DEFAULTS)
                   .setToolTip("he user's Medicare Card Index. Not used by myCare.");
      addControl(m_mcCardIndex);

      m_WantEmailAlerts = new CheckBox(UICreateFlags::Null, "Want Email Alerts", this, NewControlId(), false);
      m_WantEmailAlerts->addValidator(new GenericValidator(true, m_WantEmailAlerts->liveAddr<bool>()))
                       .createLabel(UICreateFlags::Null, "[Want Emails]", this, NewControlId(), "Want Email Alerts",
                                    RLABEL_DEFAULTS)
                       .setToolTip(
                          "If ticked, the user will receive Email Alerts for notifications about appointments, "
                          "vaccinations, referrals expiring, etc.")
                       .hookAndHandle(wxEVT_CHECKBOX, m_WantEmailAlerts,
                                      [this]([[maybe_unused]] wxEvent &event) { OnWantEmailAlerts(event); })
                       .show(bResidential);
      addControl(m_WantEmailAlerts);

      m_PBSConcession = new CheckBox(UICreateFlags::Null, "Have PBS Concession", this, NewControlId(), false);
      m_PBSConcession->addValidator(new GenericValidator(true, m_PBSConcession->liveAddr<bool>()))
                     .createLabel(UICreateFlags::Null, "[Want Popups]", this, NewControlId(), "Have PBS Concession",
                                  LLABEL_DEFAULTS)
                     .setToolTip(
                        "If ticked, the user is entitled to the Concessional rate for Threshold and Copayment. "
                        "Otherwise the General rates apply.")
                     .hookAndHandle(wxEVT_CHECKBOX, m_PBSConcession,
                                    [this]([[maybe_unused]] wxEvent &event) { OnWantPopupAlerts(event); })
                     .show(idUser != ID::New && bResidential);
      addControl(m_PBSConcession);

      (m_DOB = new DatePicker(UICreateFlags::Null, "Date of Birth", this, NewControlId(),
                              wxDateTime(wx::Tm2wxDateTime(nulltm))))
         ->addValidator(new DateValidator(true, m_DOB->liveAddr<wxDateTime>()))
         .createLabel(UICreateFlags::Null, "[Date of Birth]", this, NewControlId(), "Date of Birth", LLABEL_DEFAULTS)
         .setToolTip("Date of birth")
         .show(bResidential);
      addControl(m_DOB);

      m_ytdCopay = new MarkupText(UICreateFlags::Null, "YTD Copay", this, NewControlId(), "", wxDefaultPosition,
                                  wxSize{200, wxDefaultCoord});
      m_ytdCopay
         ->addValidator(
            new CapsValidator(true, m_ytdCopay->liveAddr<std::string>(), [] { return settings()->useCaps(); }))
         .createLabel(UICreateFlags::Null, "[YTD Copay]", this, NewControlId(), "YTD Copay", wxDefaultPosition,
                      wxSize{100, wxDefaultCoord}, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL)
         .setToolTip("he Year-To-Date amount the user has copayed so far. (Calculated)");
      addControl(m_ytdCopay);

      m_copayThreshold = new MarkupText(UICreateFlags::Null, "Copay Threshold", this, NewControlId(), "",
                                        wxDefaultPosition, wxSize(200, wxDefaultCoord));
      m_copayThreshold
         ->addValidator(new CapsValidator(true, m_copayThreshold->liveAddr<std::string>(),
                                          [] { return settings()->useCaps(); }))
         .createLabel(UICreateFlags::Null, "[Copay Threshold]", this, NewControlId(), "Copay Threshold",
                      wxDefaultPosition, wxSize{100, wxDefaultCoord}, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL)
         .setToolTip("The Threshold that applies to this user. (Calculated)");
      addControl(m_copayThreshold);

      m_DateReached = new DatePicker(UICreateFlags::Null, "Date Reached", this, NewControlId(),
                                     wxDateTime(01, wxDateTime::Month::Jan, 1970));
      m_DateReached->addValidator(new DateValidator(true, m_DateReached->liveAddr<wxDateTime>()))
                   .createLabel(UICreateFlags::Null, "[Date Reached]", this, NewControlId(), "Date Reached",
                                LLABEL_DEFAULTS)
                   .setToolTip("Date the co-payment threshold was reached.")
                   .show(bResidential);
      addControl(m_DateReached);

      m_GP = new Combo<int>(UICreateFlags::Null, "GP", this, NewControlId(), hs::string_v{}, wxDefaultSize,
                            wxCB_DROPDOWN | wxTE_PROCESS_ENTER | wxCB_READONLY);
      m_GP
         ->addValidator(
            new ComboLikeValidator<Combo<int>>(true, m_GP->liveAddr<int>(), hs::TransferModel::ByIndex))
         .createLabel(UICreateFlags::Null, "[Title]", this, NewControlId(), "GP", LLABEL_DEFAULTS)
         .setToolTip("Select the persons preferred Doctor")
         .show(idUser != ID::New && bResidential);
      addControl(m_GP);

      m_Pharmacist =
         new Combo<int>(UICreateFlags::Null, "Pharmacist", this, NewControlId(), hs::string_v{}, wxDefaultSize,
                        wxCB_DROPDOWN | wxTE_PROCESS_ENTER | wxCB_READONLY);
      m_Pharmacist
         ->addValidator(
            new ComboLikeValidator<Combo<int>>(true, m_Pharmacist->liveAddr<int>(), hs::TransferModel::ByIndex))
         .createLabel(UICreateFlags::Null, "[Title]", this, NewControlId(), "Pharmacist", LLABEL_DEFAULTS)
         .setToolTip("Select the persons preferred Pharmacist")
         .show(idUser != ID::New && bResidential);
      addControl(m_Pharmacist);

      // Try to load the layout file and dynamically place the controls. If that fails, fall back to the
      // manual placement.

      bool autoLayoutSuccessful{};

#if defined(USING_YAML)

      auto layoutFile{hs::resourceName(UIType::FormLayout, layoutPath.string(), false, nullptr)};
      if (!layoutFile.empty()) {

         autoLayoutSuccessful = loadLayout(layoutFile);
      }
#endif
      if (!autoLayoutSuccessful) {
         m_layout.sizer->Clear(); // Fresh start

         // clang-format off
            auto bc = borderWidth;
            auto bl = borderWidth;
            int cntrlFlags = stdCtrlFlags;
            int stdLabelFlags = wxALIGN_CENTRE_VERTICAL | wxALL | wxALIGN_RIGHT;

            placeControl (m_layout, m_WantPopupAlerts,          0,  0,  1,  1,  cntrlFlags, bc,
                                    m_WantPopupAlerts->label(), 0,  1,  1,  1,  stdLabelFlags, bl);
            placeControl (m_layout, m_mcCardNumber->label(),    0,  2,  1,  1,  stdLabelFlags, bl,
                                    m_mcCardNumber,             0,  3,  1,  1,  cntrlFlags, bc);
            placeControl (m_layout, m_WantSMSAlerts,            1,  0,  1,  1,  cntrlFlags, bc,
                                    m_WantSMSAlerts->label(),   1,  1,  1,  1,  stdLabelFlags, bl);
            placeControl (m_layout, m_mcCardIndex->label(),     1,  2,  1,  1,  stdLabelFlags, bl,
                                    m_mcCardIndex,              1,  3,  1,  1,  cntrlFlags, bc);
            placeControl (m_layout, m_WantEmailAlerts,          2,  0,  1,  1,  cntrlFlags, bc,
                                    m_WantEmailAlerts->label(), 2,  1,  1,  1,  stdLabelFlags, bl);
            placeControl (m_layout, m_PBSConcession->label(),   2,  2,  1,  1,  stdLabelFlags, bl,
                                    m_PBSConcession,            2,  3,  1,  1,  cntrlFlags, bc);

            placeControl (m_layout, m_DOB->label(),             3,  0,  1,  1,  stdLabelFlags, bl,
                                    m_DOB,                      3,  1,  1,  1,  stdCtrlFlags, bc);

            placeControl (m_layout, m_ytdCopay->label(),        4,  0,  1,  1,  stdLabelFlags, bl,
                                    m_ytdCopay,                 4,  1,  1,  1,  cntrlFlags, bc);
            placeControl (m_layout, m_copayThreshold->label(),  4,  2,  1,  1,  stdLabelFlags, bl,
                                    m_copayThreshold,           4,  3,  1,  1,  cntrlFlags, bc);
            placeControl (m_layout, m_DateReached->label(),     5,  0,  1,  1,  stdLabelFlags, bl,
                                    m_DateReached,              5,  1,  1,  1,  cntrlFlags, bc);

            placeControl (m_layout, m_GP->label(),              7,  0,  1,  1,  stdLabelFlags, bl,
                                    m_GP,                       7,  1,  1,  3,  cntrlFlags, bc);
            placeControl (m_layout, m_Pharmacist->label(),      8,  0,  1,  1,  stdLabelFlags, bl,
                                    m_Pharmacist,               8,  1,  1,  3,  cntrlFlags, bc);

         // clang-format on
      }

      for (auto i : std::views::iota(0, m_layout.sizer->GetEffectiveColsCount())) {
         if (!m_layout.sizer->IsColGrowable(i))
            m_layout.sizer->AddGrowableCol(i, 1);
      }
      for (auto i : std::views::iota(0, m_layout.sizer->GetEffectiveRowsCount())) {
         if (!m_layout.sizer->IsRowGrowable(i))
            m_layout.sizer->AddGrowableRow(i, 1);
      }
   }
};

auto MyCareUserGroup::onKillActive() -> void {

   Group::onKillActive();

   TraceIf(TraceLevel::Trace) {
      // clang-format off
        dout << "\x1b[33;1m" << std::setw (30) << m_WantPopupAlerts->label()->creationName() << "\x1b[0m" << std::setw (20) << m_WantPopupAlerts->orig<bool>()           << "\x1b[36;1m <- Old \x1b[0m:\x1b[32;1m New -> \x1b[0m" << m_WantPopupAlerts->live<bool>()         << std::endl;
        dout << "\x1b[33;1m" << std::setw (30) << m_mcCardNumber->label()->creationName()    << "\x1b[0m" << std::setw (20) << m_mcCardNumber->orig<std::string>()       << "\x1b[36;1m <- Old \x1b[0m:\x1b[32;1m New -> \x1b[0m" << m_mcCardNumber->live<std::string>()     << std::endl;
        dout << "\x1b[33;1m" << std::setw (30) << m_WantSMSAlerts->label()->creationName()   << "\x1b[0m" << std::setw (20) << m_WantSMSAlerts->orig<bool>()             << "\x1b[36;1m <- Old \x1b[0m:\x1b[32;1m New -> \x1b[0m" << m_WantSMSAlerts->live<bool>()           << std::endl;
        dout << "\x1b[33;1m" << std::setw (30) << m_mcCardIndex->label()->creationName()     << "\x1b[0m" << std::setw (20) << m_mcCardIndex->orig<int>()                << "\x1b[36;1m <- Old \x1b[0m:\x1b[32;1m New -> \x1b[0m" << m_mcCardIndex->live<int>()              << std::endl;
        dout << "\x1b[33;1m" << std::setw (30) << m_WantEmailAlerts->label()->creationName() << "\x1b[0m" << std::setw (20) << m_WantEmailAlerts->orig<bool>()           << "\x1b[36;1m <- Old \x1b[0m:\x1b[32;1m New -> \x1b[0m" << m_WantEmailAlerts->live<bool>()         << std::endl;
        dout << "\x1b[33;1m" << std::setw (30) << m_PBSConcession->label()->creationName()   << "\x1b[0m" << std::setw (20) << m_PBSConcession->orig<bool>()             << "\x1b[36;1m <- Old \x1b[0m:\x1b[32;1m New -> \x1b[0m" << m_PBSConcession->live<bool>()           << std::endl;
        // dout << "\x1b[33;1m" << std::setw (30) << m_DOB->label()->creationName()             << "\x1b[0m" << std::setw (20) << m_DOB->orig<wxDateTime>()                 << "\x1b[36;1m <- Old \x1b[0m:\x1b[32;1m New -> \x1b[0m" << m_DOB->live<wxDateTime>()               << std::endl;
        dout << "\x1b[33;1m" << std::setw (30) << m_ytdCopay->label()->creationName()        << "\x1b[0m" << std::setw (20) << m_ytdCopay->orig<std::string>()           << "\x1b[36;1m <- Old \x1b[0m:\x1b[32;1m New -> \x1b[0m" << m_ytdCopay->live<std::string>()         << std::endl;
        dout << "\x1b[33;1m" << std::setw (30) << m_copayThreshold->label()->creationName()  << "\x1b[0m" << std::setw (20) << m_copayThreshold->orig<std::string>()     << "\x1b[36;1m <- Old \x1b[0m:\x1b[32;1m New -> \x1b[0m" << m_copayThreshold->live<std::string>()   << std::endl;
        // dout << "\x1b[33;1m" << std::setw (30) << m_DateReached->label()->creationName()     << "\x1b[0m" << std::setw (20) << m_DateReached->orig<wxDateTime>()         << "\x1b[36;1m <- Old \x1b[0m:\x1b[32;1m New -> \x1b[0m" << m_DateReached->live<wxDateTime>()       << std::endl;
        dout << "\x1b[33;1m" << std::setw (30) << m_GP->label()->creationName()              << "\x1b[0m" << std::setw (20) << m_GP->orig<int>()                         << "\x1b[36;1m <- Old \x1b[0m:\x1b[32;1m New -> \x1b[0m" << m_GP->live<int>()                       << std::endl;
        dout << "\x1b[33;1m" << std::setw (30) << m_Pharmacist->label()->creationName()      << "\x1b[0m" << std::setw (20) << m_Pharmacist->orig<int>()                 << "\x1b[36;1m <- Old \x1b[0m:\x1b[32;1m New -> \x1b[0m" << m_Pharmacist->live<int>()               << std::endl;
      // clang-format on
   }
}

auto MyCareUserGroup::onSetActive() -> void {

   Group::onSetActive();
}

} // namespace mc
