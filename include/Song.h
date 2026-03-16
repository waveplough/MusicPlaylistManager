#pragma once
#include "PlaylistItem.h"
#include <string>

class Song : public PlaylistItem {
private:
	std::string artist;		// Artist name
	std::string album;		// Album name
	std::string genre;		// Genre name
	int playCount{ 0 };			// Number of plays

public:
	Song();							// Default constructor
	Song(const std::string& id, const std::string& title, int dur, const std::string& artist
		, const std::string& album, const std::string& genre);
	virtual ~Song();				// Destructor
	virtual void displayItem();		// Outputs item
	void incrementPlayCount();		// Increments the play count
	//getters
	const std::string& getArtist() const { return artist; }
	const std::string& getAlbum() const { return album;  }
	const std::string& getGenre() const { return genre;  }
	int getPlayCount() const { return playCount; }
	//friend function + as overloading
	bool operator==(const Song& other) const;
	friend std::ostream& operator<<(std::ostream& os, const Song& song);
};

