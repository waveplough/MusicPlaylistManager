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
        AnalyticsEngine(const std::vector<std::shared_ptr<T>>& songData);

        int computeTotalListeningTime() const;
        std::vector<std::shared_ptr<T>> computeMostPlayedSongs(int limit = 10) const;
        double computeAverageSongDuration() const;
        std::unordered_map<std::string, int> computeSongsByGenre() const;

    private:
        const std::vector<std::shared_ptr<T>>& songs;
    };