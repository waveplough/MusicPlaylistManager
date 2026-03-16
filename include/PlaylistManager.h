#pragma once
#include "MusicLibrary.h"
#include <memory>
#include <vector>
class PlaylistManager
{
public:
	PlaylistManager(MusicLibrary& lib);
	~PlaylistManager();
	bool addSongToPlaylist(const std::string& songID, std::string& playlistID);
	bool removeSongFromPlayList(const std::string& songID, std::string& playlistID);
	void getSongStats(const std::string& songID) const;
private:
	MusicLibrary& library;
};

