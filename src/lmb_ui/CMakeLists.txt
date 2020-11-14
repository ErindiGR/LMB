
file(
    GLOB LMBUI_SRC 
    "*.cpp"
    "imgui/*.cpp"
)


add_executable(
    lmb_ui 
    ${LMBUI_SRC}
)

target_include_directories(
    lmb_ui PUBLIC
    ${PROJECT_SOURCE_DIR}/includes/
    ${PROJECT_SOURCE_DIR}/third_party/includes/
)


target_link_libraries(
    lmb_ui 
    ${CMAKE_SOURCE_DIR}/third_party/libs/SDL2/libSDL2-2.0.dylib
    "-framework OpenGL"
    lmb
)