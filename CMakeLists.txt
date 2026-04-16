cmake_minimum_required(VERSION 3.16)

project("MusicPlaylistManager")

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Multimedia)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

#set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_SOURCE_DIR}/src")

add_executable(MusicPlaylistManager
    "src/main.cpp"
    "src/PlaylistManager.cpp"
    
    "src/DataManager.cpp"
    "src/MusicLibrary.cpp"
    "src/Playlist.cpp"
    "src/PlaylistItem.cpp"
    "src/Song.cpp"
    "src/MainWindow.cpp"
    "src/mainwindow.ui"
    "src/MediaController.cpp"
    "src/SongCard.cpp"
    "src/PlaylistCard.cpp"
    "src/LibraryCard.cpp"
    "include/PlaylistCard.h"
    "include/SongCard.h"
    "include/MediaController.h"
    "include/MainWindow.h"
    "include/playlistCard.ui"
    "include/LibraryCard.h"
    "include/songCard.ui"
    "include/LibraryCard.ui" "include/Util.h")
target_include_directories(MusicPlaylistManager PRIVATE include)
target_link_libraries(MusicPlaylistManager PRIVATE Qt6::Widgets)
target_link_libraries(MusicPlaylistManager PRIVATE Qt6::Multimedia)