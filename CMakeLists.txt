file(GLOB SOURCES_LIST "src/*.cpp" "src/*.c")

idf_component_register(
    SRCS "${SOURCES_LIST}"
    INCLUDE_DIRS "include"
    REQUIRES espp_i2c espp_ads1x15 esp-idf-lib__bmp280
)
