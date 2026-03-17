#include "MusicLibrary.h"

void MusicLibrary::addSong(const std::shared_ptr<Song>& song) {
    if (!song) return;
    for (const auto& existingSong : songs) {
        if (existingSong->getItemID() == song->getItemID()) {
            return;
        }
    }

    songs.push_back(song);
}

void MusicLibrary::deleteSong(const std::string& songID) {
    // Remove from the library using an iterator
    for (auto it = songs.begin(); it != songs.end(); ) {
        if ((*it)->getItemID() == songID) {
            it = songs.erase(it);
        }
        else {
            ++it; 
        }
    }
    // Remove it from all playlists
    for (size_t i = 0; i < playlists.size(); ++i) {
        playlists[i]->removeSong(songID);
    }
}

void MusicLibrary::createPlaylist(const std::string& playlistID, const std::string& name) {
    playlists.push_back(std::make_unique<Playlist>(playlistID, name));
}

void MusicLibrary::deletePlaylist(const std::string& playlistID) {
    for (auto it = playlists.begin(); it != playlists.end(); ) {
        if ((*it)->getPlaylistID() == playlistID) {
            it = playlists.erase(it);
        }
        else {
            ++it;
        }
    }
}