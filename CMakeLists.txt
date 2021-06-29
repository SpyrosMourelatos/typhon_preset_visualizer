cmake_minimum_required(VERSION 3.5)

project(Presets_Comparer LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)
find_package(protoc REQUIRED CONFIG)
find_package(Protobuf REQUIRED )

add_executable(Presets_Comparer MACOSX_BUNDLE 
  src/main.cpp
  src/utilities.cpp
  src/preset_viewer.cpp
  include/preset_viewer/preset_viewer.h
  src/preset_viewer.ui
)
target_include_directories(${CMAKE_PROJECT_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/preset_viewer
)
protobuf_generate(TARGET Presets_Comparer PROTOS Resources/preset.proto)
set_property(SOURCE ${CMAKE_CURRENT_BINARY_DIR}/Resources/preset.pb.h PROPERTY SKIP_AUTOGEN ON)
set_property(SOURCE ${CMAKE_CURRENT_BINARY_DIR}/Resources/preset.pb.cc PROPERTY SKIP_AUTOGEN ON)
target_link_libraries(Presets_Comparer 
  PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
      protobuf::libprotobuf

)

#cmake -B build -DCMAKE_TOOLCHAIN_FILE=build/conan_paths.cmake

install(
    TARGETS ${CMAKE_PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_PROJECT_NAME}
    COMPONENT ${CMAKE_PROJECT_NAME}_installer
    BUNDLE DESTINATION  "."
    COMPONENT ${CMAKE_PROJECT_NAME}_installer
)

if (APPLE)
    
    add_custom_target(package_mac 
        COMMAND   macdeployqt ${CMAKE_CURRENT_BINARY_DIR}/Presets_Comparer.app/ -always-overwrite  
        COMMAND   create-dmg ${CMAKE_CURRENT_BINARY_DIR}/Presets_Comparer.app --overwrite || true   
        )
    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "Presets_Comparer")
    set(CPACK_PACKAGE_NAME Presets_Comparer)
    set(CPACK_PACKAGE_VENDOR "DREADBOX")
    set(CPACK_PACKAGE_FILE_NAME installer)
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Installation Tool")
    set(CPACK_PACKAGE_VERSION "2.0.0") # Version of installer
    set(CPACK_DMG_VOLUME_NAME "Installer for Presets_Comparer " )
    set(CPACK_GENERATOR "DragNDrop")
    set(myApp_ICON ${CMAKE_CURRENT_SOURCE_DIR}/installer/icon.icns)
    set(MACOSX_BUNDLE_ICON_FILE myApp_ICON)
    set_source_files_properties(${myApp_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    set_target_properties(Presets_Comparer PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_BINARY_DIR}/../Info.plist)
    include (CPACK)

elseif (WIN32)
    function(CPACKIFW_FUNC)
        set(CPACK_PACKAGE_NAME "Presets_Comparer")
        set(CPACK_PACKAGE_VENDOR "DREADBOX")
        set(CPACK_PACKAGE_FILE_NAME installer)
        set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Installation Tool")
        set(CPACK_PACKAGE_VERSION ".1") # Version of installer
        set(CPACK_COMPONENTS_ALL Presets_Comparer_installer)
        set(CPACK_GENERATOR IFW)
        set(CPACK_IFW_PRODUCT_URL "https://www.dreadbox-fx.com/wp-content/uploads/2020/07/Typhon-Synthesizer_Users-Manual.pdf")
        set(CPACK_IFW_VERBOSE ON)
        set(CPACK_IFW_PACKAGE_ICON ../Resources/menubarlogo.ico)
        include(CPack REQUIRED)
        include(CPackIFW REQUIRED)

        cpack_ifw_configure_component(
            #FORCED_INSTALLATION
            #NAME qt.cpackifw.installer
            Presets_Comparer_installer
        )
    endfunction()
    add_custom_command(
            TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/windeployqt_stuff
            COMMAND windeployqt.exe --compiler-runtime --dir ${CMAKE_BINARY_DIR}/windeployqt_stuff $<TARGET_FILE:Presets_Comparer>
        )
    install(
        DIRECTORY ${CMAKE_BINARY_DIR}/windeployqt_stuff/
        DESTINATION ${PROJECT_NAME}
        COMPONENT Presets_Comparer_installer
    )
    CPACKIFW_FUNC()
endif()