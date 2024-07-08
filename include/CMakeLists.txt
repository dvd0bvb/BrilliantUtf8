add_library(BrilliantUtf8 INTERFACE)

target_include_directories(
    BrilliantUtf8 
    INTERFACE 
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

target_compile_features(
    BrilliantUtf8
    INTERFACE
        cxx_std_23
)