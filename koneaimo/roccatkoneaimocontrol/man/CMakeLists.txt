FOREACH(language ${TRANSLATION_LANGUAGES})
  INSTALL(FILES roccatkoneaimocontrol.${language}.1
    DESTINATION share/man/${language}/man1
    RENAME roccatkoneaimocontrol.1
  )
ENDFOREACH()
