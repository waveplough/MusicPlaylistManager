#include "Song.h"
#include "Playlist.h"

Song::Song() : artist(""), album(""), genre(""), playCount(0) {}								// Default constructor

Song::Song(const std::string& id, const std::string& title, int dur, const std::string& artist	// Parameterized constructor
	,const std::string& album, const std::string& genre)

	:	PlaylistItem(id,title,dur),
		artist(artist),
		album(album),
		genre(genre)
{}

Song::~Song() {}																				// Destructor


void Song::displayItem() {																		// Displays item
	PlaylistItem::displayItem();
	printf("Artist: %s\n",
			"Album: %s\n",
			"Genre: %s\n",
			"Play Count: %d\n", artist.c_str(), album.c_str(), genre.c_str(), playCount);
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
		<< " | " << song.duration << "s";
	return os;
}
void Playlist::addSong(const std::shared_ptr<Song>& song) {
	// if song already exists (checks by the ID)
	for (const auto& existing : songs) {
		if (existing->getItemID() == song->getItemID()) {
			return; // already in playlist
		}
	}
	songs.push_back(song);
}
