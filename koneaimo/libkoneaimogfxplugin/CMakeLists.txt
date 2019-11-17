ADD_LIBRARY(libkoneaimogfxplugin SHARED
  koneaimogfxplugin.c
)

INCLUDE_DIRECTORIES(
  ${DBUS_INCLUDE_DIRS}
)

TARGET_LINK_LIBRARIES(libkoneaimogfxplugin
  libroccat
  libroccathelper
  ${DBUS_LIBRARIES}
  ${GLIB2_LIBRARIES}
)

SET_TARGET_PROPERTIES(libkoneaimogfxplugin
  PROPERTIES
    OUTPUT_NAME koneaimogfxplugin
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"KoneAIMOGfxplugin\\\" -fvisibility=hidden"
)

INSTALL(TARGETS libkoneaimogfxplugin DESTINATION ${GAMINGGEAR0_FX_PLUGIN_DIR})
