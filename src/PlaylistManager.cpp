#include "PlaylistManager.h"
#include "Playlist.h"
bool PlaylistManager::addSongToPlaylist(const std::string& songID, std::string& playlistID) {
    std::shared_ptr<Song> targetedSong = nullptr;

    for (const auto& s : library.getSongs()) {
        if (s->getItemID() == songID) {
            targetedSong = s;
            break;
        }
    }
    Playlist* targetPlaylist = nullptr;
    for (const auto& p : library.getPlaylists()) {
        if (p->getPlaylistID() == playlistID) {
            targetPlaylist = p.get();
            break;
        }
    }
    // if both exist, add the song to the playlist
    if (targetedSong && targetPlaylist) {
        targetPlaylist->addSong(targetedSong);
        return true;
    }

    return false;
}

bool PlaylistManager::removeSongFromPlayList(const std::string& songID, std::string& playlistID) {
    for (const auto& p : library.getPlaylists()) {
        if (p->getPlaylistID() == playlistID) {
            p->removeSong(songID);
            return true;
        }
    }
    return false;
}