file(GLOB SOURCES_LIST "src/*.cpp" "src/*.c")

idf_component_register(
    SRCS "${SOURCES_LIST}"
    INCLUDE_DIRS "include"
    REQUIRES EmCore driver espp__ads1x15
)
