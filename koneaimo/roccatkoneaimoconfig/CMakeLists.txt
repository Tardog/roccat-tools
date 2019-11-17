ADD_EXECUTABLE(roccatkoneaimoconfig
  koneaimoconfig.c
  koneaimoconfig_configuration_dialog.c
  koneaimoconfig_profile_page.c
  koneaimoconfig_window.c
)

INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/koneplus/libroccatkonepluswidget
  ${CMAKE_SOURCE_DIR}/koneaimo/libroccatkoneaimowidget
)

TARGET_LINK_LIBRARIES(roccatkoneaimoconfig
  libroccatkoneaimo
  libroccatkoneaimowidget
  libroccatkoneplus
  libroccatkonepluswidget
  libroccat
  libroccatwidget
  ${GAMINGGEAR0_WIDGET_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
  ${GTK_LIBRARIES}
)

INSTALL(TARGETS roccatkoneaimoconfig
  DESTINATION bin
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

INSTALL(FILES roccatkoneaimoconfig.desktop
  DESTINATION share/applications
)
