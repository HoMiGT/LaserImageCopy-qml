target_sources(${CMAKE_PROJECT_NAME} PUBLIC
    "main.cpp"
    "backend.h"
    "backend.cpp"
    "foldermodel.cpp"
    "foldermodel.h"
)

qt_add_resources(${CMAKE_PROJECT_NAME} "setting"
    PREFIX "/App/"
    FILES
        setting.ini
)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::Qml
    Qt${QT_VERSION_MAJOR}::Concurrent
)

target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    $<$<CONFIG:Release>:QT_NO_DEBUG_OUTPUT>
)