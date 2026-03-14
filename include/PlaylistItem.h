#pragma once

#include <string>

class PlaylistItem {

protected:
	std::string itemID;						// Unique identifier for the item, 
	std::string title;						// Title of the media item
	int duration;							// Duration in seconds

public:
	PlaylistItem();
	virtual ~PlaylistItem();

	virtual void displayItem() = 0;
	int getDuration();

};
