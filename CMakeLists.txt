########################################################################################################################
#                                              N o   n e e d   t o   c h a n g e   a n y t h i n g   b e l o w   h e r e
########################################################################################################################

cmake_minimum_required(VERSION 3.28.2) #                                              Absolute minimum required is 3.8.2
set(CMAKE_CXX_STANDARD 23) #  C++-23 used extensively. Think how much work you'll make for yourself before changing this

# Disable scanning globally BEFORE project() to satisfy compiler probes
unset(CMAKE_CXX_SCAN_FOR_MODULES CACHE)

# Define app variables first
include ("AppSpecific.cmake")
project(${APP_NAME} VERSION "${APP_VERSION}" DESCRIPTION "${DESCRIPTION}" LANGUAGES CXX)

## Re-enable it globally AFTER project() so it's available for your targets
set(CMAKE_CXX_SCAN_FOR_MODULES ON CACHE BOOL "Enable modules for targets" FORCE)

message(NOTICE "")
message(NOTICE "CMAKE_CXX_COMPILER_ID=${CMAKE_CXX_COMPILER_ID}")
message(NOTICE "")

if(NOT cmake_root)
    # Bootstrap: if cmake/ is absent or empty, fetch the shared cmake framework.
    # When built from MCA via ExternalProject, cmake_root is passed in and this block
    # is skipped entirely — MCA's own cmake/ is used instead.
    if(NOT EXISTS "${PROJECT_SOURCE_DIR}/cmake/CMakeLists.cmake")
        set(_cmake_framework_repo "git@github.com:ga2k/CMakeFiles.git")
        message(STATUS "[${APP_NAME}] cmake framework not found — fetching from ${_cmake_framework_repo}")
        file(REMOVE_RECURSE "${PROJECT_SOURCE_DIR}/cmake")
        execute_process(
            COMMAND git clone --depth 1 --branch master
                    "${_cmake_framework_repo}"
                    "${PROJECT_SOURCE_DIR}/cmake"
            RESULT_VARIABLE _clone_result
            OUTPUT_QUIET
        )
        if(_clone_result)
            message(FATAL_ERROR
                "[${APP_NAME}] Failed to fetch cmake framework.\n"
                "Clone manually: git clone ${_cmake_framework_repo} cmake")
        endif()
        unset(_clone_result)
        unset(_cmake_framework_repo)
    endif()

    file(GLOB_RECURSE cmakelists_cmake LIST_DIRECTORIES FALSE RELATIVE "${PROJECT_SOURCE_DIR}" FOLLOW_SYMLINKS CMakeLists.cmake)
    list(GET cmakelists_cmake 0 CMAKELISTS_CMAKE)
    get_filename_component(cmake_root "${CMAKELISTS_CMAKE}" DIRECTORY)
    get_filename_component(cmake_root "${cmake_root}" ABSOLUTE)
    list(APPEND cmake_to_search "${cmake_root}")

    if (MONOREPO AND DEFINED MONOREPO_ROOT_CMAKE AND EXISTS "${MONOREPO_ROOT_CMAKE}")
        get_filename_component(cmake_root "${MONOREPO_ROOT_CMAKE}" ABSOLUTE)
        list(APPEND cmake_to_search "${cmake_root}")
    endif ()
    if (MONOREPO AND NOT DEFINED MONOREPO_ROOT_CMAKE AND EXISTS "${CMAKE_SOURCE_DIR}/cmake")
        get_filename_component(cmake_root "${CMAKE_SOURCE_DIR}/cmake" ABSOLUTE)
        list(APPEND cmake_to_search "${cmake_root}")
    endif ()
    if (EXISTS "${PROJECT_SOURCE_DIR}/cmake")
        get_filename_component(cmake_root "${PROJECT_SOURCE_DIR}/cmake" ABSOLUTE)
        list(APPEND cmake_to_search "${cmake_root}")
    endif ()

    list(REMOVE_DUPLICATES cmake_to_search)
    set(cmake_searched "${cmake_to_search}")
    set(cmake_root)

    while(cmake_to_search)
        list(POP_FRONT cmake_to_search cmake_potential_root)
        if(EXISTS "${cmake_potential_root}")
            set(cmake_root "${cmake_potential_root}")
            break()
        endif ()
    endwhile ()

    if(NOT cmake_root)
        string(REPLACE ";" "\n" cmake_searched "${cmake_searched}")
        set(ERROR_MSG "CMakeLists.cmake was not found in any of these places :-\n${cmake_searched}")
        message(FATAL_ERROR ${ERROR_MSG})
    endif ()

endif ()

# Load the cmake framework. Runs whether cmake_root was discovered above or passed
# in externally (e.g. by MCA's ExternalProject_Add via -Dcmake_root=).
include("${cmake_root}/CMakeLists.cmake")

math(EXPR _w "${_term_cols} - 2")
string(REPEAT "═" ${_w} _li)
string(REPEAT " " ${_w} _sp)

set(_top "${BOLD}${CYAN}${BLUE_BG}╔${_li}╗${NC}")
set(_mid "${BOLD}${CYAN}${BLUE_BG}║${_sp}║${NC}")
set(_bot "${BOLD}${CYAN}${BLUE_BG}╚${_li}╝${NC}")

set(_txt "P r o c e s s i n g")
string(LENGTH "${_txt}   ${APP_NAME}" _txl)
math(EXPR _tel "(${_w} - ${_txl}) / 2")
math(EXPR _ter "${_w}  - ${_tel} - ${_txl}")

string(REPEAT " " ${_tel} _lil)
string(REPEAT " " ${_ter} _lir)

set(_txt "${BOLD}${WHITE}${BLUE_BG}${_txt}   ${BOLD}${YELLOW}${APP_NAME}")
set(_mod "${BOLD}${CYAN}${BLUE_BG}║${_lil}${_txt}${_lir}${BOLD}${CYAN}║${NC}")

msg(ALWAYS " ")
msg(ALWAYS "${_top}")
msg(ALWAYS "${_mid}")
msg(ALWAYS "${_mid}")
msg(ALWAYS "${_mod}")
msg(ALWAYS "${_mid}")
msg(ALWAYS "${_mid}")
msg(ALWAYS "${_bot}")
msg(ALWAYS " ")


if (NOT MONOREPO OR DEFINED MONOREPO_PROCESSED)
    include(${cmake_root}/project_setup.cmake)
endif ()
