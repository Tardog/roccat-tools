ADD_LIBRARY(libroccatkoneaimo SHARED
  koneaimo.c
  koneaimo_config.c
  koneaimo_dbus_services.c
  koneaimo_gfx.c
  koneaimo_rmp_accessors.c
  koneaimo_rmp_conversions.c
  koneaimo_rmp_file.c
  koneaimo_rmp_high_level.c
)

ADD_DEPENDENCIES(libroccatkoneaimo
  generated_headers
  koneaimo_dbus_bindings
)

TARGET_LINK_LIBRARIES(libroccatkoneaimo
  libroccat
  libroccatkoneplus
  libroccathelper
  ${DBUS-GLIB_LIBRARIES}
  ${GAMINGGEAR0_LIBRARY}
  ${GLIB2_LIBRARIES}
)

TARGET_LINK_LIBRARIES(libroccatkoneaimo LINK_INTERFACE_LIBRARIES)

SET_TARGET_PROPERTIES(libroccatkoneaimo
  PROPERTIES
    VERSION ${V_MAJOR}.${V_MINOR}.${V_PATCH}
    SOVERSION ${V_MAJOR}
    OUTPUT_NAME roccatkoneaimo
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"KoneAIMO\\\""
)

INSTALL(TARGETS libroccatkoneaimo DESTINATION ${LIBDIR})
