find_package(Qt6 REQUIRED COMPONENTS Test Core Network)

add_executable(test_utils test_utils.cpp)
target_link_libraries(test_utils PRIVATE Qt6::Test Qt6::Core)
target_include_directories(test_utils PRIVATE ${CMAKE_SOURCE_DIR})
add_test(NAME UtilsTest COMMAND test_utils)

add_executable(test_navigation
    test_navigation.cpp
    ${CMAKE_SOURCE_DIR}/navigationmanager.cpp
)
target_link_libraries(test_navigation PRIVATE Qt6::Test Qt6::Core)
target_include_directories(test_navigation PRIVATE ${CMAKE_SOURCE_DIR})
add_test(NAME NavigationTest COMMAND test_navigation)

add_executable(test_downloadmodel
    test_downloadmodel.cpp
    ${CMAKE_SOURCE_DIR}/downloadmodel.cpp
)
target_link_libraries(test_downloadmodel PRIVATE Qt6::Test Qt6::Core)
target_include_directories(test_downloadmodel PRIVATE ${CMAKE_SOURCE_DIR})
add_test(NAME DownloadModelTest COMMAND test_downloadmodel)

add_executable(test_settings
    test_settings.cpp
    ${CMAKE_SOURCE_DIR}/settingsmanager.cpp
)
target_link_libraries(test_settings PRIVATE Qt6::Test Qt6::Core)
target_include_directories(test_settings PRIVATE ${CMAKE_SOURCE_DIR})
add_test(NAME SettingsTest COMMAND test_settings)
