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
	// Constructors
	PlaylistItem();																		// PlayList Item default constructor
	PlaylistItem(const std::string id, const std::string& title, int duration);			// Parameterized constructor

	virtual ~PlaylistItem();															// Playlist Item destructor

	// Getters
	const std::string& getItemID() const { return itemID; }
	const std::string& getTitle() const { return title;  }
	int getDuration() const { return duration; }										// Returns Playlist Item duration

	virtual void displayItem() = 0;														// Abstract function for displaying a Playlist Item
	

};
