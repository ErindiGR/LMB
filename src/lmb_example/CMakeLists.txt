
file(
    GLOB LMBEXAMPLE_SRC 
    "*.cpp"
)

add_definitions(
    -DDRAW
)


add_executable(
    lmb_example ${LMBEXAMPLE_SRC}
)

target_include_directories(
    lmb_example PUBLIC
    ${PROJECT_SOURCE_DIR}/includes/
    ${PROJECT_SOURCE_DIR}/third_party/includes/
)


target_link_libraries(
    lmb_example 
    lmb
    ${CMAKE_SOURCE_DIR}/third_party/libs/SDL2/libSDL2-2.0.dylib
    "-framework OpenGL"
)