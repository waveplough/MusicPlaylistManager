#pragma once
#include "PlaylistItem.h"
#include <iostream>
class Song : public PlaylistItem {
private:
	std::string artist;		// artist name
	std::string album;		// album name
	std::string genre;		// genre name
	int playCount;			// number of plays

public:
	Song();							// default constructor
	virtual ~Song();				// destructor
	virtual void displayItem();		// outputs item
	void incrementPlayCount();		// increments the play count
};

