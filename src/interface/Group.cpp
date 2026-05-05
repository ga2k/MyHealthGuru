module;

#include "Gfx/GlobalIDs.h"
#include "Gfx/gfx_export.h"
#include "Core/Core.h"
#include "Gfx/wx.h"
#include "Gfx/Widgets.h"
#include "Core/Util.h"

module Group;

import Ctrl;

import StaticBox;

import DDT;
import Types;
import wxTypes;
import Validator;
import Util;
import OrderedMap;
import CtrlSignals;
import wxStringConverter;
import Interface;

namespace wx {
Group::~Group() {
   if (subscriptionHandles) {
      ctrlSignal().Unsubscribe(subscriptionHandles);
      subscriptionHandles = 0;
   }
}

Group::Group(UICreateFlags cflags, std::string name, wxWindow *pParent, value_t value, const anymap &args, long style)
   : Interface(ctl(), param<int>(args, "columns", 1),
               param<int>(args, "rows", 1),
               true,
               param<int>(args, "gapV", borderWidth),
               param<int>(args, "gapH", borderWidth)),
     StaticBox(cflags | UICreateFlags::Group | UICreateFlags::Heading | UICreateFlags::NoNotifyChanged,
               name,
               pParent,
               value),
     m_sizer(new wxStaticBoxSizer(ctl_typed(), wxVERTICAL)),
     m_params(args), m_createFlags(cflags), m_name(name) {

   m_sizer->Add(&this->grid(), 1, wxGROW | wxALL, borderWidth);
   Ctrl::ctl()->SetClientData(m_sizer);

   subscriptionHandles = ctrlSignal().Subscribe(
      [this](std::shared_ptr<hs::Evt<CtrlEvent>> event) {
         if (event->ctrlID == ctl()->GetId()) {
            event->stop();
         } else if (event->ctrlID != wx::nextID()) {
            return;
         }
         this->onCtrlEvent(event);
      },
      CtrlEvent::AttachToSizer, //
      CtrlEvent::Changed, //
      CtrlEvent::DetachFromSizer, //
      CtrlEvent::Enable, //
      CtrlEvent::Notify); //
}

auto Group::onCtrlEvent(std::shared_ptr<hs::Evt<CtrlEvent>> event) -> void {
   switch (event->m_type) {
      case CtrlEvent::Changed: {
         //
         if (isCtrlInGroup(event->c_int[0])) {
            auto ctrl = this->controlMap()[event->c_int[0]];
            this->groupChanged(true);
            std::cout << ctrl->creationName() << " has changed" << std::endl;
         }
      }
      break;

      case CtrlEvent::Enable: //
         ctl()->Enable(event->c_bool[0]);
         enableCtrl(event->c_bool[0]);
         break;

      case CtrlEvent::DetachFromSizer: { //
         if (event->c_bool[0] == true) {
            if (!isAnyOfType<wxGridBagSizer *>(event->c_any[0])) {
               TraceIf(TraceLevel::Error)
                  dout << "Error in Group; Not a wxGridBagSizer* in event" << std::endl;
               return;
            }
            m_originalSizer = std::any_cast<wxGridBagSizer *>(event->c_any[0]);
         }
         if (!m_originalSizer) {
            TraceIf(TraceLevel::Error)
               dout << "Error in Group; Call to detach from unknown sizer" << std::endl;
            return;
         }
         auto sbsizer = static_cast<wxStaticBoxSizer *>(ctl()->GetClientData());
         // return layout.sizer->Add (sbsizer, newPos, item.span, actualFlags, item.width) != nullptr;

         wxGBPosition pos;
         wxGBSpan span;
         bool rCode{};

         if (m_originalSizer->FindItem(sbsizer)) {
            pos = m_originalSizer->GetItemPosition(sbsizer);
            span = m_originalSizer->GetItemSpan(sbsizer);

            rCode = m_originalSizer->Detach(sbsizer); // this->getSizer());
            ctl()->Hide();
         }
         suspendValidation(true);
         m_originalSizer->Layout();
         doutif(TraceLevel::Debug) << std::format("Detaching {} at {}, {}  from sizer: {}", this->creationName(),
                                                  hs::wxStringConverter<wxGBPosition>::toString(pos),
                                                  hs::wxStringConverter<wxGBSpan>::toString(span), rCode)
                   << std::endl;
      }
      break;

      case CtrlEvent::AttachToSizer: {

         int index = 0; // Assuming there is no sizer in [0]
         if (event->c_bool[0] == true) {

            if (!isAnyOfType<wxGridBagSizer *>(event->c_any[index])) {
               TraceIf(TraceLevel::Error)
                  dout << "Error in Group; Not a wxGridBagSizer* in event" << std::endl;
               return;
            }
            m_originalSizer = std::any_cast<wxGridBagSizer *>(event->c_any[index++]);
         }

         if (!m_originalSizer) {
            TraceIf(TraceLevel::Error)
               dout << "Error in Group; Call to attach to unknown sizer" << std::endl;
            return;
         }

         wxGBPosition pos{};
         wxGBSpan span{};
         int flags{stdCtrlFlags};
         int padding{borderWidth};

         if (!isAnyOfType<wxGBPosition>(event->c_any[index])) {
            TraceIf(TraceLevel::Error)
               dout << "Error in Group; No position given in Attach message" << std::endl;
            return;
         }
         pos = std::any_cast<wxGBPosition>(event->c_any[index++]);

         if (!isAnyOfType<wxGBSpan>(event->c_any[index])) {
            TraceIf(TraceLevel::Error)
               dout << "Error in Group; No span given in Attach message" << std::endl;
            return;
         }
         span = std::any_cast<wxGBSpan>(event->c_any[index++]);

         auto sbsizer = static_cast<wxStaticBoxSizer *>(ctl()->GetClientData());
         bool rCode = m_originalSizer->Add(sbsizer, pos, span, flags, padding) != nullptr;
         ctl()->Show();
         suspendValidation(false);
         m_originalSizer->Layout();
         doutif(TraceLevel::Debug) << std::format("Attaching {} at {}, {} to sizer: {}", this->creationName(),
                                                  hs::wxStringConverter<wxGBPosition>::toString(pos),
                                                  hs::wxStringConverter<wxGBSpan>::toString(span), rCode)
                   << std::endl;

      }
      break;
      case CtrlEvent::Notify: {
         switch (event->c_int[0]) {
            case nid::Skipped: {
               groupSkipped(event->c_bool[0]);
            }
         }
         break;
      }
      break;
      default: break;
   }
}
} // namespace wx
