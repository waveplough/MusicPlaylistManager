#include "Song.h"
#include "Playlist.h"
#include <cstdio>

Song::Song() : artist(""), album(""), genre(""), playCount(0), filePath("") {}								// Default constructor

Song::Song(const std::string& id, const std::string& title, int dur, const std::string& artist	// Parameterized constructor
	,const std::string& album, const std::string& genre, const std::string& filePath)

	:	PlaylistItem(id,title,dur),
		artist(artist),
		album(album),
		genre(genre),
		playCount(0),
		filePath(filePath)
{}

Song::~Song() {}																				// Destructor


void Song::displayItem() {																		// Displays item
	PlaylistItem::displayItem();
	printf("Artist: %s\n",
			"Album: %s\n",
			"Genre: %s\n",
			"Play Count: %d\n",
			"Path: %s\n", artist.c_str(), album.c_str(), genre.c_str(), playCount, filePath.c_str());
}

void Song::incrementPlayCount() {																// Increments the play count
	playCount++;
}	

bool Song::operator==(const Song& other) const {
	return itemID == other.itemID;  // Compare by unique ID
}

std::ostream& operator<<(std::ostream& os, const Song& song) {
	os << "Song: " << song.title
		<< " | " << song.artist
		<< " | " << song.duration << "s"
		<< " | " << song.filePath;
	return os;
}

