########################################################################################################################
#                                                                          C u s t o m i z e   y o u r   a p p   h e r e
set(APP_NAME "MyCare") #                                                                                   Your app name
set(APP_VENDOR "HoffSoft") #    Your Company Name or other identifying tag. Multiple apps by the same vendor are grouped
set(APP_DESCRIPTION "My complete health and allied services record-keeper") #                       Aww, that's so sweet
set(APP_VERSION "1.0.0") #                             The app version number. Components have their own version numbers
set(APP_DATE "2026-02-21") #                                                                                  Build Date
set(APP_TYPE "Executable") #                                                                       Library or Executable

set(APP_DATA_VERSION 1000) #
set(APP_MAGIC_BEAN 0xC0FFEE) #      Sanity check value stored in created databases to ensure this datafile belongs to us

set(APP_FILE_TYPES "MyCare Documents&All Databases&All Files") #        Ampersand separated list of allowable file types
set(APP_FILE_FILTERS "*.mcdb&*.db&*.*") #                                The file extensions that go with the file types
set(APP_FILE_EXTN "mcdb") #                           The actual filename extension to be used by files used by this app
set(APP_DOC_CLASS "Document") #   Class name for your document. Usually "Document", leave it unless you have a real need
set(APP_VIEW_CLASS "View") #              Class name for your view. Usually "View", leave it unless you have a real need

set(APP_GENERATE_RECORDSETS ${CMAKE_SOURCE_DIR}/resources/configs/generator-source) #       Dynamic yaml-to-ixx creation
set(APP_GENERATE_UI_CLASSES ${CMAKE_SOURCE_DIR}/resources/configs/generator-source) #       Dynamic yaml-to-ixx creation

set(APP_SHOW_SIZER_INFO_IN_SOURCE ON) #                 Generate sizer info comments in generated UI modules source code

set(APP_FEATURES #                         Available : APPEARANCE BOOST COMMS CORE DATABASE GFX LOGGER PRINT SSL GUI
        "CORE PACKAGE FindCore"
        "GFX  PACKAGE FindGfx"
        SSL
        DATABASE
        GUI
)

set(APP_CONSUMES_PLUGINS
        Appearance
        Logger
        Print
)

set(APP_LOCAL_RESOURCES "resources")
set(APP_GLOBAL_RESOURCES "git@github.com:ga2k/HoffSoft_resources.git")
set(APP_DEBUG ON)
########################################################################################################################
