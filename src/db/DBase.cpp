module;

// clang-format off
#include "Gfx/wx.h"
#include "Core/Core.h"

#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>

// clang-format on

#include "Core/CoreData.h"
#include "Gfx/Widgets.h"

#include <iostream>
#include <string>
#include <vector>

module MC.DBase;
import Database;

import Ctrl;
import DDT;
import Streamable;
import StringUtil;
import Types;

import Addresses.RS;
import EmailAuthority.RS;
import EmailPorts.RS;
import EmailSecurity.RS;
import PhoneNumbers.RS;
import PhoneTypes.RS;
import Settings.RS;
import States.RS;
import Titles.RS;
import Users.RS;
import Years.RS;
import TestStatus.RS;

import Admissions.RS;
import Appointments.RS;
import Doctors.RS;
import Illnesses.RS;
import Items.RS;
import Locations.RS;
import Scripts.RS;
import Statistics.RS;
import Vaccinations.RS;

namespace mc {
using namespace db;

auto DBase::newDB(const std::filesystem::path &name) -> bool {
   if (Database::newDB(name)) {
      wx::ICtrl::db(this);
      return true;
   }
   return false;
}
auto DBase::openDB(const std::filesystem::path &name, int magic, int minVersion) -> bool {
   if (Database::openDB(name, magic, minVersion)) {
      wx::ICtrl::db(this);
      return true;
   }
   return false;
}
auto DBase::closeDB() -> void {
   Database::closeDB();
   wx::ICtrl::db(nullptr);
}

auto DBase::initializeDefaultData() -> bool {

   try {
      auto statesTbl = getTable("states");
      StatesRS statesRS(*statesTbl);

      std::ranges::for_each(hs::australianStates(), [&statesRS](const auto &s) {
         statesRS.insertRecord(s.searchable, s.abbreviation, s.name);
      });

      std::ranges::for_each(hs::northAmericanStates(), [&statesRS](const auto &e) {
         statesRS.insertRecord(e.searchable, e.abbreviation, e.name);
      });

      auto titleTbl = getTable("titles");
      TitlesRS titleRS(*titleTbl);
      std::ranges::for_each(hs::titles(), [&titleRS](const auto &title) {
         titleRS.insertRecord(title.searchable, title.abbreviation, title.name);
      });

      auto phoneTypeTbl = getTable("phone_types");
      PhoneTypesRS phoneRS(*phoneTypeTbl);
      std::ranges::for_each(std::array{"", "Home", "Business", "Mobile", "Fax"},
                            [&phoneRS](std::string s) { phoneRS.insertRecord(s); });

      auto emailSecurityTbl = getTable("email_ports");
      EmailSecurityRS securityRS(*emailSecurityTbl);
      std::ranges::for_each(std::array<std::pair<int, std::string>, 3>{std::pair{0, "None"},
                                                                       std::pair{1, "STARTTLS"},
                                                                       std::pair{2, "SSL/TLS"}},
                            [&securityRS](const std::pair<int, std::string> &pr) {
                               securityRS.insertRecord(pr.first, pr.second);
                            });

      auto emailAuthorityTbl = getTable("email_ports");
      EmailAuthorityRS authorityRS(*emailAuthorityTbl);
      std::ranges::for_each(std::array<std::pair<int, std::string>, 7>{std::pair{0, "None"},
                                                                       std::pair{1, "Normal Password"},
                                                                       std::pair{2, "Encrypted Password"},
                                                                       std::pair{3, "Kerberos/GSSAPI"},
                                                                       std::pair{4, "NTLM"},
                                                                       std::pair{5, "TLS Certificate"},
                                                                       std::pair{6, "OAuth2"}},
                            [&authorityRS](const std::pair<int, std::string> &pr) {
                               authorityRS.insertRecord(pr.first, pr.second);
                            });

      auto emailPortsTbl = getTable("email_ports");
      EmailPortsRS portsRS(*emailPortsTbl);
      std::ranges::for_each(std::array<std::pair<int, std::string>, 4>{std::pair{ 587, "Encrypted"},
                                                                       std::pair{  25, "Unencrypted"},
                                                                       std::pair{ 465, "Deprecated"},
                                                                       std::pair{2525, "Alternative to 25"}},
                            [&portsRS](const std::pair<int, std::string> &pr) {
                               portsRS.insertRecord(pr.first, pr.second);
                            });

      auto testStatusTbl = getTable("test_status");
      TestStatusRS testStatusRS(*testStatusTbl);
      std::ranges::for_each(std::array{"Untested", "Failed", "Passed"},
                            [&testStatusRS](std::string s) { testStatusRS.insertRecord(s); });

      auto admissionsTbl = getTable("admissions");
      AdmissionsRS admissionsRS(*admissionsTbl);
      admissionsRS.insertRecord(hs_bool(true));

      auto appointmentsTbl = getTable("appointments");
      AppointmentsRS appointmentsRS(*appointmentsTbl);
      appointmentsRS.insertRecord(hs_bool(true));

      auto doctorsTbl = getTable("doctors");
      DoctorsRS doctorsRS(*doctorsTbl);
      doctorsRS.insertRecord("Claudius", "Bombay", hs_bool{false}, 1);

      auto illnessesTbl = getTable("illnesses");
      IllnessesRS illnessesRS(*illnessesTbl);
      illnessesRS.insertRecord(hs_bool(true));

      auto itemsTbl = getTable("items");
      ItemsRS itemsRS(*itemsTbl);
      itemsRS.insertRecord(hs_bool(true));

      auto locationsTbl = getTable("locations");
      LocationsRS locationsRS(*locationsTbl);
      locationsRS.insertRecord(hs_bool(true));

      auto scriptsTbl = getTable("scripts");
      ScriptsRS scriptsRS(*scriptsTbl);
      scriptsRS.insertRecord(hs_bool(true));

      auto statisticsTbl = getTable("statistics");
      StatisticsRS statisticsRS(*statisticsTbl);
      statisticsRS.insertRecord(hs_bool(true));

      auto vaccinationsTbl = getTable("vaccinations");
      VaccinationsRS vaccinationsRS(*vaccinationsTbl);
      vaccinationsRS.insertRecord(hs_bool(true));

      auto yearTbl = getTable("years");
      YearsRS yearRS(*yearTbl);
      yearRS.insertRecord(wxDateTime::Now().GetYear());

      auto settingsTbl = getTable("settings");
      SettingsRS settingsRS(*settingsTbl);
      settingsRS.insertRecord(hs::dataMagic(), hs::minDataVersion(), 1, 1);

      return true;

   } catch (std::exception &e) {
      TraceIf(TraceLevel::Error)
         doutif(TraceLevel::Error) << "Creating tables: " << e.what() << std::endl;
      return false;
   }
}

auto DBase::validateDB(const std::filesystem::path &name, int magic, int minVersion) -> bool {
   return isValid(name, magic, minVersion);
}

auto DBase::isValid(const std::filesystem::path &name, int magic, int minVersion) -> bool {
   int count = 0;
   try {
      auto testSession = std::make_shared<soci::session>(*soci::factory_sqlite3(), name.string());

      // Check if 'settings' table exists
      int tableExists = 0;
      *testSession << "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='settings'",
         soci::into(tableExists);
      if (tableExists == 0) {
         return false;
      }
      // Check if 'fMagic' field exists
      int columnExists = 0;
      *testSession << "SELECT COUNT(*) FROM pragma_table_info('settings') WHERE name='fMagic'",
         soci::into(columnExists);
      if (columnExists == 0) {
         return false;
      }

      // Check if 'fMagic' has the expected value
      *testSession << "SELECT COUNT(*) FROM settings WHERE fMagic = :magic", soci::use(magic), soci::into(count);
      if (count == 0) {
         return false;
      }

      // Check if 'fDataVersion' has the expected value
      *testSession << "SELECT COUNT(*) FROM settings WHERE fDataVersion >= :minVersion", soci::use(minVersion),
         soci::into(count);

      if (count == 0) {
         return false;
      }

      return true;

   } catch (const std::exception &e) {
      TraceIf(TraceLevel::Error)
         dout << "Database validation error: " << e.what() << std::endl;
      return false;
   }
}
} // namespace mc
