#include "Playlist.h"
#include <algorithm>

Playlist::Playlist() :  playlistID(generateID()),                               // Default constructor
                        name("Default"), 
                        songs{} {}

Playlist::Playlist(const std::string& id, const std::string& name)              // Parameterized constructor
                    :   playlistID(id), 
                        name(name) {}

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

void Playlist::displayPlaylist() const {
  
    std::cout << "Playlist: " << name << " (ID: " << playlistID << ")\n";
    std::cout << "Total duration: " << computeTotalDuration() << " seconds\n";
    std::cout << "Songs (" << songs.size() << "):\n";
    for (const auto& song : songs) {
        song->displayItem();
    }
}

void Playlist::addSong(const std::shared_ptr<Song>& song) {                     // Adds a song to the playlist
    // if song already exists (checks by the ID)
    for (const auto& existing : songs) {
        if (existing->getItemID() == song->getItemID()) {
            return; // already in playlist
        }
    }
    songs.push_back(song);
}

void Playlist::removeSong(const std::string& songID) {                          // Finds and deletes a song
    for (auto s = songs.begin(); s != songs.end(); s++) {
        if ((*s)->getItemID() == songID) {
            songs.erase(s);
            return;
        }
    }
}

// This function moves a song from one index to another, shifting the other songs accordingly.
void Playlist::reorderSong(size_t oldIndex, size_t newIndex)                       // Moves a song from one index to another, shifting the other songs accordingly
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





