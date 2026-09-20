cmake_minimum_required(VERSION 3.16)
project(OfficeExtensionManager VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6 or fallback to Qt5
find_package(Qt6 COMPONENTS Widgets Core Gui Xml REQUIRED)

# Source files
set(SOURCES
    src/main.cpp
    src/core/PlatformService.cpp
    src/core/MacPlatformService.cpp
    src/core/WindowsPlatformService.cpp
    src/core/ZipHandler.cpp
    src/core/ExtensionManager.cpp
    src/ribbon/RibbonModel.cpp
    src/ribbon/RibbonXmlParser.cpp
    src/ribbon/RibbonXmlGenerator.cpp
    src/ui/MainWindow.cpp
    src/ui/AllRibbonTabsWidget.cpp
    src/ui/WordTabWidget.cpp
    src/ui/PowerPointTabWidget.cpp
    src/ui/RibbonEditorWidget.cpp
    src/ui/XmlHighlighter.cpp
    src/ui/StyleHelper.cpp
)

set(HEADERS
    src/core/PlatformService.h
    src/core/MacPlatformService.h
    src/core/WindowsPlatformService.h
    src/core/ExtensionItem.h
    src/core/ZipHandler.h
    src/core/ExtensionManager.h
    src/ribbon/RibbonModel.h
    src/ribbon/RibbonXmlParser.h
    src/ribbon/RibbonXmlGenerator.h
    src/ui/MainWindow.h
    src/ui/AllRibbonTabsWidget.h
    src/ui/WordTabWidget.h
    src/ui/PowerPointTabWidget.h
    src/ui/RibbonEditorWidget.h
    src/ui/XmlHighlighter.h
    src/ui/StyleHelper.h
)

add_executable(OfficeExtensionManager
    ${SOURCES}
    ${HEADERS}
    resources.qrc
)

target_include_directories(OfficeExtensionManager PRIVATE
    src
    src/core
    src/ribbon
    src/ui
)

target_link_libraries(OfficeExtensionManager PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    Qt6::Xml
)

if(APPLE)
    set_source_files_properties(assets/AppIcon.icns PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources")
    target_sources(OfficeExtensionManager PRIVATE assets/AppIcon.icns)

    set_target_properties(OfficeExtensionManager PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME "Office Ribbon Manager"
        MACOSX_BUNDLE_DISPLAY_NAME "Office Extension & Ribbon Manager"
        MACOSX_BUNDLE_ICON_FILE "AppIcon"
        MACOSX_BUNDLE_INFO_STRING "Office Extension & Ribbon Manager for Word & PowerPoint"
        MACOSX_BUNDLE_LONG_VERSION_STRING "1.0.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.khmeroffice.extensionmanager"
    )
endif()

# Unit tests
add_executable(test_ribbon
    test/test_ribbon.cpp
    src/core/PlatformService.cpp
    src/core/MacPlatformService.cpp
    src/core/WindowsPlatformService.cpp
    src/core/ZipHandler.cpp
    src/core/ExtensionManager.cpp
    src/ribbon/RibbonModel.cpp
    src/ribbon/RibbonXmlParser.cpp
    src/ribbon/RibbonXmlGenerator.cpp
)

target_include_directories(test_ribbon PRIVATE
    src
    src/core
    src/ribbon
)

target_link_libraries(test_ribbon PRIVATE
    Qt6::Core
    Qt6::Xml
)
