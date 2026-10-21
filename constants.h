#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QtCore/QString>

#ifdef BROWSER_VERSION
#define BROWSER_VERSION_TEXT BROWSER_VERSION
#else
#define BROWSER_VERSION_TEXT "2.0.0"
#endif

namespace AppConstants {

constexpr const char *APP_NAME            = "Browser";
constexpr const char *APP_DISPLAY_NAME    = "Yaron's Browser";
constexpr const char *APP_ORGANIZATION    = "Yaron Koresh";
constexpr const char *APP_DESCRIPTION     = "A modern web browser built with Qt.";
constexpr const char *APP_VERSION         = BROWSER_VERSION_TEXT;

constexpr const char *DEFAULT_HOME_URL    = "https://www.google.com/";
constexpr const char *DEFAULT_SEARCH_URL  = "https://www.google.com/search?q=%1";
constexpr const char *DEFAULT_THEME       = "system";

constexpr int DEFAULT_WINDOW_WIDTH        = 1024;
constexpr int DEFAULT_WINDOW_HEIGHT       = 768;
constexpr int SOURCE_DIALOG_WIDTH         = 800;
constexpr int SOURCE_DIALOG_HEIGHT        = 600;
constexpr int DOWNLOAD_DIALOG_WIDTH       = 400;
constexpr int DOWNLOAD_DIALOG_HEIGHT      = 300;

constexpr int STATUS_TIMEOUT_SHORT        = 2000;
constexpr int STATUS_TIMEOUT_LONG         = 5000;
constexpr int PROGRESS_UI_THROTTLE_MS     = 250;

constexpr int MAX_HISTORY_SIZE            = 1000;
constexpr int MAX_CONCURRENT_DOWNLOADS    = 3;

constexpr qint64 BYTES_PER_KB            = 1024;
constexpr qint64 BYTES_PER_MB            = 1024 * 1024;
constexpr qint64 BYTES_PER_GB            = 1024LL * 1024 * 1024;

constexpr const char *KEY_HOME_PAGE              = "General/HomePage";
constexpr const char *KEY_WINDOW_SIZE            = "General/WindowSize";
constexpr const char *KEY_DOWNLOAD_DIR           = "Downloads/DefaultDirectory";
constexpr const char *KEY_MAX_CONCURRENT_DL      = "Downloads/MaxConcurrent";
constexpr const char *KEY_MAX_HISTORY            = "History/MaxSize";
constexpr const char *KEY_JS_ENABLED             = "Browsing/JavaScriptEnabled";
constexpr const char *KEY_SEARCH_ENGINE          = "Browsing/SearchEngine";
constexpr const char *KEY_WINDOW_GEOMETRY        = "General/WindowGeometry";
constexpr const char *KEY_WINDOW_STATE           = "General/WindowState";
constexpr const char *KEY_LAST_SESSION_URLS      = "Session/LastUrls";
constexpr const char *KEY_ZOOM_FACTOR            = "Browsing/ZoomFactor";
constexpr const char *KEY_THEME                  = "General/Theme";

constexpr const char *LABEL_GO            = "Go";
constexpr const char *LABEL_BACK          = "Back";
constexpr const char *LABEL_FORWARD       = "Forward";
constexpr const char *LABEL_CANCEL        = "Cancel";
constexpr const char *LABEL_RETRY         = "Retry";
constexpr const char *LABEL_BROWSE        = "Browse...";
constexpr const char *LABEL_ACCEPT        = "Accept";
constexpr const char *LABEL_DOWNLOADS     = "Downloads";
constexpr const char *LABEL_REFRESH       = "Refresh";
constexpr const char *LABEL_VIEW_SOURCE   = "View Source";
constexpr const char *LABEL_ABOUT         = "About";
constexpr const char *LABEL_NEW_TAB       = "New Tab";
constexpr const char *LABEL_CLOSE_TAB     = "Close Tab";
constexpr const char *LABEL_FIND          = "Find...";
constexpr const char *LABEL_ZOOM_IN       = "Zoom In";
constexpr const char *LABEL_ZOOM_OUT      = "Zoom Out";
constexpr const char *LABEL_ZOOM_RESET    = "Reset Zoom";
constexpr const char *LABEL_FULLSCREEN    = "Fullscreen";
constexpr const char *LABEL_PRINT         = "Print";
constexpr const char *LABEL_SETTINGS      = "Settings";
constexpr const char *LABEL_BOOKMARKS     = "Bookmarks";

constexpr double ZOOM_STEP                = 0.1;
constexpr double ZOOM_MIN                 = 0.25;
constexpr double ZOOM_MAX                 = 5.0;
constexpr double ZOOM_DEFAULT             = 1.0;

}

#endif // CONSTANTS_H
