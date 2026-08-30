cmake_minimum_required(VERSION 3.5)

project(TextRecognizer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS Widgets Sql Network REQUIRED Concurrent)


qt_standard_project_setup()

qt_add_executable(TextRecognizer WIN32
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui

    database.h
    database.cpp

    registerwindow.h
    registerwindow.cpp
    registerwindow.ui

    loginwindow.h
    loginwindow.cpp
    loginwindow.ui

    historywindow.h
    historywindow.cpp
    historywindow.ui


    aboutwindow.h
    aboutwindow.cpp
    aboutwindow.ui

    triallwindow.h
    triallwindow.cpp
    triallwindow.ui

    welcomwindow.h welcomwindow.cpp welcomwindow.ui

    profilewindow.h profilewindow.cpp profilewindow.ui

    style.qss
    imagelabel.h imagelabel.cpp
    animatedbackground.h animatedbackground.cpp
    avatarcropdialog.h avatarcropdialog.cpp
    GlassSplashScreen.h GlassSplashScreen.cpp


)

qt_add_resources(TextRecognizer_resources
    resources2.qrc
)

target_sources(TextRecognizer PRIVATE
    ${TextRecognizer_resources}
)



target_link_libraries(TextRecognizer PRIVATE Qt6::Widgets Qt6::Sql Qt6::Network Qt6::Concurrent)

