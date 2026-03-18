#pragma once
#include <Song.h>
#include <Playlist.h>
#include <vector>
#include <memory>
class MusicLibrary
{
private:
	std::vector<std::shared_ptr<Song>> songs;
	std::vector<std::unique_ptr<Playlist>> playlists;

public:
	MusicLibrary(); //default constructor
	~MusicLibrary();
	void addSong(const std::shared_ptr<Song>& song);
	void editSong(const std::string& songID, const Song& updated);
	void deleteSong(const std::string& songID);
	void createPlaylist(const std::string& playlistID, const std::string& name);
	void deletePlaylist(const std::string& playlistID);
	std::vector<std::shared_ptr<Song>> searchSongs(const std::string& query) const;

	//getters
	const std::vector<std::shared_ptr<Song>>& getSongs() const { return songs; }
	const std::vector<std::unique_ptr<Playlist>>& getPlaylists() const { return playlists; }

};

