cmake_minimum_required (VERSION 3.5)

file(GLOB_RECURSE SOURCE_FILES src/*.c)
file(GLOB_RECURSE HEADER_FILES src/*.h)

if (NOT DEFINED COMPONENT_DIR)

    project(HD44780)

    include_directories(src)

    add_library(lcdgfx STATIC ${HEADER_FILES} ${SOURCE_FILES})

else()

    idf_component_register(SRCS ${SOURCE_FILES}
                           INCLUDE_DIRS
                               "src"
                           REQUIRES
                               "driver esp_rom freertos")

endif()
