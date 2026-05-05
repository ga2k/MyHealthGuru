module;

#include <wx/wx.h>
#include <wx/docview.h>
#include "Core/Util.h"

#include <filesystem>

export module MC.DBManager;
export import DBManager;

import MC.DBase;

namespace mc {
export class DBManager : public db::DBManager<DBase> {
 public:
   DBManager (long flags = 0, bool initialize = true);
   ~DBManager() override;
   auto CreateDocument (const wxString &path, long flags) -> wxDocument * override;

 protected:

   [[nodiscard]] auto firstRun (bool forceRun) -> DBHandlerResult override;
   // wxDECLARE_DYNAMIC_CLASS (DBManager);
};

/*
 *
 *
 *
 *
 *
 *
 *
 */

export class DocTemplate : public CDocTemplate<DBase> {
 protected:
   auto validateFile (const std::filesystem::path &path, int magic, int minVersion = 0) -> bool override;

 public:
   using CDocTemplate::CDocTemplate;
   ~DocTemplate() override = default;
};
} // namespace mc

