cmake_minimum_required(VERSION 3.28.2) #                                              Absolute minimum required is 3.8.2
set(CMAKE_CXX_STANDARD 23) #  C++-23 used extensively. Think how much work you'll make for yourself before changing this

# Disable scanning globally BEFORE project() to satisfy compiler probes
unset(CMAKE_CXX_SCAN_FOR_MODULES CACHE)

include(AppSpecific.cmake)
string(REGEX MATCH "^([0-9]+)\\.([0-9]+)\\.([0-9]+)$" _ "${APP_VERSION}")

set(APP_VERSION_MAJOR "${CMAKE_MATCH_1}")
set(APP_VERSION_MINOR "${CMAKE_MATCH_2}")
set(APP_VERSION_PATCH "${CMAKE_MATCH_3}")
set(PACK_VERSION "${APP_VERSION}")

# @formatter:off
project(${APP_NAME}
        VERSION      ${APP_VERSION_MAJOR}.${APP_VERSION_MINOR}.${APP_VERSION_PATCH}
        DESCRIPTION "${APP_DESCRIPTION}"
        LANGUAGES    CXX)
# @formatter:on

include(GNUInstallDirs)

## Re-enable it globally AFTER project() so it's available for your targets
set(CMAKE_CXX_SCAN_FOR_MODULES ON CACHE BOOL "Enable modules for targets" FORCE)

message("")
message("CMAKE_CXX_COMPILER_ID=${CMAKE_CXX_COMPILER_ID}")
message(" ")

set(cmake_root "${CMAKE_SOURCE_DIR}/cmake")

include("${cmake_root}/CMakeLists.cmake")
include("${cmake_root}/project_setup.cmake")
include("${cmake_root}/project_install.cmake")
include("${cmake_root}/clearVars.cmake")

project_setup(src)

include(InstallRequiredSystemLibraries)

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "${APP_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${APP_VERSION_MINOR}")
set(CPACK_PACKAGE_NAME "HoffSoft")
set(CPACK_COMPONENTS_GROUPING ALL_COMPONENTS_IN_ONE)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # NSIS produces a proper .exe installer.
    # The custom template (cmake/NSIS.template.in) adds "Target amd64-unicode" so
    # makensis uses amd64 plugin DLLs even though it defaults to x86 target.
    if(EXISTS "$ENV{HOME}/.local/bin/makensis" OR EXISTS "/usr/bin/makensis" OR EXISTS "/usr/local/bin/makensis")
        set(CPACK_GENERATOR "NSIS;ZIP")
        set(CPACK_NSIS_ARCHITECTURE "x86_64")
        # cmake/NSIS.template.in adds "Target amd64-unicode" so makensis uses amd64
        # plugin DLLs even though it defaults to x86 target.  CPACK_MODULE_PATH is
        # the search path cpack uses for FindTemplate(); it must come before include(CPack).
        set(CPACK_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
    else()
        set(CPACK_GENERATOR "ZIP")
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(CPACK_GENERATOR "DragNDrop;TGZ")
else()
    set(CPACK_GENERATOR "STGZ;RPM")
endif()

# Set the install prefix _inside_ each generated package to the system path
# (e.g. /usr/local). Without this, CPack would inherit CMAKE_INSTALL_PREFIX
# which points at the build-time stage dir — wrong for a redistributable package.
# CPACK_PACKAGING_INSTALL_PREFIX is the correct way to override this; it lets
# CPack still manage per-component/group staging directories (required when
# CPACK_ARCHIVE_COMPONENT_INSTALL=ON) rather than flattening everything via
# a hard-coded 4th element in CPACK_INSTALL_CMAKE_PROJECTS.
set(CPACK_PACKAGING_INSTALL_PREFIX "${_SYSPATH}")

include(CPack)
