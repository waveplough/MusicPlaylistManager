#pragma once
#include "Song.h"
#include "PlaylistItem.h"
#include <string>
#include <vector>
#include <memory>
class Playlist
{
private:
	std::string playlistID;
	std::string name;
	std::vector<std::shared_ptr<Song>> songs;

public:
	Playlist();
	Playlist(const std::string& id, const std::string& n);
	~Playlist();
	const std::string& getPlaylistID() const { return playlistID; }
	const std::string& getName() const { return name;  }
	const std::vector<std::shared_ptr<Song>>& getSongs() const{ return songs;  }
	void addSong(const std::shared_ptr<Song>& song);
	void removeSong(const std::string& songID);
	void reorderSong(size_t oldIndex, size_t newIndex);
	int computeTotalDuration() const;
	void displayPlaylist() const;

};

