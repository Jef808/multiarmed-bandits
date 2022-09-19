set(CMAKE_EXPORT_COMPILE_COMMANDS on)
include_directories("/usr/lib/emscripten/system/include/")
add_executable(test test.C)
