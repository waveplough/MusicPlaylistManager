#pragma once
#include "PlaylistItem.h"
#include <iostream>
class Song : public PlaylistItem {
private:
	std::string artist;		// Artist name
	std::string album;		// Album name
	std::string genre;		// Genre name
	int playCount;			// Number of plays

public:
	Song();							// Default constructor
	virtual ~Song();				// Destructor
	virtual void displayItem();		// Outputs item
	void incrementPlayCount();		// Increments the play count
};

