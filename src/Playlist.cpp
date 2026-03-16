#include "Playlist.h"
#include <iostream>
#include <numeric>

int Playlist::computeTotalDuration() const {
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