cmake_minimum_required(VERSION 3.0.0)
project(tinyMp3Player VERSION 0.1.0)

include(CTest)
enable_testing()
set(EXECUTABLE_NAME "tinyMP3")
set(CMAKE_CXX_STANDARD 14)


set(SFML_DIR "C:\\Program Files\\SFM251\\SFML-2.5.1-sources\\SFML-2.5.1\\buildSFML")
set(ImGui-SFML_DIR "C:\\Program Files (x86)\\imgui_sfml\\lib\\cmake\\ImGui-SFML")
set(BASS_DIR "E:\\tinyMP3\\bass\\bass24")
include_directories("C:\\Program Files (x86)\\imgui_sfml\\include")
include_directories("E:\\tinyMP3\\bass\\bass24\\c")

find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)
find_package(ImGui-SFML REQUIRED)






add_executable(${EXECUTABLE_NAME}  main.cpp )
LINK_DIRECTORIES("E:\\tinyMP3\\bass\\bass24\\c")
target_link_libraries(${EXECUTABLE_NAME} "E:\\tinyMP3\\bass\\bass24\\c\\x64\\bass.lib" sfml-system sfml-window sfml-graphics sfml-audio ImGui-SFML::ImGui-SFML stdc++fs )


set(CPACK_PROJECT_NAME ${PROJECT_NAME})

set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})


include(CPack)
