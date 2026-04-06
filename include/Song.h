#pragma once
#include "PlaylistItem.h"
#include <string>
#include <ostream>

class Song : public PlaylistItem {
private:
	std::string artist;			// Artist name
	std::string album;			// Album name
	std::string genre;			// Genre name
	int playCount{ 0 };			// Number of plays
	std::string filePath;       // Full audio file path

public:
	// Constructors
	Song();																						// Default constructor
	Song(const std::string& id, const std::string& title, int dur, const std::string& artist
		,const std::string& album, const std::string& genre, const std::string& filePath = "");

	// Destructor
	virtual ~Song();																			// Destructor

	// Methods
	virtual void displayItem();																	// Outputs item
	void incrementPlayCount();																	// Increments the play count

	// Getters
	const std::string& getArtist() const { return artist; }
	const std::string& getAlbum() const { return album;  }
	const std::string& getGenre() const { return genre;  }
	int getPlayCount() const { return playCount; }
	const std::string& getFilePath() const { return filePath; }

	// Setters
	void setArtist(const std::string& a) { artist = a; }
	void setAlbum(const std::string& a) { album = a; }
	void setGenre(const std::string& g) { genre = g; }
	void setPlayCount(int count) { playCount = count; }
	void setFilePath(const std::string& path) { filePath = path; }

	// Friend function + as overloading
	bool operator==(const Song& other) const;
	friend std::ostream& operator<<(std::ostream& os, const Song& song);
};

