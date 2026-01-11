cmake_minimum_required(VERSION 3.20)
project(TicTacToe LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt")
find_package(Qt6 REQUIRED COMPONENTS Widgets Core)

add_executable(game
    main.cpp

    mainwindow.cpp
    mainwindow.h

    widgets/boardwidget.cpp
    widgets/boardwidget.h

    statsdialog.cpp
    statsdialog.h

    ui/rulesdialog.cpp
    ui/rulesdialog.h

    game/game_types.h

    game/game_engine.cpp
    game/game_engine.h

    game/modes/igame_mode.h
    game/modes/classic_mode.cpp
    game/modes/classic_mode.h
    game/modes/score_mode.cpp
    game/modes/score_mode.h

    game/score/score_helpers.cpp
    game/score/score_helpers.h
)

target_include_directories(game PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(game PRIVATE Qt6::Widgets Qt6::Core)
