#include "Song.h"

Song::Song() : artist(""), album(""), genre(""), playCount(0) {}	// Default constructor

Song::~Song() {}													// Destructor


void Song::displayItem() {											// Displays item
	PlaylistItem::displayItem();
	printf("Artist: %s\n",
		"Album: %s\n",
		"Genre: %s\n",
		"Play Count: %d", artist.c_str(), album.c_str(), genre.c_str(), playCount);
}							

void Song::incrementPlayCount() {									// Increments the play count
	playCount++;
}									
