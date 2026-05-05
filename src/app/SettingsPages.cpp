module;
// Non-module includes in global module fragment
#include "Core/CoreData.h"
#include "Gfx/wx.h"
#include "Core/Core.h"
#include "Gfx/Widgets.h"
#include "MyHealthGuru/MyHealthGuru.h"

#include <wx/aui/auibook.h>
module MC.View;

import Book;
import DDT;
import wxTypes;
import wxUtil;
import SubPage;

// Import remaining generated UI page modules for Settings and its subpages, and Vaccinations
import Settings.Page;
import AppearanceSettings.Page;
import GeneralSettings.Page;
import ListsSettings.Page;
import ScriptsSettings.Page;
import YearsSettings.Page;
import Vaccinations.Page;

namespace mc {
void populateSettingsPages(wx::Book* book, wx::SubPage*& settings) {
   hs::size m_imageXY{32, 32};

   wxNotebook::Images settingsImages;
   settingsImages.push_back (wx::getIcon (UIType::Menu, "preferences-system",    true, &m_imageXY));
   settingsImages.push_back (wx::getIcon (UIType::Menu, "mail-message-new-list", true, &m_imageXY));
   settingsImages.push_back (wx::getIcon (UIType::Menu, "format-list-ordered",   true, &m_imageXY));
   settingsImages.push_back (wx::getIcon (UIType::Menu, "fill-color",            true, &m_imageXY));
   settingsImages.push_back (wx::getIcon (UIType::Menu, "project-development",   true, &m_imageXY));

   anymap settingsArgs = {
      {"imageList", &settingsImages},
      {"imageIndex", 0},
      {"bookFlags", static_cast<long>(wxAUI_NB_BOTTOM | wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS |
                                       wxAUI_NB_CLOSE_ON_ACTIVE_TAB)}
   };

   // Create the Settings SubPage and its subpages
   // book->addPage(settings = new wx::SettingsPage ( book, wx::nextID(), "Settings", PageType::SubPage, 0, settingsArgs), false, 0);  settings->book()->load();
   // settings->addPage (new wx::GeneralSettingsPage    ( settings->book(), wx::nextID(), "General",      PageType::GeneralSettings,     0 ), false,  0);
   // settings->addPage (new wx::ScriptsSettingsPage    ( settings->book(), wx::nextID(), "Scripts",      PageType::ScriptSettings,      1 ), false,  1);
   // settings->addPage (new wx::ListsSettingsPage      ( settings->book(), wx::nextID(), "Lists",        PageType::ListSettings,        2 ), false,  2);
   // settings->addPage (new wx::AppearanceSettingsPage ( settings->book(), wx::nextID(), "Appearance",   PageType::AppearanceSettings,  3 ), false,  3);
   // settings->addPage (new wx::YearsSettingsPage      ( settings->book(), wx::nextID(), "Years",        PageType::YearsSettings,       4 ), false,  4);
   settings = new wx::SettingsPage ( book, wx::nextID(), "Settings", PageType::SubPage, 0, settingsArgs);
   settings->book()->load();
   new wx::GeneralSettingsPage    ( settings->book(), wx::nextID(), "General",      PageType::GeneralSettingsPage,     0 );
   new wx::ScriptsSettingsPage    ( settings->book(), wx::nextID(), "Scripts",      PageType::ScriptsSettingsPage,     1 );
   new wx::ListsSettingsPage      ( settings->book(), wx::nextID(), "Lists",        PageType::ListsSettingsPage,       2 );
   new wx::AppearanceSettingsPage ( settings->book(), wx::nextID(), "Appearance",   PageType::AppearanceSettingsPage,  3 );
   new wx::YearsSettingsPage      ( settings->book(), wx::nextID(), "Years",        PageType::YearsSettingsPage,       4 );

   // Set default selections based on config
   wxTheApp->Yield();

   auto viewPageSelection = book->get<std::string>("Settings", "DefaultPage").value_or("None");
   if (viewPageSelection == "None")
      viewPageSelection = "Appointments"s;

   auto settingsPageSelection = settings->book()->get<std::string>("Settings", "DefaultPage").value_or("None");
   if (settingsPageSelection == "None")
      settingsPageSelection = "General";

   int viewPageIndex = book->FindPage(book->FindWindow(viewPageSelection));
   int settingsPageIndex = settings->book()->FindPage(settings->book()->FindWindow(settingsPageSelection));

   book->SetSelection(viewPageIndex);
   if (viewPageSelection == "Settings") {
      settings->book()->SetSelection(settingsPageIndex);
   }
}
} // namespace mc
