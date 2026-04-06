#pragma once
#include "MusicLibrary.h"
#include <memory>
#include <vector>
class PlaylistManager
{
public:
	PlaylistManager(std::shared_ptr<MusicLibrary>& lib);
	~PlaylistManager();
	bool addSongToPlaylist(const std::string& songID, const std::string& playlistID);
	bool removeSongFromPlayList(const std::string& songID,const std::string& playlistID);
	void getSongStats(const std::string& songID) const;
	std::shared_ptr<MusicLibrary> getMusicLibrary() { return library; }

private:
	std::shared_ptr<MusicLibrary> library;
};

