module;

// clang-format off
#include "Core/Core.h"


// clang-format on

export module MC.DBase;
export import Database;

export namespace mc {
using namespace db;

class DBase : public Database {

 public:
   DBase() = default;
   DBase (const DBase &) = delete;
   ~DBase() override = default;

   auto newDB(const std::filesystem::path &name) -> bool override;
   auto openDB(const std::filesystem::path &name, int magic, int minVersion) -> bool override;
   auto closeDB() -> void override;

   auto initializeSchema() -> std::shared_ptr<TableLoader> override;
   auto initializeDefaultData() -> bool override;
   auto validateDB (const std::filesystem::path &name, int magic, int minVersion) -> bool override;
   static auto isValid (const std::filesystem::path &name, int magic, int minVersion) -> bool;
};
} // namespace mc
