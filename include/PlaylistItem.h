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
	const std::string& getItemID() const { return itemID; }
	const std::string& getTitle() const { return title;  }
	virtual void displayItem();			// Abstract function for displaying a Playlist Item
	int getDuration() const { return duration; }						// Returns Playlist Item duration

};
