#include "PlaylistItem.h"

PlaylistItem::PlaylistItem() : itemID(""), title(""), duration(0) {		// Initializing attributes with default values
}

PlaylistItem::~PlaylistItem() {											// Destructor called 
}

void PlaylistItem::displayItem() {										// Displays the details of the playlist item
	printf("Duration: %d\n",
		   "ItemID: %s\n",
		   "Title: %s\n", duration, itemID.c_str(), title.c_str());	
}

int PlaylistItem::getDuration() {										// Returns the duration of the playlist item
	return duration;
}