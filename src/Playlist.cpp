#include "Playlist.h"

Playlist::Playlist() :  playlistID(std::to_string(nextId++)),                   // Default constructor
                        name("Default"), 
                        songs{ std::make_shared<Song>() } {}

Playlist::Playlist(const std::string& id, const std::string& name)              // Parameterized constructor
                    :   playlistID(id), 
                        name(name) {}

int Playlist::computeTotalDuration() const {                                    // Sums the total duration
    return std::accumulate(songs.begin(), songs.end(), 0,
        [](int total, const std::shared_ptr<Song>& song) {
            return total + song->getDuration();
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

void Playlist::removeSong(const std::string& songID) {                              // Finds and deletes a song
    for (auto s = songs.begin(); s != songs.end(); s++) {
        if ((*s)->getItemID() == songID) {
            songs.erase(s);
        }
    }
}

void Playlist::reorderSong(size_t oldIndex, size_t newIndex) {                      // Reorder song

    if (oldIndex == newIndex) { return; }

    // Swap first

    if (oldIndex < newIndex) {
        int lower = oldIndex;
        std::shared_ptr<Song> lowerSong = songs[lower];
        int higher = newIndex;
        int i = 0;

        for (i = lower; i < higher; i++) {   // Bubbles up
            songs[i] = songs[i + 1];
        }
        songs[i] = lowerSong;
    }
    else
    {
        int lower = newIndex;
        int higher = oldIndex;
        std::shared_ptr<Song> higherSong = songs[higher];
        int i = 0;

        for (i = higher; i > lower; i--) {   // Bubbles down
            songs[i] = songs[i - 1];
        }
        songs[i] = higherSong;
    }

}
