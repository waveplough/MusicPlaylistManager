#pragma once
class AnalyticsEngine
{
#include <vector>
#include <string>
#include <map>
#include <"Song.h>
    template<typename T = Song>
    class AnalyticsEngine {
    public:
        // Stores a reference to the song vector
        AnalyticsEngine(const std::vector<std::shared_ptr<T>>& songData)
            : songs(songData) {
        }

        int computeTotalListeningTime() const {
            int totalSeconds = 0;
        }

        std::vector<std::shared_ptr<T>> computeMostPlayedSongs(int limit = 10) const;
        double computeAverageSongDuration() const;
        std::unordered_map<std::string, int> computeSongsByGenre() const;

    private:
        const std::vector<std::shared_ptr<T>>& songs;
    };