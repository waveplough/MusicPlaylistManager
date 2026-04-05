#pragma once
#include "Song.h"
#include "PlaylistItem.h"
#include "Util.h"
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <numeric>
#include <unordered_map>

class Playlist
{
private:
	
	std::string playlistID;
	std::string name;
	std::vector<std::shared_ptr<Song>> songs;

public:
	// Constructors
	Playlist();																		// Default constructor
	Playlist(const std::string& id, const std::string& name);						// Parameterized constructor
	~Playlist() {};																	// Destructor

	// Getters
	const std::string& getPlaylistID() const { return playlistID; }
	const std::string& getName() const { return name;  }
	const std::vector<std::shared_ptr<Song>>& getSongs() const{ return songs;  }

	// Utility
	void addSong(const std::shared_ptr<Song>& song);
	void removeSong(const std::string& songID);
	void reorderSong(size_t oldIndex, size_t newIndex);
	int computeTotalDuration() const;
	void displayPlaylist() const;

};

