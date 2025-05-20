cmake_minimum_required(VERSION 3.14)
project(HelloWorld VERSION 0.1 LANGUAGES CXX)

include(CTest)
enable_testing()

include(FetchContent)
set(FETCHCONTENT_BASE_DIR ${CMAKE_SOURCE_DIR}/third_party)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG        release-3.2.14
)

FetchContent_MakeAvailable(SDL3)

add_executable(HelloWorld main.cpp)
set_property(TARGET HelloWorld PROPERTY CXX_STANDARD 17)
target_include_directories(HelloWorld PUBLIC ${SDL3_SOURCE_DIR}/include)
target_link_libraries(HelloWorld PUBLIC SDL3::SDL3)

# --- Copiar DLL en Windows ---
if (WIN32)
    add_custom_command(
        TARGET HelloWorld POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            "${SDL3_BINARY_DIR}/$<CONFIG>/SDL3.dll"
            "$<TARGET_FILE_DIR:HelloWorld>"
    )
endif()