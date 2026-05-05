list(APPEND _MODULES
        # groups/UserGroup.ixx

        # wizard/pages/AddressPage.ixx
        # wizard/pages/FilenamePage.ixx
        # wizard/pages/NamePage.ixx
        # wizard/pages/PersonPage.ixx
        ${CMAKE_CURRENT_SOURCE_DIR}/wizard/pages/WelcomePage.ixx
        ${CMAKE_CURRENT_SOURCE_DIR}/wizard/wizards/NewFileWizard.ixx
)

set(_MODULES ${_MODULES} PARENT_SCOPE)
