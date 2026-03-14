cmake_minimum_required(VERSION 3.16)

project("MusicPlaylistManager")

find_package(Qt6 REQUIRED COMPONENTS Widgets)
add_executable(MusicPlaylistManager
    "src/main.cpp"
    "src/PlaylistManager.cpp"
    "src/GUI.cpp"
    "src/DataManager.cpp"
    "src/MusicLibrary.cpp"
    "src/Playlist.cpp"
    "src/PlaylistItem.cpp"
    "src/Song.cpp"
)
target_include_directories(MusicPlaylistManager PRIVATE include)
target_link_libraries(MusicPlaylistManager PRIVATE Qt6::Widgets)