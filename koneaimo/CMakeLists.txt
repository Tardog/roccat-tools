list_contains(DEVICES "koneaimo;konexdt;konepure;konepuremilitary;konepureoptical;koneaimooptical" contains)
IF(contains)
  add_dbus_bindings(koneaimo)

  INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/koneplus/libroccatkoneplus
    ${CMAKE_SOURCE_DIR}/koneaimo/libroccatkoneaimo
    ${CMAKE_BINARY_DIR}/koneaimo
  )

  ADD_SUBDIRECTORY(libroccatkoneaimo)
  ADD_SUBDIRECTORY(libroccatkoneaimowidget)
ENDIF()

list_contains(DEVICES koneaimo contains)
IF(contains)
  ADD_SUBDIRECTORY(roccatkoneaimoconfig)
  ADD_SUBDIRECTORY(roccatkoneaimocontrol)
  ADD_SUBDIRECTORY(libkoneaimoeventhandler)
  ADD_SUBDIRECTORY(libkoneaimogfxplugin)
ENDIF()
