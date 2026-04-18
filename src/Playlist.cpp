#include "Playlist.h"
#include <algorithm>

/**
 * Creates a playlist with a generated ID and default name
 */
Playlist::Playlist() :  playlistID(generateID()),                               // Default constructor
                        name("Default"), 
                        songs{} {}

/**
 * Creates a playlist with specified ID and name
 *
 * @param id Unique identifier for the playlist (must be unique across all playlists)
 * @param name Display name of the playlist (user-facing)
 */
Playlist::Playlist(const std::string& id, const std::string& name)              // Parameterized constructor
                    :   playlistID(id), 
                        name(name) {}

/**
 * Computes the total duration of all songs in the playlist
 *
 * @return Total duration in seconds, or 0 if playlist is empty
 */
int Playlist::computeTotalDuration() const {                                    // Sums the total duration
        return std::accumulate(songs.begin(), songs.end(), 0,
            [](int total, const std::shared_ptr<Song>& song) {                  
                if (!song) {                                                    // Suleiman, check this logic. Updated for .m3u
                    return total;
                }
                else {
                    auto d = song->getDuration();
                    if (d == UNKNOWN_DUR) {
                        d = 0;
                    }
                    return d + total;
                }
                // Old Logic // return song ? total + song->getDuration() : total;              
            });
}

/**
 * Displays all playlist information to the console
 */
void Playlist::displayPlaylist() const {
  
    std::cout << "Playlist: " << name << " (ID: " << playlistID << ")\n";
    std::cout << "Total duration: " << computeTotalDuration() << " seconds\n";
    std::cout << "Songs (" << songs.size() << "):\n";
    for (const auto& song : songs) {
        song->displayItem();
    }
}

/**
 * Adds a song to the playlist
 *
 * @param song Shared pointer to the Song object to add
 */
void Playlist::addSong(const std::shared_ptr<Song>& song) {                
    // if song already exists (checks by the ID)
    for (const auto& existing : songs) {
        if (existing->getItemID() == song->getItemID()) {
            return; // already in playlist
        }
    }
    songs.push_back(song);
}

/**
 * Removes a song from the playlist by its ID
 *
 * @param songID Unique identifier of the song to remove
 */
void Playlist::removeSong(const std::string& songID) {                          
    for (auto s = songs.begin(); s != songs.end(); s++) {
        if ((*s)->getItemID() == songID) {
            songs.erase(s);
            return;
        }
    }
}

/**
 * Moves a song from one index to another within the playlist
 *
 * @param oldIndex Current position of the song
 * @param newIndex Desired new position 
 */
void Playlist::reorderSong(size_t oldIndex, size_t newIndex)                       
{
    if (oldIndex >= songs.size() || newIndex >= songs.size()) {
        return;
    }

    if (oldIndex == newIndex) {
        return;
    }

    if (oldIndex < newIndex) {
        size_t lower = oldIndex;
        std::shared_ptr<Song> lowerSong = songs[lower];
        size_t higher = newIndex;
        size_t i = 0;

        for (i = lower; i < higher; i++) {
            songs[i] = songs[i + 1];
        }
        songs[i] = lowerSong;
    }
    else {
        size_t lower = newIndex;
        size_t higher = oldIndex;
        std::shared_ptr<Song> higherSong = songs[higher];
        size_t i = 0;

        for (i = higher; i > lower; i--) {
            songs[i] = songs[i - 1];
        }
        songs[i] = higherSong;
    }
}
//*******************************************************************************************************//
// Sorting the playlist//
//*******************************************************************************************************//

/**
 * Sorts the playlist songs alphabetically by artist name
 */
void Playlist::sortByArtist()                                                   
{
    std::sort(songs.begin(), songs.end(),
        [](const std::shared_ptr<Song>& a, const std::shared_ptr<Song>& b) {
            if (!a || !b) {
                return a != nullptr;
            }
            return a->getArtist() < b->getArtist();
        });
}

/**
 * Sorts the playlist songs alphabetically by genre
 */
void Playlist::sortByGenre()                                                   
{
    std::sort(songs.begin(), songs.end(),
        [](const std::shared_ptr<Song>& a, const std::shared_ptr<Song>& b) {
            if (!a || !b) {
                return a != nullptr;
            }
            return a->getGenre() < b->getGenre();
        });
}

/**
 * Sorts the playlist songs alphabetically by album name
 */
void Playlist::sortByAlbum()                                                   
{
    std::sort(songs.begin(), songs.end(),
        [](const std::shared_ptr<Song>& a, const std::shared_ptr<Song>& b) {
            if (!a || !b) {
                return a != nullptr;
            }
            return a->getAlbum() < b->getAlbum();
        });
}




