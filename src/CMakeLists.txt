
unset (_HEADERS)
unset (_SOURCES)
unset (_MODULES)

set(_HEADERS
        ../include/MyHealthGuru/App.h
        ../include/MyHealthGuru/GlobalIDs.h
        ../include/MyHealthGuru/MyHealthGuru.h
        ../include/MyHealthGuru/Sizes.h)

add_subdirectory(app)
add_subdirectory(ctrls)
add_subdirectory(db)
add_subdirectory(interface)

include(BaseDirs.cmake)

list(REMOVE_DUPLICATES _SOURCES)
list(REMOVE_DUPLICATES _MODULES)
list(REMOVE_DUPLICATES _HEADERS)

set(GEN_OUTPUT "${BUILD_DIR}/AppConfigData.cpp")
list(APPEND _SOURCES "${GEN_OUTPUT}")

add_custom_command(
        OUTPUT "${GEN_OUTPUT}"
        COMMAND ${CMAKE_COMMAND}
            "-DINPUT_FILE=${APP_YAML_PATH}"
            "-DOUTPUT_FILE=${GEN_OUTPUT}"
            -P "${CMAKE_SOURCE_DIR}/cmake/EmbedFile.cmake"
        DEPENDS "${APP_YAML_PATH}"
        COMMENT "Generating embedded config: ${GEN_OUTPUT}"
        VERBATIM)

# @formatter:off
addLibrary(
        NAME          ${APP_NAME} EXECUTABLE
        USES          GUI CORE GFX
        VERSION       ${APP_VERSION}
        SOURCES       ${_SOURCES}
        MODULES       ${_MODULES}
        HEADERS       ${_HEADERS}
        BASE_DIRS     ${HEADER_BASE_DIRS}
        CXX_BASE_DIRS ${CXX_BASE_DIRS})
# @formatter:on

set_target_properties(${APP_NAME} PROPERTIES
        BUILD_RPATH "\$ORIGIN/../lib;\$ORIGIN/../lib64"
        INSTALL_RPATH "\$ORIGIN/../lib;\$ORIGIN/../lib64"
)
#################################################################################################################
#################################################################################################################


# Link libraries

# On WIN32, Gfx.dll is built with --export-all-symbols which embeds and re-exports all wx
# symbols. Linking the static wx archive alongside the Gfx import lib produces hundreds of
# duplicate symbol errors. Strip wx from both the library list and the wx-specific flags.
if(WIN32 AND TARGET HoffSoft::Gfx)
    list(REMOVE_ITEM HS_LibrariesList "wxWidgets::wxWidgets")
    set(HS_wxLibraries "")
    set(HS_wxFrameworks "")
endif()

target_link_libraries(${APP_NAME} PRIVATE ${DYN_FLAG}
        ${HS_LibrariesList}
)

target_compile_options(${APP_NAME} PRIVATE
        -Wno-ignored-attributes
)
#################################################################################################################
if (APPLE)
    set(ENTITLEMENTS resources/${APP_NAME}.entitlements)
    list(APPEND RESOURCE_FILES
            resources/appicons/wxmac.icns
            resources/appicons/wxmac.png)

    set_target_properties(${APP_NAME} PROPERTIES
            # @formatter:off
            MACOSX_BUNDLE_BUNDLE_NAME               "${APP_NAME}"
            MACOSX_BUNDLE_BUNDLE_VERSION            "${APP_VERSION}"
            MACOSX_BUNDLE_COPYRIGHT                 "Copyright ${APP_VENDOR}"
            MACOSX_BUNDLE_GUI_IDENTIFIER            "org.${APP_VENDOR_LC}.${APP_NAME_LC}"
            MACOSX_BUNDLE_ICON_FILE                 wxmac.icns
            MACOSX_BUNDLE_SHORT_VERSION_STRING      "1.0"
            RESOURCE                                "${RESOURCE_FILES}"
            XCODE_ATTRIBUTE_CODE_SIGN_ENTITLEMENTS  ${ENTITLEMENTS}
            XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY      "Apple Development"
            # @formatter:on
    )
    # macOS bundle executables must not have a versioned filename in Contents/MacOS/.
    # addLibrary sets VERSION 1.0.0 which makes CMake produce "MyHealthGuru-1.0.0" + a
    # "MyHealthGuru" symlink; fixup_bundle then processes the same inode twice and fails.
    # Setting VERSION "" produces "MyHealthGuru-" (empty suffix). Only a true property unset
    # produces the plain "MyHealthGuru" binary name.
    set_property(TARGET ${APP_NAME} PROPERTY VERSION)
    set_property(TARGET ${APP_NAME} PROPERTY SOVERSION)
#################################################################################################################
elseif (WIN32)
    # Include a RC file for windows
    set(RESOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/src/MyHealthGuru.rc")
    set(RESOURCE_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/include/MyHealthGuru/resource.h")

    if (MSVC)
        # Use manifest from wx.rc
        set_target_properties(${APP_NAME} PROPERTIES
                COMPILE_FLAGS "/DwxUSE_RC_MANIFEST"
                LINK_FLAGS "/MANIFEST:NO"
        )
    endif ()
elseif (LINUX)
endif ()

if (${BUILD_TYPE} STREQUAL "Debug")
    set(RESOURCES_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/resources)
endif ()
# @formatter:off
target_compile_definitions( ${APP_NAME} PRIVATE BUILDING_${APP_NAME} ${HS_DefinesList})
# Workaround: libc++ 21 adds [[abi_tag]] to _LIBCPP_HIDE_FROM_ABI. With PCH enabled, the PCH
# and module global fragments both define the same inline functions with the same mangled name
# → "definition with same mangled name" collision. Disabling PCH for module interface units
# prevents the double-definition. (Same workaround as Gfx.)
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set_target_properties(${APP_NAME} PROPERTIES DISABLE_PRECOMPILE_HEADERS ON)
    # libc++ 21 adds [[abi_tag]] to _LIBCPP_HIDE_FROM_ABI. MyHealthGuru imports Gfx BMIs (compiled
    # with _LIBCPP_NO_ABI_TAG) and also directly includes std headers via CoreData.h — the two
    # sets of declarations conflict unless all TUs use the same abi_tag setting.
    target_compile_definitions(${APP_NAME} PRIVATE _LIBCPP_NO_ABI_TAG)
    # C++ module interface files (.ixx) peak at 2-4 GB each. Cap MyHealthGuru's parallel compile
    # jobs at 4 to avoid swap thrashing, regardless of --parallel N passed to cmake --build.
    # This is per-target: Core is unaffected.
    if (CMAKE_GENERATOR MATCHES "Ninja")
        set_property(GLOBAL PROPERTY JOB_POOLS heavy_cxx=4)
        set_property(TARGET ${APP_NAME} PROPERTY JOB_POOL_COMPILE heavy_cxx)
    endif()
endif()
target_compile_options(     ${APP_NAME} PRIVATE ${HS_CompileOptionsList})
target_include_directories( ${APP_NAME} PRIVATE ${HS_IncludePathsList})
target_link_directories(    ${APP_NAME} PRIVATE ${HS_LibraryPathsList})
target_link_options(        ${APP_NAME} PRIVATE ${HS_LinkOptionsList})

# Ensure the executable sees wx headers/defs/libs since it includes Gfx headers that include <wx/wx.h>
if (GUI IN_LIST APP_FEATURES)
    target_compile_definitions( ${APP_NAME} PRIVATE ${HS_wxDefines})
    target_compile_options(     ${APP_NAME} PRIVATE ${HS_wxCompilerOptions})
    target_include_directories( ${APP_NAME} PRIVATE ${HS_wxIncludePaths})
    target_link_directories(    ${APP_NAME} PRIVATE ${HS_wxLibraryPaths})
    target_link_libraries(  ${APP_NAME} PRIVATE ${HS_wxLibraries} ${HS_wxFrameworks})
endif ()
# @formatter:on
