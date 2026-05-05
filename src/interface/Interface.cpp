module;

//
// Created by geoffrey on 14/8/25.
//

#include "Core/CoreData.h"
#include "Core/Core.h"
// #include "Core/Sizes.h"
#include <unordered_map>
#include <string>
#include <ranges>
#include <wx/aui/auibook.h>

module Interface;

import Book;
import Ctrl;
import DBaseUI;
import DDT;
import Group;
import MarkupText;
import OrderedMap;
import Page;
import StaticText;
import String;
import Validator;
import Wizard;
import WizardPage;
import wxStringConverter;
import wxTypes;
import wxUtil;

namespace wx {

Interface::Interface(wxWindow *pParent, int cols, int rows, bool isGroup, int gapV, int gapH, const String& wizardPageName)
   : m_pParent(pParent) {

   if (!wizardPageName.empty())
      m_name = static_cast<std::string>(wizardPageName);
   else
   {
      m_name = static_cast<std::string>(pParent->GetName());
   }
   m_isGroup = isGroup;

   m_grid = new wxFlexGridSizer(0);
   initialize(cols, rows);
   m_grid->SetVGap(gapV);
   m_grid->SetHGap(gapH);

}

auto Interface::addControl(ICtrl *pCtrl2Add) -> void {
   m_controls.insert_or_assign(pCtrl2Add->creationID(), pCtrl2Add);
   m_unified.insert_or_assign(pCtrl2Add->creationID(), pCtrl2Add);
}

auto Interface::addGroup(ICtrl *pGroup2Add) -> void {
   m_groups.insert_or_assign(pGroup2Add->creationID(), pGroup2Add);
   m_unified.insert_or_assign(pGroup2Add->creationID(), pGroup2Add);
}

auto Interface::isCtrlInGroup(int ctrlID) -> bool { return m_unified.contains(ctrlID); }

auto Interface::onSetActive(bool autoEnableOnSetActive) -> void {
   dout << "on Set Active \t" << m_pParent->GetName() << std::endl;

   if (autoEnableOnSetActive) {
      hs::isEditing(true);
      ICtrl::transferTheseToWindow(m_controls);
      ICtrl::refreshPage(m_controls, hs::PageState::Enable | hs::PageState::ReflectState);
   }
   for (auto &&[id, group] : m_groups) {
      dynamic_cast<Group *>(group)->onSetActive(autoEnableOnSetActive);
   }
}

auto Interface::onKillActive(bool autoDisableOnKillActive) -> void {
   dout << "on Kill Active\t" << m_pParent->GetName() << std::endl;

   if (autoDisableOnKillActive) {
      hs::isEditing(false);
      ICtrl::transferTheseFromWindow(m_controls);
      ICtrl::refreshPage(m_controls, hs::PageState::Disable | hs::PageState::ReflectState);
      m_validationResults = ICtrl::validatePage(m_controls);

      for (auto &&[id, group] : m_groups) {
         dynamic_cast<Group *>(group)->onKillActive(autoDisableOnKillActive);
      }
   }
}

auto Interface::onEvent(db::DBaseUIEvent event) -> void {
   dout << "on Event\t" << m_pParent->GetName() << std::endl;

   for (auto &&[id, group] : m_groups) {
      dynamic_cast<Group *>(group)->onEvent(event);
   }

   switch (event) {
      case db::DBaseUIEvent::Edit: //

         ICtrl::transferTheseToWindow(m_controls);
         ICtrl::refreshPage(m_controls, hs::PageState::ReflectState);
         break;

      case db::DBaseUIEvent::Commit: //

         ICtrl::transferTheseFromWindow(m_controls);
         ICtrl::refreshPage(m_controls, hs::PageState::Disable);
         m_validationResults = ICtrl::validatePage(m_controls);
         break;

      case db::DBaseUIEvent::Rollback: //

         for (auto &control : m_controls | std::views::values) {
            (void) control->undo();
            control->enableCtrl(false);
            control->isErrored(false, "");
            // control->touchedByFocus(false);

            control->labelInvoke(
               hs::LIX::All,
               [control](ICtrl::StaticDetails label, const std::string &state, bool touchedByFocus) {
//                   label.first->SetForegroundColour(control->foregroundColour("Label"));
//                   label.first->SetBackgroundColour(control->backgroundColour("Label"));
                  wxFont f{label.first->GetFont()};
                  f.SetWeight(ICtrl::settings()->useBoldLabels() ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL);
                  label.first->SetFont(f);
               },
               control->getState(), control->touchedByFocus());
         }

         ICtrl::transferTheseFromWindow(m_controls);
         ICtrl::refreshPage(m_controls, hs::PageState::Disable);
         break;

      default: break;
   }
}

auto Interface::addItemToSizer(wxControl *item, const std::string &section, int row, int col, int itemFlags, int proportion,
                               int itemBorder,
                               bool hide) -> bool {
   return doAddItemToSizer(item, ParsedItem{
                              .ctrl = item,
                              .section = section,
                              .name = item ? std::string(item->GetName().ToUTF8()) : std::string{},
                              .lix = hs::LIX::Null,
                              .row = row,
                              .col = col,
                              .flags = itemFlags,
                              .border = itemBorder,
                              .hide = hide,
                              .proportion = proportion});
}

auto Interface::doAddItemToSizer(wxControl *item, const std::string &section, int row, int col, int itemFlags,
                                 int proportion, int itemBorder,
                                 bool hide) -> bool {
   return doAddItemToSizer(item, ParsedItem{
                              .ctrl = item,
                              .section = section,
                              .name = item ? std::string(item->GetName().ToUTF8()) : std::string{},
                              .lix = hs::LIX::Null,
                              .row = row,
                              .col = col,
                              .flags = itemFlags,
                              .border = itemBorder,
                              .hide = hide,
                              .proportion = proportion});
}

auto Interface::doAddItemToSizer(wxControl *ctrl, const ParsedItem &item) -> bool {

   if (first) {
      first = false;

      TraceIf(TraceLevel::Info) {
         // clang-format off
         dout  << "\x1b[0;1;3m" << std::setw(20) << "Action"
               << "\x1b[0;1;2m" << std::setw(25) << "Section"
               << "\x1b[0;1m"   << std::setw(25) << "Item Name"
               << "\x1b[0;32m"  << std::setw(8)  << "WinID"
               << "\x1b[0;33m"  << std::setw(4)  << "Row"
               << "\x1b[0;33m"  << std::setw(4)  << "Col"
               << "\x1b[0;34m"  << std::setw(4)  << "Pro"
               << "\x1b[0;34m"  << std::setw(4)  << "Bdr"
               << "\x1b[0;35m"  << std::setw(4)  << "Hid"
               << "\x1b[0;36m"  << " Sizer Flags" << std::endl;

         dout  << "\x1b[0;1;3m" << std::setw(20) << "-----------------"
               << "\x1b[0;1;2m" << std::setw(25) << "------------------------"
               << "\x1b[0;1m"   << std::setw(25) << "------------------------"
               << "\x1b[0;32m"  << std::setw(8)  << "-------"
               << "\x1b[0;33m"  << std::setw(4)  << "---"
               << "\x1b[0;33m"  << std::setw(4)  << "---"
               << "\x1b[0;34m"  << std::setw(4)  << "---"
               << "\x1b[0;34m"  << std::setw(4)  << "---"
               << "\x1b[0;35m"  << std::setw(4)  << "---"
               << "\x1b[0;36m"  << " ---------------------------------------" << std::endl;
         // clang-format on
      }
   }

   TraceIf(TraceLevel::Info) {
      std::string whatWeArePlacing =
         item.ctrl == nullptr
            ? "Place Spacer"
            : item.lix != hs::LIX::Null
            ? "Place Label"
            : "Place Control";

      // clang-format off
      dout     << "\x1b[0;1;3m" << std::setw (20) << whatWeArePlacing
               << "\x1b[0;1;2m" << std::setw (25) << item.section
               << "\x1b[0;1m"   << std::setw (25) << (item.ctrl ? item.name : "<Spacer>")
               << "\x1b[0;32m"  << std::setw (8)  << (item.ctrl ? std::to_string(item.ctrl->GetId()) : "N/A")
               << "\x1b[0;33m"  << std::setw (4)  << item.row
               << "\x1b[0;33m"  << std::setw (4)  << item.col
               << "\x1b[0;34m"  << std::setw (4)  << item.proportion
               << "\x1b[0;34m"  << std::setw (4)  << item.border
               << "\x1b[0;35m"  << std::setw (4)  << (item.hide ? "Yes" : "No")
               << "\x1b[0;36m"  << " " << decodeSizerFlags (item.flags) << "\x1b[0m" << std::dec << std::endl;
      // clang-format on
   }

   bool bOK = true;

   if (item.ctrl == nullptr) {
      bOK = m_grid->AddStretchSpacer(item.proportion < 0 ? 0 : item.proportion) ? true : false;
   } else {
      if (auto sbsizer = static_cast<wxStaticBoxSizer *>(item.ctrl->GetClientData()); sbsizer != nullptr) {
         auto sizerItem = m_grid->Add(sbsizer, item.proportion < 0 ? 0 : item.proportion, item.flags, item.border);
         if (sizerItem && item.hide) {
            sizerItem->Show(false);
         }
         bOK = sizerItem != nullptr;
      } else {
         // Ensure the window we add to this grid has the correct parent (the grid's containing window)
         if (auto *container = m_grid->GetContainingWindow()) {
            auto *win = item.ctrl;
            if (win && win->GetParent() != container) {
               throw std::runtime_error("Window " + std::string(win->GetName().ToUTF8()) + " is not a child of the grid's containing window");
               // win->Reparent(container);
            }
         }
         auto sizerItem = m_grid->Add(item.ctrl, item.proportion < 0 ? 0 : item.proportion, item.flags,
                                      item.border);
         if (sizerItem && item.hide) {
            sizerItem->Show(false);
         }
         bOK = sizerItem != nullptr;
      }
   }

   return bOK;
}

// Map symbolic names to wxWidgets flag constants
auto Interface::parseSizerFlags(const std::vector<std::string> &flagStrings) -> int {
   static const std::unordered_map<std::string, long> flagMap = {
      // clang-format off
      {"expanding_control",               stdCtrlFlags},
      {"fixed_control",                   (stdCtrlFlags & ~wxGROW)},
      {"right_label",                     stdLabelFlags},
      {"left_label",                      (stdLabelFlags & ~wxALIGN_RIGHT)},
      {"wxALIGN_BOTTOM",                  wxALIGN_BOTTOM},
      {"wxALIGN_CENTER",                  wxALIGN_CENTRE},
      {"wxALIGN_CENTER_HORIZONTAL",       wxALIGN_CENTRE_HORIZONTAL},
      {"wxALIGN_CENTER_VERTICAL",         wxALIGN_CENTRE_VERTICAL},
      {"wxALIGN_CENTRE",                  wxALIGN_CENTRE},
      {"wxALIGN_CENTRE_HORIZONTAL",       wxALIGN_CENTRE_HORIZONTAL},
      {"wxALIGN_CENTRE_VERTICAL",         wxALIGN_CENTRE_VERTICAL},
      {"wxALIGN_LEFT",                    wxALIGN_LEFT},
      {"wxALIGN_RIGHT",                   wxALIGN_RIGHT},
      {"wxALIGN_TOP",                     wxALIGN_TOP},
      {"wxALL",                           wxALL},
      {"wxBOTTOM",                        wxBOTTOM},
      {"wxDOWN",                          wxDOWN},
      {"wxEXPAND",                        wxEXPAND},
      {"wxFIXED_MINSIZE",                 wxFIXED_MINSIZE},
      {"wxGROW",                          wxGROW},
      {"wxLEFT",                          wxLEFT},
      {"wxRESERVE_SPACE_EVEN_IF_HIDDEN",  wxRESERVE_SPACE_EVEN_IF_HIDDEN},
      {"wxRIGHT",                         wxRIGHT},
      {"wxSHAPED",                        wxSHAPED},
      {"wxSTRETCH_NOT",                   wxSTRETCH_NOT},
      {"wxTOP",                           wxTOP},
      {"wxUP",                            wxUP},
      // clang-format on
   };

   int result = 0;
   for (const auto &flag : flagStrings) {
      auto it = flagMap.find(flag);
      if (it != flagMap.end()) {
         result |= it->second;
      } else {
         throw std::runtime_error("Unknown flag: "s + flag);
      }
   }
   return result;
}

// // Map symbolic names to wxWidgets flag constants
// auto Interface::parseObjectSize(const std::string &sizeFlag) -> wxSize {
//    static const std::unordered_map<std::string, wxSize> sizeMap = {
//       // clang-format off
//       {"sizeCtrlSpin",           sizeCtrlSpin},
//       {"sizeCtrlSmall",          sizeCtrlSmall},
//       {"sizeCtrlIntComboLike",   sizeCtrlIntComboLike},
//       {"sizeCtrlButton",         sizeCtrlButton},
//       {"sizeCtrlComboLike",      sizeCtrlComboLike},
//       {"sizeCtrlMedium",         sizeCtrlMedium},
//       {"sizeCtrl",               sizeCtrl},
//       {"sizeCtrlMediumLarge",    sizeCtrlMediumLarge},
//       {"sizeCtrlLarge",          sizeCtrlLarge},
//       {"sizeCtrlELB",            sizeCtrlELB},
//       {"sizeLabelSmall",         sizeLabelSmall},
//       {"sizeLabelMedium",        sizeLabelMedium},
//       {"sizeLabel",              sizeLabel},
//       {"sizeLabelLarge",         sizeLabelLarge},
//       {"sizeNotes",              sizeNotes},
//       {"sizeCtrlCheckBox",       sizeCtrlCheckBox}
//       // clang-format on
//    };
//
//    wxSize result = wxSize(wxDefaultCoord, wxDefaultCoord);
//    for (const auto &flag : sizeMap) {
//       if (flag.first == sizeFlag) {
//          result = flag.second;
//          break;
//       }
//    }
//
//    if (result == wxSize(wxDefaultCoord, wxDefaultCoord)) {
//       throw std::runtime_error("Unknown size flag: " + sizeFlag);
//    }
//
//    return result;
// }

auto Interface::decodeSizerFlags(long flags) -> std::string {

   static const std::unordered_map<long, std::string> flagMap = {
      // clang-format off
      {wxALIGN_BOTTOM,                 "wxALIGN_BOTTOM"},
      {wxALIGN_CENTER,                 "wxALIGN_CENTRE"},
      {wxALIGN_CENTER_HORIZONTAL,      "wxALIGN_CENTRE_HORIZONTAL"},
      {wxALIGN_CENTER_VERTICAL,        "wxALIGN_CENTRE_VERTICAL"},
      {wxALIGN_CENTRE,                 "wxALIGN_CENTRE"},
      {wxALIGN_CENTRE_HORIZONTAL,      "wxALIGN_CENTRE_HORIZONTAL"},
      {wxALIGN_CENTRE_VERTICAL,        "wxALIGN_CENTRE_VERTICAL"},
      // {wxALIGN_LEFT,                   "wxALIGN_LEFT"},
      {wxALIGN_RIGHT,                  "wxALIGN_RIGHT"},
      // {wxALIGN_TOP,                    "wxALIGN_TOP"},
      {wxALL,                          "wxALL"},
      {wxUP,                           "wxUP"},
      {wxDOWN,                         "wxDOWN"},
      {wxBOTTOM,                       "wxBOTTOM"},
      {wxEXPAND,                       "wxEXPAND"},
      {wxFIXED_MINSIZE,                "wxFIXED_MINSIZE"},
      {wxGROW,                         "wxGROW"},
      {wxLEFT,                         "wxLEFT"},
      {wxRESERVE_SPACE_EVEN_IF_HIDDEN, "wxRESERVE_SPACE_EVEN_IF_HIDDEN"},
      {wxRIGHT,                        "wxRIGHT"},
      {wxSHAPED,                       "wxSHAPED"},
      // {wxSTRETCH_NOT,                  "wxSTRETCH_NOT"},
      {wxTOP,                          "wxTOP"},
      // {wxALIGN_NOT,                    "wxALIGN_NOT"}
      // clang-format on
   };

   std::string result{};

   auto appendFlagName = [&result](const std::string &name) {
      if (!result.empty()) {
         result += "|";
      }
      result += name;
   };

   if (isBitSet(flags, wxALL)) {
      appendFlagName("wxALL");
      flags &= ~wxALL;
   }

   bool needLeft = true;
   bool needBottom = true;

   if (isBitSet(flags, wxALIGN_CENTER)) {
      appendFlagName("wxALIGN_CENTER");
      flags &= ~wxALIGN_CENTER;
      needLeft = false;
      needBottom = false;
   }

   for (const auto &[flag, name] : flagMap) {
      if (isBitSet(flags, flag))
         appendFlagName(name);
      if (flag == wxALIGN_RIGHT || flag == wxALIGN_CENTER_HORIZONTAL)
         needLeft = false;
      if (flag == wxALIGN_TOP || flag == wxALIGN_CENTER_VERTICAL)
         needBottom = false;
   }

   if (needLeft)
      appendFlagName("(wxALIGN_LEFT)");
   if (needBottom)
      appendFlagName("(wxALIGN_BOTTOM");

   return result;
}

auto Interface::loadLayout(const fs::path &path, const std::string &node) -> bool {
   TraceCall;

   YAML::Node rooty_hill;
   try {
      rooty_hill = YAML::LoadFile(path.string());
   } catch (const YAML::Exception &err) {
      dout << "Failed to load/parse YAML: " << err.what() << std::endl;
      return false;
   }

   std::string cSection;

   auto root = !node.empty() ? rooty_hill["groups"][node] : rooty_hill["groups"];
   bool wizardFound = {}, pageFound = {}, groupFound = !Dump(root).empty();
   if (groupFound) {
      cSection = root["title"].as<std::string>("(anonymous group)");
   } else {
      root = !node.empty() ? rooty_hill["pages"][node] : rooty_hill["pages"];
      pageFound = !Dump(root).empty();
      if (pageFound) {
         cSection = root["title"].as<std::string>("(anonymous group)");
      } else {
         root = !node.empty() ? rooty_hill["wizardpages"][node] : rooty_hill["wizardpages"];
         wizardFound = !Dump(root).empty();
         if (wizardFound) {
            cSection = root["title"].as<std::string>("(anonymous group)");
         } else {
            dout << "No 'groups', 'pages', or 'wizardpages' node in " << path << std::endl;
            dout << Dump(rooty_hill) << std::endl;
            return false;
         }
      }
   }

   auto processSection = [&, this](YAML::Node thisRoot, const std::string &section,
                                   const std::filesystem::path &yaml_file) -> bool {
      using hs::int_v, hs::string_v;

      // New schema: find the 'elements' node for the selected page/group/wizardpage
      YAML::Node elementsNode;
      for (const auto &kv : thisRoot) {
         if (kv.first.as<std::string>() == "elements") {
            elementsNode = kv.second;
            break;
         }
      }
      if (!elementsNode || !elementsNode.IsSequence()) {
         dout << "Warning: Missing or invalid 'elements' (must be a sequence) in section " << section << "file://" <<
            yaml_file
            << std::endl;
         dout << Dump(thisRoot) << std::endl;
         return true;
      }

      if (!thisRoot["sizer"] || !thisRoot["sizer"].IsMap()) {
         dout << "Missing or invalid 'sizer' (must be a map) in section " << section << "file://" << yaml_file <<
            std::endl;
         dout << Dump(thisRoot) << std::endl;
         return false;
      }

      auto sizer = thisRoot["sizer"];

      // clang-format off
      auto rows    = sizer["rows"].as<int>(1);
      auto cols    = sizer["cols"].as<int>(1);
      auto grows   = sizer["growable_rows"];
      auto gcols   = sizer["growable_cols"];

      int_v growableRows;
      if (grows.IsSequence()) {
         growableRows = grows.as<int_v>();
      }
      int_v growableCols;
      if (gcols.IsSequence()) {
         growableCols = gcols.as<int_v>();
      }

      if (rows <= 0)                   { dout << "Warning: ''rows' must be greater than zero for section " << section << "file://" << yaml_file << std::endl; rows = 1;  }
      if (growableRows.size() > rows)  { dout << "Warning: 'growable_rows' must be 'rows' or less in length for section " << section << "file://" << yaml_file << std::endl; growableRows.clear();   }
      if (cols <= 0)                   { dout << "Warning: ''cols' must be greater than zero for section " << section << "file://" << yaml_file << std::endl; cols = 1;  }
      if (growableCols.size() > cols)  { dout << "Warning: 'growable_cols' must be 'cols' or less in length for section " << section << "file://" << yaml_file << std::endl; growableCols.clear();   }

      initialize(cols, rows);

      if (!growableRows.empty())     std::ranges::for_each(growableRows,   [&, this](auto &&gr){m_grid->AddGrowableRow(gr);});
      if (!growableCols.empty())     std::ranges::for_each(growableCols,   [&, this](auto &&gc){m_grid->AddGrowableCol(gc);});

      // clang-format on

      auto findCtrlByName = [&](const std::string &name, hs::LIX lix = hs::LIX::Null) -> std::pair<wxControl *, UICreateFlags> {
         for (const auto &entry : unifiedMap()) {
            ICtrl *c = entry.second;
            if (!c)
               continue;

            if (c->creationName() == name) {
               return {c->ctl(), c->creationFlags()};
            }

            if (auto lbl = c->label(name); lbl.first != nullptr) {
               if (lbl.first->GetName() == name) {
                  return lbl;
               }
            }

            if (lix != hs::LIX::Null) {
               if (auto lbl2 = c->label(lix); lbl2.first != nullptr) {
                  if (lbl2.first->GetName() == name) {
                     return lbl2;
                  }
               }
            }
         }
         if (lix != hs::LIX::Null) {
            dout << "Warning: Could not find control name: " << name << " for LIX " << lix << std::endl;
         } else {
            dout << "Warning: Could not find control name: " << name << std::endl;
         }
         return {};
      };

      // Iterate YAML controls in YAML order. Place items for each control in that order.

      // Iterate elements list; each element has items[]
      for (const auto &element : elementsNode) {
         if (!element.IsMap())
            continue;

         // Labels and control/group placements come from items[]
         const YAML::Node itemsNode = element["items"];
         if (!itemsNode || !itemsNode.IsSequence()) {
            continue;
         }

         // Place labels and controls/groups in order as per index
         for (const auto &yamlItem : itemsNode) {
            if (!yamlItem.IsMap())
               continue;

            // Sizer subobject
            auto parseSizer = [this](ParsedItem &item, const YAML::Node &sizer, int defaultBorderWidth) {
               string_v defaultFlags{"wxALIGN_CENTRE_VERTICAL", "wxALL"};

               auto pos = sizer["position"].as<std::vector<int>>();
               item.row = pos[0];
               item.col = pos[1];

               if (item.col == -1 || item.row == -1) {
                  return false;
               }
               // }

               item.flags = sizer && sizer["flags"]
                               ? parseSizerFlags(sizer["flags"].as<std::vector<std::string>>(defaultFlags))
                               : wxALIGN_CENTRE_VERTICAL | wxALL;
               item.border = sizer && sizer["borderWidth"]
                                ? sizer["borderWidth"].as<int>(defaultBorderWidth)
                                : sizer && sizer["border"]
                                ? sizer["border"].as<int>(defaultBorderWidth)
                                : defaultBorderWidth;
               item.hide = sizer && sizer["hide"] ? sizer["hide"].as<bool>(false) : false;
               item.proportion = sizer && sizer["proportion"] ? sizer["proportion"].as<int>(1) : 1;

               return true;
            };

            // Labels block
            // dout << Dump(yamlItem) << "\n\n" << std::endl;
            // dout << Dump(yamlItem["labels"]) << "\n\n" << std::endl;
            if (const auto &labelsNode = yamlItem["labels"]; labelsNode && labelsNode.IsSequence()) {
               for (const auto &entry : labelsNode) {
                  if (!entry.IsMap())
                     continue;

                  ParsedItem parsedItem{};
                  // Key -> LIX
                  auto getLix = [](const std::string &k) {
                     if (k == "LIX::Main" || k == "[LIX::Main]")
                        return hs::LIX::Main;
                     if (k == "LIX::Second" || k == "[LIX::Second]")
                        return hs::LIX::Second;
                     return hs::wxStringConverter<hs::LIX>::fromString(k);
                  };
                  const std::string labelKey = entry["key"] ? entry["key"].as<std::string>("LIX::Main") : "LIX::Main";
                  const auto tag = entry["tag"].as<std::string>();
                  parsedItem.lix = getLix(labelKey);
                  VERIFY_MSG(!tag.empty(),
                             std::format("Label missing 'tag' file://{}", yaml_file.
                                string()));
                  parsedItem.name = tag;
                  parsedItem.section = section;

                  auto thing = findCtrlByName(tag, parsedItem.lix);
                  parsedItem.ctrl = thing.first;
                  parsedItem.hide = isBitSet(thing.second, UICreateFlags::Hide);
                  if (isBitClear(thing.second, UICreateFlags::NoPlacement)) {
                     if (parsedItem.ctrl) {
                        const YAML::Node sz = entry["sizer"];
                        if (parseSizer(parsedItem, sz, borderWidth)) {
                           m_gridData[parsedItem.row][parsedItem.col] = parsedItem;
                        }
                     }
                  }
               }
            }

            // Control or group block
            const auto &ctrlNode = yamlItem["widget"];
            if (!(ctrlNode)) {
               // dout << Dump(yamlItem) << std::endl;
               continue;
            }
            // For placement data (sizer) and the creation-visible name
            const YAML::Node elemSizerNode = ctrlNode["sizer"];

            const auto tag = ctrlNode["tag"].as<std::string>();
            VERIFY_MSG(!tag.empty(), std::format("Widget {} missing 'tag' file://{}", section, yaml_file.string()));

            ParsedItem parsedItem{};
            parsedItem.name = tag; // control/group constructor-visible name
            parsedItem.section = section;

            auto thing = findCtrlByName(tag);
            parsedItem.ctrl = thing.first;
            if (parsedItem.ctrl) {
               if (isBitClear(thing.second, UICreateFlags::NoPlacement)) {
                  if (parseSizer(parsedItem, elemSizerNode, borderWidth)) {
                     m_gridData[parsedItem.row][parsedItem.col] = parsedItem;
                     parsedItem.hide = isBitSet(thing.second, UICreateFlags::Hide);
                  }
               }
            }
         }
      }

      return true;
   };

   if (processSection(root, cSection, path)) {
      std::ranges::for_each(m_gridData, [&](auto &&row) {
         std::ranges::for_each(row, [&](auto &&col) {
            doAddItemToSizer(col.ctrl, col);
         });
      });
      m_grid->Layout();
      return true;
   }
   return false;
}
}
