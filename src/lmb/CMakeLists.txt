
file(
    GLOB LMB_SRC
    "*.cpp"
    "*/*.cpp"
)

add_library(
    lmb SHARED 
    ${LMB_SRC}
)

target_include_directories(
    lmb PUBLIC
    ${PROJECT_SOURCE_DIR}/includes/
    ${PROJECT_SOURCE_DIR}/third_party/includes/
)