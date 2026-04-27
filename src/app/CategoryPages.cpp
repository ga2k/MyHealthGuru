module;
// Non-module includes in global module fragment
#include "Core/CoreData.h"
#include "Gfx/wx.h"
#include "Core/Core.h"
#include "Gfx/Widgets.h"
#include "MyCare/MyCare.h"

#include <wx/aui/auibook.h>
module MC.View;

import Book;
import DDT;
import wxTypes;
import wxUtil;
import SubPage;

// Import a subset of generated UI page modules
import Admissions.Page;
import Appointments.Page;
import Doctors.Page;
import Illnesses.Page;
import Items.Page;
import Locations.Page;
import Scripts.Page;
import Statistics.Page;
import Users.Page;

namespace mc {
void populateCategoryPages(wx::Book* book, wx::SubPage*& settings) {
   hs::size m_imageXY{32, 32};

   wxNotebook::Images images;
   images.push_back (wx::getIcon (UIType::Page, "Settings.png",      false, &m_imageXY));
   images.push_back (wx::getIcon (UIType::Page, "Locations.png",     false, &m_imageXY));
   images.push_back (wx::getIcon (UIType::Page, "Doctors.png",       false, &m_imageXY));
   images.push_back (wx::getIcon (UIType::Page, "Users.png",         false, &m_imageXY));
   images.push_back (wx::getIcon (UIType::Page, "Illnesses.png",     false, &m_imageXY));
   images.push_back (wx::getIcon (UIType::Page, "Scripts.png",       false, &m_imageXY));
   images.push_back (wx::getIcon (UIType::Page, "Items.png",         false, &m_imageXY));
   images.push_back (wx::getIcon (UIType::Page, "Statistics.png",    false, &m_imageXY));
   images.push_back (wx::getIcon (UIType::Page, "Vaccinations.png",  false, &m_imageXY));
   images.push_back (wx::getIcon (UIType::Page, "Admissions.png",    false, &m_imageXY));
   images.push_back (wx::getIcon (UIType::Page, "Appointments.png",  false, &m_imageXY));
   book->SetImages (images);

   // Add major pages except Settings and Settings subpages
   // book->addPage     (new wx::AdmissionsPage         ( book,             wx::nextID(), "Admissions",   PageType::Admissions,    9 ), false,  9);
   // book->addPage     (new wx::AppointmentsPage       ( book,             wx::nextID(), "Appointments", PageType::Appointments, 10 ), false, 10);
   // book->addPage     (new wx::DoctorsPage            ( book,             wx::nextID(), "Doctors",      PageType::Doctors,       2 ), false,  2);
   // book->addPage     (new wx::IllnessesPage          ( book,             wx::nextID(), "Illnesses",    PageType::Illnesses,     4 ), false,  4);
   // book->addPage     (new wx::ItemsPage              ( book,             wx::nextID(), "Items",        PageType::Items,         6 ), false,  6);
   // book->addPage     (new wx::LocationsPage          ( book,             wx::nextID(), "Locations",    PageType::Locations,     1 ), false,  1);
   // book->addPage     (new wx::ScriptsPage            ( book,             wx::nextID(), "Scripts",      PageType::Scripts,       5 ), false,  5);
   // book->addPage     (new wx::StatisticsPage         ( book,             wx::nextID(), "Statistics",   PageType::Statistics,    7 ), false,  7);
   // book->addPage     (new wx::UsersPage              ( book,             wx::nextID(), "Users",        PageType::Users,         3 ), false,  3);
   new wx::AdmissionsPage         ( book,             wx::nextID(), "Admissions",   PageType::AdmissionsPage,    9 );
   new wx::AppointmentsPage       ( book,             wx::nextID(), "Appointments", PageType::AppointmentsPage, 10 );
   new wx::DoctorsPage            ( book,             wx::nextID(), "Doctors",      PageType::DoctorsPage,       2 );
   new wx::IllnessesPage          ( book,             wx::nextID(), "Illnesses",    PageType::IllnessesPage,     4 );
   new wx::ItemsPage              ( book,             wx::nextID(), "Items",        PageType::ItemsPage,         6 );
   new wx::LocationsPage          ( book,             wx::nextID(), "Locations",    PageType::LocationsPage,     1 );
   new wx::ScriptsPage            ( book,             wx::nextID(), "Scripts",      PageType::ScriptsPage,       5 );
   new wx::StatisticsPage         ( book,             wx::nextID(), "Statistics",   PageType::StatisticsPage,    7 );
   new wx::UsersPage              ( book,             wx::nextID(), "Users",        PageType::UsersPage,         3 );
}
} // namespace mc
