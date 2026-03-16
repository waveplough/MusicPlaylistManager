#include "Song.h"

Song::Song() : artist(""), album(""), genre(""), playCount(0) {}	// Default constructor

Song::~Song() {}													// Destructor


void Song::displayItem() {											// Displays item
	PlaylistItem::displayItem();
	printf("Artist: %s\nAlbum: %s\nGenre: %s\nPlay Count: %d\n",
		artist.c_str(), album.c_str(), genre.c_str(), playCount);
}

void Song::incrementPlayCount() {									// Increments the play count
	playCount++;
}	
bool Song::operator==(const Song& other) const {
	return itemID == other.itemID;  // Compare by unique ID
}

std::ostream& operator<<(std::ostream& os, const Song& song) {
	os << "Song: " << song.title
		<< " | " << song.artist
		<< " | " << song.duration << "s";
	return os;
}
