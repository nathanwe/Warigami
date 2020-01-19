INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Game by WizardPeople")
SET(CPACK_PACKAGE_VENDOR "Digipen")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/docs/ReadMe.txt")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/docs/Copyright.txt")


SET(CPACK_PACKAGE_VERSION_MAJOR ${WizardEngine_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${WizardEngine_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH "0")

SET(CPACK_PACKAGE_INSTALL_DIRECTORY WizardGame)

IF(WIN32 AND NOT UNIX)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  # SET(CPACK_GENERATOR "NSIS")
  # SET(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}\\\\hat.ico")
  SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\WizardGame.exe")
  SET(CPACK_NSIS_DISPLAY_NAME "WizardGame")
  SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\localhost")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\localhost")
  SET(CPACK_NSIS_CONTACT "saveliy.baranov@digipen.edu")
  SET(CPACK_NSIS_MODIFY_PATH ON)
ELSE(WIN32 AND NOT UNIX)
  SET(CPACK_STRIP_FILES "bin/WizardGame")
  SET(CPACK_SOURCE_STRIP_FILES "")
ENDIF(WIN32 AND NOT UNIX)
SET(CPACK_PACKAGE_EXECUTABLES "WizardGame" "Wizard Game")

INCLUDE(CPack)
