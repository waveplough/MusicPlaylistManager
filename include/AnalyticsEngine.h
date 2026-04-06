#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <numeric>
#include "Song.h"

/**
* 
 * Template class for calculating music statistics.
 * defaults to Song
 */
template<typename T = Song>
class AnalyticsEngine {
public:
    // Stores a reference to the library's song vector (to avoid unnecessary copying)
    AnalyticsEngine(const std::vector<std::shared_ptr<T>>& songData)
        : songs(songData) {
    }

    /**
     * Sums the duration of all songs multiplied by their play counts.
     * Total listening time's in seconds.
     */
    int computeTotalListeningTime() const {
        return std::accumulate(songs.begin(), songs.end(), 0,
            [](int total, const std::shared_ptr<T>& song) {
                return total + (song->getDuration() * song->getPlayCount());
            });
    }

    /**
     * Sorts songs by playCount and returns the top results.
     */
    std::vector<std::shared_ptr<T>> computeMostPlayedSongs(int limit = 10) const {
        std::vector<std::shared_ptr<T>> sortedSongs = songs;

        std::sort(sortedSongs.begin(), sortedSongs.end(),
            [](const std::shared_ptr<T>& a, const std::shared_ptr<T>& b) {
                return a->getPlayCount() > b->getPlayCount();
            });

        if (sortedSongs.size() > static_cast<size_t>(limit)) {
            sortedSongs.resize(limit);
        }
        return sortedSongs;
    }

    /**
     * Calculates the average duration of a song in the library.
     */
    double computeAverageSongDuration() const {
        if (songs.empty()) return 0.0;

        double totalDuration = std::accumulate(songs.begin(), songs.end(), 0.0,
            [](double total, const std::shared_ptr<T>& song) {
                return total + song->getDuration();
            });

        return totalDuration / songs.size();
    }

    /**
     * Groups songs by their genre string.
     * A map where key is genre and value is count of songs.
     */
    std::unordered_map<std::string, int> computeSongsByGenre() const {
        std::unordered_map<std::string, int> genreMap;
        for (const auto& song : songs) {
            genreMap[song->getGenre()]++;
        }
        return genreMap;
    }

private:
    const std::vector<std::shared_ptr<T>>& songs;
};