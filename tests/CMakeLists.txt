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

add_executable(test_constants test_constants.cpp)
target_link_libraries(test_constants PRIVATE Qt6::Test Qt6::Core)
target_include_directories(test_constants PRIVATE ${CMAKE_SOURCE_DIR})
target_compile_definitions(test_constants PRIVATE
    ${BROWSER_VERSION_DEFINITION}
    EXPECTED_VERSION="${PROJECT_VERSION}"
)
add_test(NAME ConstantsTest COMMAND test_constants)

add_executable(test_downloadmanager
    test_downloadmanager.cpp
    ${CMAKE_SOURCE_DIR}/downloadmanager.cpp
    ${CMAKE_SOURCE_DIR}/downloadmodel.cpp
)
target_link_libraries(test_downloadmanager PRIVATE Qt6::Test Qt6::Core Qt6::Network)
target_include_directories(test_downloadmanager PRIVATE ${CMAKE_SOURCE_DIR})
add_test(NAME DownloadManagerTest COMMAND test_downloadmanager)

add_executable(test_bookmarks
    test_bookmarks.cpp
    ${CMAKE_SOURCE_DIR}/bookmarkmanager.cpp
)
target_link_libraries(test_bookmarks PRIVATE Qt6::Test Qt6::Core)
target_include_directories(test_bookmarks PRIVATE ${CMAKE_SOURCE_DIR})
add_test(NAME BookmarksTest COMMAND test_bookmarks)

find_package(Qt6 QUIET COMPONENTS Widgets WebEngineWidgets WebEngineCore)
if(TARGET Qt6::WebEngineWidgets AND TARGET Qt6::Widgets)
    add_executable(test_browsertab
        test_browsertab.cpp
        ${CMAKE_SOURCE_DIR}/browsertab.cpp
        ${CMAKE_SOURCE_DIR}/tabmanager.cpp
        ${CMAKE_SOURCE_DIR}/navigationmanager.cpp
        ${CMAKE_SOURCE_DIR}/settingsmanager.cpp
        ${CMAKE_SOURCE_DIR}/thememanager.cpp
    )
    target_link_libraries(test_browsertab PRIVATE
        Qt6::Test Qt6::Core Qt6::Widgets Qt6::WebEngineWidgets Qt6::WebEngineCore)
    target_include_directories(test_browsertab PRIVATE ${CMAKE_SOURCE_DIR})
    add_test(NAME BrowserTabTest COMMAND test_browsertab)
    set_tests_properties(BrowserTabTest PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen;QTWEBENGINE_CHROMIUM_FLAGS=--no-sandbox --disable-gpu")
endif()

find_package(Qt6 QUIET COMPONENTS Widgets)
if(TARGET Qt6::Widgets)
    add_executable(test_bookmarksdialog
        test_bookmarksdialog.cpp
        ${CMAKE_SOURCE_DIR}/bookmarksdialog.cpp
        ${CMAKE_SOURCE_DIR}/bookmarkmanager.cpp
    )
    target_link_libraries(test_bookmarksdialog PRIVATE Qt6::Test Qt6::Core Qt6::Widgets)
    target_include_directories(test_bookmarksdialog PRIVATE ${CMAKE_SOURCE_DIR})
    add_test(NAME BookmarksDialogTest COMMAND test_bookmarksdialog)
    set_tests_properties(BookmarksDialogTest PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
endif()

if(TARGET Qt6::Widgets)
    add_executable(test_thememanager
        test_thememanager.cpp
        ${CMAKE_SOURCE_DIR}/thememanager.cpp
        ${CMAKE_SOURCE_DIR}/settingsmanager.cpp
    )
    target_link_libraries(test_thememanager PRIVATE Qt6::Test Qt6::Core Qt6::Widgets)
    target_include_directories(test_thememanager PRIVATE ${CMAKE_SOURCE_DIR})
    add_test(NAME ThemeManagerTest COMMAND test_thememanager)
    set_tests_properties(ThemeManagerTest PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

    add_executable(test_preferences
        test_preferences.cpp
        ${CMAKE_SOURCE_DIR}/preferencesdialog.cpp
        ${CMAKE_SOURCE_DIR}/settingsmanager.cpp
    )
    target_link_libraries(test_preferences PRIVATE Qt6::Test Qt6::Core Qt6::Widgets)
    target_include_directories(test_preferences PRIVATE ${CMAKE_SOURCE_DIR})
    add_test(NAME PreferencesTest COMMAND test_preferences)
    set_tests_properties(PreferencesTest PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

    add_executable(test_downloaddialog
        test_downloaddialog.cpp
        ${CMAKE_SOURCE_DIR}/downloaddialog.cpp
        ${CMAKE_SOURCE_DIR}/settingsmanager.cpp
    )
    target_link_libraries(test_downloaddialog PRIVATE Qt6::Test Qt6::Core Qt6::Widgets)
    target_include_directories(test_downloaddialog PRIVATE ${CMAKE_SOURCE_DIR})
    add_test(NAME DownloadDialogTest COMMAND test_downloaddialog)
    set_tests_properties(DownloadDialogTest PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

    add_executable(test_downloadnotification
        test_downloadnotification.cpp
        ${CMAKE_SOURCE_DIR}/downloadnotification.cpp
    )
    target_link_libraries(test_downloadnotification PRIVATE Qt6::Test Qt6::Core Qt6::Widgets)
    target_include_directories(test_downloadnotification PRIVATE ${CMAKE_SOURCE_DIR})
    add_test(NAME DownloadNotificationTest COMMAND test_downloadnotification)
    set_tests_properties(DownloadNotificationTest PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
endif()

if(TARGET Qt6::Widgets AND TARGET Qt6::WebEngineWidgets AND TARGET Qt6::PrintSupport)
    find_package(Qt6 QUIET COMPONENTS PrintSupport)
    add_executable(test_mainwindow
        test_mainwindow.cpp
        ${CMAKE_SOURCE_DIR}/mainwindow.cpp
        ${CMAKE_SOURCE_DIR}/browsertab.cpp
        ${CMAKE_SOURCE_DIR}/tabmanager.cpp
        ${CMAKE_SOURCE_DIR}/navigationmanager.cpp
        ${CMAKE_SOURCE_DIR}/settingsmanager.cpp
        ${CMAKE_SOURCE_DIR}/thememanager.cpp
        ${CMAKE_SOURCE_DIR}/bookmarkmanager.cpp
        ${CMAKE_SOURCE_DIR}/bookmarksdialog.cpp
        ${CMAKE_SOURCE_DIR}/downloadmanager.cpp
        ${CMAKE_SOURCE_DIR}/downloadmodel.cpp
        ${CMAKE_SOURCE_DIR}/downloaddialog.cpp
        ${CMAKE_SOURCE_DIR}/downloadnotification.cpp
        ${CMAKE_SOURCE_DIR}/preferencesdialog.cpp
        ${CMAKE_SOURCE_DIR}/findbar.cpp
    )
    target_link_libraries(test_mainwindow PRIVATE
        Qt6::Test Qt6::Core Qt6::Widgets Qt6::Network Qt6::WebEngineWidgets
        Qt6::WebEngineCore Qt6::PrintSupport)
    target_include_directories(test_mainwindow PRIVATE ${CMAKE_SOURCE_DIR})
    add_test(NAME MainWindowTest COMMAND test_mainwindow)
    set_tests_properties(MainWindowTest PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen;QTWEBENGINE_CHROMIUM_FLAGS=--no-sandbox --disable-gpu")
endif()
