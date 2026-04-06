#include "PlaylistManager.h"
#include "Playlist.h"

PlaylistManager::PlaylistManager(std::shared_ptr<MusicLibrary>& lib)
    : library(lib)
{
}

PlaylistManager::~PlaylistManager() {
}

bool PlaylistManager::addSongToPlaylist(const std::string& songID,const std::string& playlistID) {
    std::shared_ptr<Song> targetedSong = nullptr;

    for (const auto& s : library->getSongs()) {
        if (s->getItemID() == songID) {
            targetedSong = s;
            break;
        }
    }
    Playlist* targetPlaylist = nullptr;
    for (const auto& p : library->getPlaylists()) {
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

bool PlaylistManager::removeSongFromPlayList(const std::string& songID,const std::string& playlistID) {
    for (const auto& p : library->getPlaylists()) {
        if (p->getPlaylistID() == playlistID) {
            p->removeSong(songID);
            return true;
        }
    }
    return false;
}

void PlaylistManager::getSongStats(const std::string& songID) const {
    for (const auto& s : library->getSongs()) {
        if (s->getItemID() == songID) {
            std::cout << "Song found: " << s->getTitle() << std::endl;
            return;
        }
    }
    std::cout << "Song not found." << std::endl;
}