module;

//
// Created by geoffrey on 7/9/25.
//

#include "Core/CoreData.h"
#include "MyCare/MyCare.h"
#include <algorithm>

export module Titles.Choice;
import Choice;
import Titles.RS;
import StringUtil;
import wxUtil;
import ComboLikeBase;

export namespace mc {

   class TitlesChoice : public wx::Choice<ID::Type> {
      public:
	  using Choice::Choice;

//       TitlesChoice (wx::UICreateFlags cflags, std::string name, wxWindow *parent, wx::value_t value, const anymap &args = nullanymap,
//               long style = defaultStyle, const wxSize &size = wxDefaultSize)
//          : base (cflags, name, parent, value,
//                  new wxChoice (parent, wx::nextID(), wxDefaultPosition, wxDefaultSize,
//                                wxArrayString(), style),
//                  args, style, wx::get_DefaultChoice()) {
//          setTransferModel (wx::ComboTraits<ID::Type>::model);
//          this->orig (this->live());
//          this->set (this->live());
//      }

      auto loadFromDB() -> bool override {
   		 auto *c = this->ctl_typed();

          if (hs::isFileOpen()) {
             TitlesRS titleRS(*db()->getTable("titles"));
             auto records = titleRS.selectRecords();
             c->Append("", Util::getInstance().intToVoidPtr(static_cast<int>(ID::Null)));
             for (auto &row : titleRS.records_) {
                c->Append(row->fAbbreviation, Util::getInstance().intToVoidPtr(row->id));
             }
          } else {
             int id = 0;
             c->Append("", Util::getInstance().intToVoidPtr(static_cast<int>(ID::Null)));
             std::ranges::for_each(hs::titles(), [this, c, &id] (auto &n) {
                c->Append(wx::to_wxString(n.abbreviation), Util::getInstance().intToVoidPtr(++id));
             });
          }
         return true;
      }
   };
}
