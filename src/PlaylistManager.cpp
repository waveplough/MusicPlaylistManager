#include "PlaylistManager.h"
#include "Playlist.h"

/**
 * Initializes PlaylistManager with a reference to the MusicLibrary
 *
 * @param lib Shared pointer to the MusicLibrary containing all songs and playlists
 */
PlaylistManager::PlaylistManager(std::shared_ptr<MusicLibrary>& lib)
    : library(lib)
{
}

/**
 * Cleans up any resources
 */
PlaylistManager::~PlaylistManager() {
}

/**
 * Adds a song to a specified playlist
 *
 * @param songID Unique identifier of the song to add
 * @param playlistID Unique identifier of the target playlist
 * @return true if the song was successfully added, false otherwise
 */
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

/**
 * Removes a song from a specified playlist
 *
 * @param songID Unique identifier of the song to remove
 * @param playlistID Unique identifier of the playlist to remove from
 * @return true if the playlist was found and removal attempted, false otherwise
 */
bool PlaylistManager::removeSongFromPlayList(const std::string& songID,const std::string& playlistID) {
    for (const auto& p : library->getPlaylists()) {
        if (p->getPlaylistID() == playlistID) {
            p->removeSong(songID);
            return true;
        }
    }
    return false;
}

/**
 * Displays statistics for a specific song
 *
 * @param songID Unique identifier of the song to query
 */
void PlaylistManager::getSongStats(const std::string& songID) const {
    for (const auto& s : library->getSongs()) {
        if (s->getItemID() == songID) {
            std::cout << "Song found: " << s->getTitle() << std::endl;
            return;
        }
    }
    std::cout << "Song not found." << std::endl;
}