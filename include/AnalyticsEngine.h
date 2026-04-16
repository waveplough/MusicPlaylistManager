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
     * Sums the listening time of all songs
     * Total listening time's in seconds.
     */
    int computeTotalListeningTime() const {
        return std::accumulate(songs.begin(), songs.end(), 0,
            [](int total, const std::shared_ptr<T>& song) {
                return total + song->getTotalListeningTime();  
            });
    }

    /**
     * Sorts songs by playCount and returns the top results.
     * Only includes songs with valid duration.
     */
    std::vector<std::shared_ptr<T>> computeMostPlayedSongs(int limit = 10) const {
        std::vector<std::shared_ptr<T>> validSongs;
        for (const auto& song : songs) {
            if (song->getDuration() > 0) {
                validSongs.push_back(song);
            }
        }

        std::sort(validSongs.begin(), validSongs.end(),
            [](const std::shared_ptr<T>& a, const std::shared_ptr<T>& b) {
                return a->getPlayCount() > b->getPlayCount();
            });

        if (validSongs.size() > static_cast<size_t>(limit)) {
            validSongs.resize(limit);
        }
        return validSongs;
    }

    /**
     * Calculates the average duration of a song in the library.
     */
    double computeAverageSongDuration() const {
        double totalDuration = 0.0;
        int validCount = 0;

        for (const auto& song : songs) {
            if (song->getDuration() > 0) {
                totalDuration += song->getDuration();
                validCount++;
            }
        }

        return validCount > 0 ? totalDuration / validCount : 0.0;
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