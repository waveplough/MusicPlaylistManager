#include "MusicLibrary.h"
#include <algorithm>

/**
 * Default Constructor
 * Initializes an empty music library
 */
MusicLibrary::MusicLibrary() {}

/**
 * Destructor
 * Cleans up all songs and playlists
 *
 * Since songs are stored as shared_ptr and playlists as unique_ptr,
 * memory is automatically freed when the vectors are destroyed.
 */
MusicLibrary::~MusicLibrary() {}

/**
 * Adds a song to the library if it doesn't already exist
 *
 * @param song Shared pointer to the Song object to add
 */
void MusicLibrary::addSong(const std::shared_ptr<Song>& song) {
    if (!song) return;
    for (const auto& existingSong : songs) {
        if (existingSong->getItemID() == song->getItemID()) {
            return;
        }
    }

    songs.push_back(song);
}

/**
 * Updates an existing song's metadata
 *
 * @param songID Unique identifier of the song to edit
 * @param updated Song object containing the new metadata
 */
void MusicLibrary::editSong(const std::string& songID, const Song& updated) {
    for (auto& existingSong : songs) {
        if (existingSong->getItemID() == songID) {
            *existingSong = updated;   // updates the same shared object
            return;
        }
    }
}

/**
 * Removes a song from the library and all playlists that contain it
 *
 * @param songID Unique identifier of the song to delete
 */
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

/**
 * Creates a new playlist in the library
 *
 * @param playlistID Unique identifier for the new playlist
 * @param name Display name of the playlist
 */
void MusicLibrary::createPlaylist(const std::string& playlistID, const std::string& name) {
    for (const auto& playlist : playlists) {
        if (playlist->getPlaylistID() == playlistID) {
            return;
        }
    }
    playlists.push_back(std::make_unique<Playlist>(playlistID, name));
}

/**
 * Deletes a playlist from the library
 *
 * @param playlistID Unique identifier of the playlist to delete
 */
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

/**
 * Finds and returns a playlist by its ID
 *
 * @param playlistID Unique identifier of the playlist to find
 * @return Raw pointer to the Playlist, or nullptr if not found
 */
Playlist* MusicLibrary::findPlaylist(const std::string& playlistID) {   // Takes in only a playlist ID. Could later be adjusted for name too.
    for (auto it = playlists.begin(); it != playlists.end(); ++it) {        // Iterator loop
        if ((*it)->getPlaylistID() == playlistID) {
            return it->get();                                           // No need to dereference it because its reutrning the pointer
        }
    }
    return nullptr;
}

/**
 * Searches for songs matching a query string across multiple fields
 *
 * @param query Search string entered by the user
 * @return Vector of shared_ptr to songs that match the query
 */
std::vector<std::shared_ptr<Song>> MusicLibrary::searchSongs(const std::string& query) const {
    std::vector<std::shared_ptr<Song>> results;

    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(),
        [](unsigned char c) { return std::tolower(c); });

    for (const auto& song : songs) {
        std::string title = song->getTitle();
        std::string artist = song->getArtist();
        std::string album = song->getAlbum();
        std::string genre = song->getGenre();

		std::transform(title.begin(), title.end(), title.begin(),                   // Convert to lowercase for case-insensitive search
			[](unsigned char c) { return std::tolower(c); });                       // Lambda function to convert characters to lowercase
        std::transform(artist.begin(), artist.end(), artist.begin(),
            [](unsigned char c) { return std::tolower(c); });
        std::transform(album.begin(), album.end(), album.begin(),
            [](unsigned char c) { return std::tolower(c); });
        std::transform(genre.begin(), genre.end(), genre.begin(),
            [](unsigned char c) { return std::tolower(c); });

        if (title.find(lowerQuery) != std::string::npos ||
            artist.find(lowerQuery) != std::string::npos ||
            album.find(lowerQuery) != std::string::npos ||
            genre.find(lowerQuery) != std::string::npos ||
            song->getItemID().find(query) != std::string::npos) {
            results.push_back(song);
        }
    }

    return results;
}

/**
 * Finds a song by its file system path
 *
 * @param path Absolute or relative file path to search for
 * @return Shared pointer to the Song, or nullptr if not found
 */
std::shared_ptr<Song> MusicLibrary::findSongByPath(const std::string& path) {
    for (const auto& songListIt : songs) {            // the prebuilt list
        if (songListIt && songListIt->getFilePath() == path) {
            return songListIt;
        }   // null if no match. Consider boolean.
    }
    return nullptr;
}
