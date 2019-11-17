ADD_LIBRARY(libroccatkoneaimowidget SHARED
  koneaimo_color_frame.c
  koneaimo_cpi_selector.c
)

TARGET_LINK_LIBRARIES(libroccatkoneaimowidget
  libroccatkoneplus
  libroccatkoneaimo
  libroccatwidget
  ${GTK_LIBRARIES}
)

TARGET_LINK_LIBRARIES(libroccatkoneaimowidget LINK_INTERFACE_LIBRARIES)

SET_TARGET_PROPERTIES(libroccatkoneaimowidget
  PROPERTIES
    VERSION ${V_MAJOR}.${V_MINOR}.${V_PATCH}
    SOVERSION ${V_MAJOR}
    OUTPUT_NAME roccatkoneaimowidget
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"KoneaimoWidget\\\""
)

INSTALL(TARGETS libroccatkoneaimowidget DESTINATION ${LIBDIR})
