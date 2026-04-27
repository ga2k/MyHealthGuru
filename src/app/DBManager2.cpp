module;
// clang-format off

// #include "Gfx/GlobalIDs.h"
#include "Gfx/Widgets.h"
#include "Core/CoreData.h"
#include "Core/TraceLevel.h"
#include "Core/Util.h"
// #include <filesystem>
// #include <wx/docview.h>
#include <wx/string.h>
// clang-format on

module MC.DBManager;
import DBManager;

// import MC.DBase;
// import MC.Document;
// import MC.View;

import NewFile.Wizard;
import Users.RS;
import Years.RS;
import Settings.RS;
import PhoneNumbers.RS;
import PhoneTypes.RS;

import DDT;
// import HtmlDialog;
// import Streamable;
import Types;
// import wxTypes;
// import String;
import Util;
import SplashSignals;

namespace mc {

auto DBManager::firstRun(bool forceRun) -> DBHandlerResult {
   doutif (TraceLevel::Fatal) << "In DBManager::firstRun()\n";

   DBHandlerResult result{};

   bool initialSetupComplete = false;
   bool firstRun{true};
   bool systemRunning{hs::isRunning()};

   std::string lastDatafile{""};
   ID::Type lastUserID{ID::None};

   if (forceRun) {
      firstRun = true;
      set(true, "General", "FirstRun");
      removeAllFiles();
   } else if (systemRunning) {
      firstRun = get<bool>("General", "FirstRun").value_or(true);
   } else {
      firstRun = get<bool>("General", "FirstRun").value_or(true);
      lastDatafile = getFile().string();
   }

   if (firstRun || lastDatafile.empty()) {
      anymap args;
      // args.insert_or_assign("bInitialRun", firstRun);
      auto app = hs::appName();
      auto pmt = std::format("Welcome to {}", app);
      auto frm = wx::pFrame();

      wx::NewFileWizard wiz(frm, pmt, args);

      hs::splashSignal().Hide();

      initialSetupComplete = wiz.RunWizard(wiz.GetFirstPage());

      if (!initialSetupComplete) {
         result = {CreateBits::Failed, ""};
         return result;
      }
      if (wiz.enoughStepsCompleted()) {

         hs::splashSignal().Show();

         try {
            auto data = wiz.data();
            auto pathStr = anyV<std::string>(data["Basic Details"]["Datafile"]["Filename"], "");
            if (pathStr.empty()) {
               return {firstRun ? CreateBits::New : CreateBits::Open, lastDatafile};
            }
            auto path = std::filesystem::path(pathStr);
#ifdef WIN32
            auto trash_path = Util::getInstance().getHomeDir();
            std::error_code trash_code;
            fs::create_directory(path.parent_path(), trash_path, trash_code);
#else
            fs::create_directory(path.parent_path());
#endif

            DBase new_db;
            auto ok = new_db.newDB(path) && new_db.isOk();

            if (ok) {

               auto userTable = new_db.getTable("users");
               UsersRS userRS(*userTable);

               auto gn = anyV<std::string>(data["Basic Details"]["Name Details"]["Given Name"], "");
               auto fn = anyV<std::string>(data["Basic Details"]["Name Details"]["Family Name"], "");
               auto ff = static_cast<hs_bool>(anyV<bool>(data["Basic Details"]["Name Details"]["Family Name First"],
                                                             false));
               auto tl = anyV<ID::Type>(data["Basic Details"]["Name Details"]["Title"], ID::None);

               int userID = userRS.insertRecord(gn, fn, ff, static_cast<int>(tl), hs_bool(true));

               firstRun = false;
               set(firstRun, "General", "FirstRun");
               lastDatafile = path.string();
               addFile(lastDatafile);
               return {firstRun ? CreateBits::New : CreateBits::Open, lastDatafile};
            }
         } catch (std::exception ex) {
            TraceIf(TraceLevel::Error)
               dout << ex.what() << std::endl;
            return {firstRun ? CreateBits::New : CreateBits::Open, lastDatafile};
         }
      } else {

      }
   }
   return {firstRun ? CreateBits::New : CreateBits::Open, lastDatafile};
}

auto DocTemplate::validateFile(const fs::path &path, int magic, int minVersion) -> bool {
   TraceCall;
   return mc::DBase::isValid(path, magic, minVersion);
}

} // namespace mc
