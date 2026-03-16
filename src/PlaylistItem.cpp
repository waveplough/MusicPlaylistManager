#include "PlaylistItem.h"
#include <string>

PlaylistItem::PlaylistItem() : itemID(""), title(""), duration(0) {							// Initializing attributes with default values
}

PlaylistItem::PlaylistItem(const std::string id, const std::string& title, int duration)	// Parameterized constructor
	:	itemID(id),
		title(title),
		duration(duration)
{
}

PlaylistItem::~PlaylistItem() {																// Destructor called 
}

void PlaylistItem::displayItem() {															// Displays the details of the playlist item
	printf("Duration: %d\n",
		   "ItemID: %s\n",
		   "Title: %s\n", duration, itemID.c_str(), title.c_str());	
}

