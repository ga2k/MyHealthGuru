module;

// clang-format off
#include "Core/Core.h"
#include "Gfx/wx.h"

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


auto DBase::initializeSchema() -> std::shared_ptr<TableLoader> {

   try {
      TableRegistry::instance()
         .clear()

         .registerTable("states")
         .registerTable("addresses")
         .registerTable("phone_types")
         .registerTable("phone_numbers")

         .registerTable("email_authority")
         .registerTable("email_ports")
         .registerTable("email_security")
         .registerTable("test_status")
         .registerTable("settings")

         .registerTable("admissions")
         .registerTable("appointments")
         .registerTable("doctors")
         .registerTable("illnesses")
         .registerTable("items")
         .registerTable("locations")
         .registerTable("scripts")
         .registerTable("statistics")
         .registerTable("titles")
         .registerTable("users")
         .registerTable("vaccinations")

         .registerTable("years");

      // Tables referenced by other tables. ComboBox contents first.

      (void) createTablesFromSchema ("states_table.yaml");
      (void) createTablesFromSchema ("phone_types_table.yaml");
      (void) createTablesFromSchema ("email_authority_table.yaml");
      (void) createTablesFromSchema ("email_ports_table.yaml");
      (void) createTablesFromSchema ("email_security_table.yaml");
      (void) createTablesFromSchema ("test_status_table.yaml");
      (void) createTablesFromSchema ("titles_table.yaml");

      // Now whole tables referenced by other tables

      (void) createTablesFromSchema ("Locations.yaml");
      (void) createTablesFromSchema ("addresses_table.yaml");
      (void) createTablesFromSchema ("Doctors.yaml");
      (void) createTablesFromSchema ("Users.yaml");
      (void) createTablesFromSchema ("years_table.yaml");
      (void) createTablesFromSchema ("settings_table.yaml");

      (void) createTablesFromSchema ("phone_numbers_table.yaml");

      (void) createTablesFromSchema ("Admissions.yaml");
      (void) createTablesFromSchema ("Appointments.yaml");
      (void) createTablesFromSchema ("Illnesses.yaml");
      (void) createTablesFromSchema ("Items.yaml");
      (void) createTablesFromSchema ("Scripts.yaml");
      (void) createTablesFromSchema ("Statistics.yaml");
      return createTablesFromSchema ("Vaccinations.yaml");

      // clang-format on

   } catch (std::exception &e) {
      TraceIf(TraceLevel::Error)
         doutif(TraceLevel::Error) << "Creating tables: " << e.what() << std::endl;
      return nullptr;
   }
}
} // namespace mc
