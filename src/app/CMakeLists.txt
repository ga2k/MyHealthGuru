list(APPEND _SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/App.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/DBManager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/DBManager2.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Document.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Document2.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Frame.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/CategoryPages.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/SettingsPages.cpp
)
list(APPEND _MODULES
        ${CMAKE_CURRENT_SOURCE_DIR}/DBManager.ixx
        ${CMAKE_CURRENT_SOURCE_DIR}/Document.ixx
        ${CMAKE_CURRENT_SOURCE_DIR}/Frame.ixx
        ${CMAKE_CURRENT_SOURCE_DIR}/View.ixx
)

set(_SOURCES ${_SOURCES} PARENT_SCOPE)
set(_MODULES ${_MODULES} PARENT_SCOPE)
