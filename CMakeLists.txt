cmake_minimum_required(VERSION 3.10)

project(Steganography VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDERD_REQUIRED TRUE)

add_executable(Steganography main.c stb_image.h stb_image_write.h)

target_link_options(Steganography PRIVATE -lm)

add_custom_command(
    TARGET Steganography POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/obrazki"
            "$<TARGET_FILE_DIR:Steganography>/obrazki"
)
