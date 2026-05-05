module;

//
// Created by geoffrey on 7/9/25.
//

#include "Core/CoreData.h"
#include "MyHealthGuru/MyHealthGuru.h"

#include <algorithm>

export module EmailSecurity.Choice;
import Choice;
import EmailSecurity.RS;
import StringUtil;
import wxUtil;

export namespace mc {

class EmailSecurityChoice : public wx::Choice<ID::Type> {
public:
   using Choice::Choice;

   auto loadFromDB() -> bool override {
      ctl_typed()->Clear();

      if (hs::isFileOpen()) {
         EmailSecurityRS securityRS(*db()->getTable("email_security"));
         auto records = securityRS.selectRecords();
         ctl_typed()->Append("", Util::getInstance().intToVoidPtr(static_cast<int>(ID::Null)));
         std::ranges::for_each(securityRS.records_, [this] (auto &row) {
            ctl_typed()->Append(row->fName, Util::getInstance().intToVoidPtr(row->id));
         });
      }
      return false;
   }
};
}
