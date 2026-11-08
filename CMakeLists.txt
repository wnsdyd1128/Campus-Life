cmake_minimum_required(VERSION 3.20)

project(CampusLife LANGUAGES CXX)

include(CTest)

add_executable(CampusLife
    include/Activity.hpp
    include/CampusLifeGame.hpp
    include/ConsoleUI.hpp
    include/PersistenceResult.hpp
    include/SaveManager.hpp
    include/ChickenActivity.hpp
    include/Course.hpp
    include/PartTimeJobActivity.hpp
    include/SleepActivity.hpp
    include/Student.hpp
    include/StudyActivity.hpp
    include/WeeklyPlan.hpp
    src/main.cpp
    src/CampusLifeGame.cpp
    src/ConsoleUI.cpp
    src/SaveManager.cpp
    src/ChickenActivity.cpp
    src/Course.cpp
    src/PartTimeJobActivity.cpp
    src/SleepActivity.cpp
    src/Student.cpp
    src/StudyActivity.cpp
    src/WeeklyPlan.cpp
)

target_include_directories(CampusLife PRIVATE include)
target_compile_features(CampusLife PRIVATE cxx_std_20)

set_target_properties(CampusLife PROPERTIES
    CXX_EXTENSIONS OFF
)

if(MSVC)
    target_compile_options(CampusLife PRIVATE /W4 /permissive- /EHsc)
else()
    target_compile_options(CampusLife PRIVATE -Wall -Wextra -Wpedantic)
endif()

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES
    src/main.cpp
    src/CampusLifeGame.cpp
    src/ConsoleUI.cpp
    src/SaveManager.cpp
    src/ChickenActivity.cpp
    src/Course.cpp
    src/PartTimeJobActivity.cpp
    src/SleepActivity.cpp
    src/Student.cpp
    src/StudyActivity.cpp
    src/WeeklyPlan.cpp
    include/Activity.hpp
    include/CampusLifeGame.hpp
    include/ConsoleUI.hpp
    include/PersistenceResult.hpp
    include/SaveManager.hpp
    include/ChickenActivity.hpp
    include/Course.hpp
    include/PartTimeJobActivity.hpp
    include/SleepActivity.hpp
    include/Student.hpp
    include/StudyActivity.hpp
    include/WeeklyPlan.hpp
)

set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    PROPERTY VS_STARTUP_PROJECT CampusLife
)

if(BUILD_TESTING)
    include(FetchContent)

    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.18.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_executable(StudentOwnershipTests
        tests/StudentOwnershipTests.cpp
        src/Course.cpp
        src/Student.cpp
    )

    target_include_directories(StudentOwnershipTests PRIVATE include)
    target_compile_features(StudentOwnershipTests PRIVATE cxx_std_20)
    target_link_libraries(StudentOwnershipTests PRIVATE GTest::gtest_main)

    if(MSVC)
        target_compile_options(StudentOwnershipTests PRIVATE /W4 /permissive- /EHsc)
    else()
        target_compile_options(StudentOwnershipTests PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    include(GoogleTest)
    gtest_add_tests(TARGET StudentOwnershipTests)

    add_executable(OperatorOverloadingTests
        tests/OperatorOverloadingTests.cpp
        src/ChickenActivity.cpp
        src/Course.cpp
        src/PartTimeJobActivity.cpp
        src/SleepActivity.cpp
        src/Student.cpp
        src/StudyActivity.cpp
        src/WeeklyPlan.cpp
    )

    target_include_directories(OperatorOverloadingTests PRIVATE include)
    target_compile_features(OperatorOverloadingTests PRIVATE cxx_std_20)
    target_link_libraries(OperatorOverloadingTests PRIVATE GTest::gtest_main)

    if(MSVC)
        target_compile_options(OperatorOverloadingTests PRIVATE /W4 /permissive- /EHsc)
    else()
        target_compile_options(OperatorOverloadingTests PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    gtest_add_tests(TARGET OperatorOverloadingTests)

    add_executable(ActivityInheritanceTests
        tests/ActivityInheritanceTests.cpp
        src/ChickenActivity.cpp
        src/Course.cpp
        src/PartTimeJobActivity.cpp
        src/SleepActivity.cpp
        src/Student.cpp
        src/StudyActivity.cpp
    )

    target_include_directories(ActivityInheritanceTests PRIVATE include)
    target_compile_features(ActivityInheritanceTests PRIVATE cxx_std_20)
    target_link_libraries(ActivityInheritanceTests PRIVATE GTest::gtest_main)

    if(MSVC)
        target_compile_options(ActivityInheritanceTests PRIVATE /W4 /permissive- /EHsc)
    else()
        target_compile_options(ActivityInheritanceTests PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    gtest_add_tests(TARGET ActivityInheritanceTests)

    add_executable(PolymorphismTests
        tests/PolymorphismTests.cpp
        src/CampusLifeGame.cpp
        src/ChickenActivity.cpp
        src/Course.cpp
        src/PartTimeJobActivity.cpp
        src/SleepActivity.cpp
        src/Student.cpp
        src/StudyActivity.cpp
        src/WeeklyPlan.cpp
    )

    target_include_directories(PolymorphismTests PRIVATE include)
    target_compile_features(PolymorphismTests PRIVATE cxx_std_20)
    target_link_libraries(PolymorphismTests PRIVATE GTest::gtest_main)

    if(MSVC)
        target_compile_options(PolymorphismTests PRIVATE /W4 /permissive- /EHsc)
    else()
        target_compile_options(PolymorphismTests PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    gtest_add_tests(TARGET PolymorphismTests)

    add_executable(ConsoleUITests
        tests/ConsoleUITests.cpp
        src/CampusLifeGame.cpp
        src/ConsoleUI.cpp
        src/SaveManager.cpp
        src/ChickenActivity.cpp
        src/Course.cpp
        src/PartTimeJobActivity.cpp
        src/SleepActivity.cpp
        src/Student.cpp
        src/StudyActivity.cpp
        src/WeeklyPlan.cpp
    )
    target_include_directories(ConsoleUITests PRIVATE include)
    target_compile_features(ConsoleUITests PRIVATE cxx_std_20)
    target_link_libraries(ConsoleUITests PRIVATE GTest::gtest_main)
    if(MSVC)
        target_compile_options(ConsoleUITests PRIVATE /W4 /permissive- /EHsc)
    else()
        target_compile_options(ConsoleUITests PRIVATE -Wall -Wextra -Wpedantic)
    endif()
    gtest_add_tests(TARGET ConsoleUITests)

endif()
