ADD_LIBRARY(libkoneaimoeventhandler SHARED
  koneaimo_dbus_server.c
  koneaimo_eventhandler.c
  koneaimo_eventhandler_channel.c
)

ADD_DEPENDENCIES(libkoneaimoeventhandler
  generated_headers
  koneaimo_dbus_bindings
)

SET_TARGET_PROPERTIES(libkoneaimoeventhandler
  PROPERTIES
    OUTPUT_NAME koneaimoeventhandler
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"KoneAIMOEventhandler\\\" -fvisibility=hidden"
)

TARGET_LINK_LIBRARIES(libkoneaimoeventhandler
  libroccat
  libroccatkoneplus
  libroccatkoneaimo
  libroccathelper
  ${GAMINGGEAR0_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
  ${GLIB2_LIBRARIES}
)

INSTALL(TARGETS libkoneaimoeventhandler DESTINATION ${EVENTHANDLER_PLUGIN_DIR})
