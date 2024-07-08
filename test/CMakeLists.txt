add_executable(BrilliantUtf8Test
    DecodeTest.cpp
)

find_package(GTest REQUIRED)

target_link_libraries(
    BrilliantUtf8Test 
    PRIVATE 
        GTest::gtest_main
        BrilliantUtf8
)

target_compile_features(
    BrilliantUtf8Test
    PRIVATE
        cxx_std_23
)

#ripped from cmake's example
set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")

target_compile_options(
    BrilliantUtf8Test
    PRIVATE
        "$<${gcc_like_cxx}:-Wall;-Wextra;-Wshadow;-Wunused>"
        "$<$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang>:-stdlib=stdc++"
        "$<${msvc_cxx}:$<BUILD_INTERFACE:-W3>>"
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/quickbrown.txt ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/utf8_invalid.txt ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

gtest_discover_tests(BrilliantUtf8Test)