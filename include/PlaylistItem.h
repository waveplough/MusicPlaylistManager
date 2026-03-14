#pragma once

#include <string>
#include <cstdio>
#include <iostream>

class PlaylistItem {

protected:
	std::string itemID;						// Unique identifier for the item, 
	std::string title;						// Title of the media item
	int duration;							// Duration in seconds

public:
	PlaylistItem();							// PlayList Item default constructor
	virtual ~PlaylistItem();				// Playlist Item destructor

	virtual void displayItem() = 0;			// Abstract function for displaying a Playlist Item
	int getDuration();						// Returns Playlist Item duration

};
