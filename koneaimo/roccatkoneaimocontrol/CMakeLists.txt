ADD_EXECUTABLE(roccatkoneaimocontrol koneaimocontrol.c)

TARGET_LINK_LIBRARIES(roccatkoneaimocontrol
  libroccatkoneaimo
  libroccatkoneplus
  libroccat
  ${GLIB2_LIBRARIES}
)

INSTALL(TARGETS roccatkoneaimocontrol
  DESTINATION bin
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

ADD_SUBDIRECTORY(man)
