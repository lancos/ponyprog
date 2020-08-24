#
# generate defines from xml files
# E.Kalinowski
#

FILE(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/ics/*.xml")

SET(DEFINES_NAME "${CMAKE_CURRENT_SOURCE_DIR}/SrcPony/defines.h")

FILE(WRITE ${DEFINES_NAME} "//\n// automatic generated file\n//\n\n")
FILE(APPEND ${DEFINES_NAME} "#ifndef AUTODEFINES_H\n\n")
FILE(APPEND ${DEFINES_NAME} "#define AUTODEFINES_H\n\n")


FOREACH(file ${files})
  FILE(READ "${file}" contents)

  STRING(REGEX REPLACE "\n" ";" contents "${contents}")

  SET(RUN_NR 0)
  SET(CURR_GROUP "")
  SET(CURR_GROUP_ID "")

  FOREACH(line ${contents})
    # remove the comments from line
    STRING(REGEX REPLACE "<!--(.)*-->" "" line "${line}")

    STRING(REGEX MATCH "group=\"[0-9a-zA-Z_]+\"" GR_NAME ${line})
    STRING(REGEX REPLACE "group=" "" GR_NAME "${GR_NAME}")
    STRING(REGEX REPLACE "\"" "" GR_NAME "${GR_NAME}")

    STRING(REGEX MATCH "id=\"0x[0-9a-fA-F]+\"" GR_ID ${line})
    STRING(REGEX REPLACE "id=" "" GR_ID "${GR_ID}")
    STRING(REGEX REPLACE "\"" "" GR_ID "${GR_ID}")

    IF (NOT ${GR_NAME} STREQUAL "")
      MESSAGE(STATUS "group ${GR_NAME} ${GR_ID}")
      FILE(APPEND ${DEFINES_NAME} "\n#define ${GR_NAME} ${GR_ID}\n")
      SET(CURR_GROUP_ID "${GR_ID}")
      SET(RUN_NR 0)
    ENDIF()

    STRING(REGEX MATCH "def=\"[0-9a-zA-Z_]+\"" DEF_NAME ${line})
    STRING(REGEX REPLACE "def=" "" DEF_NAME "${DEF_NAME}")
    STRING(REGEX REPLACE "\"" "" DEF_NAME "${DEF_NAME}")

    IF (NOT ${DEF_NAME} STREQUAL "")
      MATH(EXPR RUN_NR "${RUN_NR}+1" OUTPUT_FORMAT HEXADECIMAL)
      #MESSAGE(STATUS "${DEF_NAME} ${RUN_NR}")

      STRING(REGEX REPLACE "0x" "" TMP_NR "${RUN_NR}")

      # correct length of hex number
      STRING(LENGTH "${TMP_NR}" STR_LEN)
      IF (${STR_LEN} EQUAL 1)
        SET(RUN_ID "000${TMP_NR}")
      ELSE()
        SET(RUN_ID "00${TMP_NR}")
      ENDIF()

      #formatting with spaces
      STRING(LENGTH "${DEF_NAME}" NAME_LEN)
      SET(SPACES " ")
      MATH(EXPR NUM_SPACES "14-${NAME_LEN}")
      foreach(i RANGE ${NUM_SPACES})
        SET(SPACES "${SPACES} ")
      ENDFOREACH()
      #MESSAGE (STATUS "${DEF_NAME} ${CURR_GROUP_ID}${RUN_ID}")
      FILE(APPEND ${DEFINES_NAME} "#define ${DEF_NAME}${SPACES}${CURR_GROUP_ID}${RUN_ID}\n")
    ENDIF()

  ENDFOREACH()

ENDFOREACH()

FILE(APPEND ${DEFINES_NAME} "\n#endif //AUTODEFINES_H\n")
