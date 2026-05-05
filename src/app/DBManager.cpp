module;
// clang-format off
// #include "Gfx/GlobalIDs.h"
#include "Gfx/Widgets.h"
#include "Core/CoreData.h"
#include "Core/TraceLevel.h"
// #include "Core/Util.h"
// #include <filesystem>
#include <vector>
#include <wx/docview.h>
#include <wx/string.h>
// clang-format on

module MC.DBManager;
import DBManager;

// import MC.DBase;
// import MC.Document;
// import MC.View;

// import NewFile.Wizard;
// import Users.RS;
// import Years.RS;
// import Settings.RS;
// import PhoneNumbers.RS;
// import PhoneTypes.RS;

import DDT;
// import HtmlDialog;
// import Streamable;
// import Types;
// import wxTypes;
// import String;
import wxUtil;
// import SplashSignals;

namespace mc
{
   DBManager::DBManager(long flags, bool initialize) : db::DBManager<DBase>(flags, initialize) {
      TraceCall;
      wx::pDocManager(this);
   }

   DBManager::~DBManager() { TraceCall; }

   auto DBManager::CreateDocument(const wxString &path, long flags) -> wxDocument * {
      TraceCall;
      CreateBits bits = longToCreateBits(flags);
      std::string pathStr = path.IsEmpty() ? "" : wx::from_wxString(path);
      std::cerr << "CreateDocument: " << path << " - " << pathStr << std::endl;
      auto result = getPath(bits, pathStr);
      if (result.first == CreateBits::Failed) {
         return nullptr;
      }
      ASSERT_MSG(result.first != CreateBits::Failed, "Failed to create document");
      return wxDocManager::CreateDocument(wx::to_wxString(result.second.string()), createBitsToLong(result.first));
   }
}