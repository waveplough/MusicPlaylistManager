#include "PlaylistItem.h"

PlaylistItem::PlaylistItem() : itemID(""), title(""), duration(0) {		// initializing attributes with default values
}

PlaylistItem::~PlaylistItem() {											// destructor called 
}

void PlaylistItem::displayItem() {										// displays the details of the playlist item
	printf("Duration: %d\n",
		   "ItemID: %s\n",
		   "Title: %s\n", duration, itemID.c_str(), title.c_str());	
}

int PlaylistItem::getDuration() {										// returns the duration of the playlist item
	return duration;
}