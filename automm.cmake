
MACRO(CAR var)
    SET(${var} ${ARGV1})
ENDMACRO(CAR)

MACRO(CDR var junk)
    SET(${var} ${ARGN})
ENDMACRO(CDR)

MACRO(automm)

    CAR(_projectname ${ARGN})
    CDR(_list ${ARGN})

    SET(AUTOMM_INCLUDE_DIR ${CMAKE_BINARY_DIR}/automm)
    include_directories(${AUTOMM_INCLUDE_DIR})
    SET(AUTOMM_DIR ${AUTOMM_INCLUDE_DIR}/${_projectname})

    source_group(AutoMirrors FILES REGULAR_EXPRESSION .mm.cpp)

    SET(_matching_FILES )
    FOREACH (_current_FILE ${_list})
        GET_FILENAME_COMPONENT(_abs_FILE ${_current_FILE} ABSOLUTE)

        GET_SOURCE_FILE_PROPERTY(_skip ${_abs_FILE} SKIP_AUTOMM)

        IF ( NOT _skip AND EXISTS ${_abs_FILE} )

            FILE(READ ${_abs_FILE} _contents)

            GET_FILENAME_COMPONENT(_abs_PATH ${_abs_FILE} PATH)
            GET_FILENAME_COMPONENT(_current_BASE ${_abs_FILE} NAME_WE)
            GET_FILENAME_COMPONENT(_current_NAME ${_abs_FILE} NAME)

            SET(ENUM_BLOCK_SYMBOL ${_projectname}_${_current_NAME})
            STRING(REPLACE "." "_" ENUM_BLOCK_SYMBOL ${ENUM_BLOCK_SYMBOL})
            STRING(TOLOWER "${ENUM_BLOCK_SYMBOL}" ENUM_BLOCK_SYMBOL)

            FILE(MAKE_DIRECTORY ${AUTOMM_DIR})

            STRING(REGEX MATCHALL "# *include +[^ ]+\\.mm.(h|cpp)[\">]" _match "${_contents}")
            IF(_match)
                add_custom_command(
                    OUTPUT ${AUTOMM_DIR}/${_current_NAME}.mm.cpp
                    COMMAND mm ${_abs_FILE} ${AUTOMM_DIR}/${_current_NAME}.mm.cpp ${AUTOMM_DIR}/${_current_NAME}.mm.h ${ENUM_BLOCK_SYMBOL}
                    DEPENDS mm ${_abs_FILE}
                    )
                set_source_files_properties(${AUTOMM_DIR}/${_current_NAME}.mm.cpp PROPERTIES SKIP_AUTOMM TRUE)
                set_source_files_properties(${AUTOMM_DIR}/${_current_NAME}.mm.cpp PROPERTIES GENERATED TRUE)
                set_property(SOURCE ${_abs_PATH}/${_current_BASE}.cpp APPEND PROPERTY OBJECT_DEPENDS ${AUTOMM_DIR}/${_current_NAME}.mm.cpp)
            ENDIF()
        ENDIF()
    ENDFOREACH (_current_FILE)
ENDMACRO(automm)

