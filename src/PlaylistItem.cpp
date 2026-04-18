#include "PlaylistItem.h"
#include <string>

/**
 * Initializes a playlist item with default values
 * 
 * Used when creating a placeholder or when specific values aren't available yet
 */
PlaylistItem::PlaylistItem() : itemID(""), title(""), duration(0) {							
}

/**
 * Creates a playlist item with specified values
 *
 * @param id Unique identifier for the playlist item
 * @param title Display title of the item 
 * @param duration Length of the item in seconds
 *
 * Used when creating concrete items like Song objects with actual metadata
 */
PlaylistItem::PlaylistItem(const std::string id, const std::string& title, int duration)	// Parameterized constructor
	:	itemID(id),
		title(title),
		duration(duration)
{
}

/**
 * Ensures proper cleanup of derived classes
 */
PlaylistItem::~PlaylistItem() {																// Destructor called 
}

/**
 * Displays the playlist item's details to the console
 */
void PlaylistItem::displayItem() {															// Displays the details of the playlist item
	printf("Duration: %d\n",
		   "ItemID: %s\n",
		   "Title: %s\n", duration, itemID.c_str(), title.c_str());	
}

