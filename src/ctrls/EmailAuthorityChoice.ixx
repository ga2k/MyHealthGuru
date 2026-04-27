module;

//
// Created by geoffrey on 7/9/25.
//

#include "Core/CoreData.h"
#include "MyCare/MyCare.h"

#include <algorithm>

export module EmailAuthority.Choice;
import Choice;
import EmailAuthority.RS;
import StringUtil;
import wxUtil;

export namespace mc {

class EmailAuthorityChoice : public wx::Choice<ID::Type> {
public:
   using Choice::Choice;

   auto loadFromDB() -> bool override {
      ctl_typed()->Clear();

      if (hs::isFileOpen()) {
         EmailAuthorityRS authorityRS(*db()->getTable("email_authority"));
         auto records = authorityRS.selectRecords();
         ctl_typed()->Append("", Util::getInstance().intToVoidPtr(static_cast<int>(ID::Null)));
         std::ranges::for_each(authorityRS.records_, [this] (auto &row) {
            ctl_typed()->Append(row->fName, Util::getInstance().intToVoidPtr(row->id));
         });
      }
      return false;
   }
};
}
