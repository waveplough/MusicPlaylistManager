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

void Playlist::addSong(const std::shared_ptr<Song>& song) {
    // if song already exists (checks by the ID)
    for (const auto& existing : songs) {
        if (existing->getItemID() == song->getItemID()) {
            return; // already in playlist
        }
    }
    songs.push_back(song);
}
