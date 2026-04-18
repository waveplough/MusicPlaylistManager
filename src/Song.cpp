#include "Song.h"
#include "Playlist.h"
#include <cstdio>

/**
 * Creates an empty Song with default values
 */
Song::Song() : artist(""), album(""), genre(""), playCount(0), filePath("") {}								// Default constructor

/**
 * Creates a Song with full metadata
 *
 * @param id Unique identifier for the song
 * @param title Title of the song
 * @param dur Duration in seconds
 * @param artist Name of the performing artist
 * @param album Album name the song belongs to
 * @param genre Music category (Rock, Jazz, Pop, etc.)
 * @param filePath Absolute file path to the audio file on disk
 *
 * Initializes base class PlaylistItem first, then other members.
 */
Song::Song(const std::string& id, const std::string& title, int dur, const std::string& artist	// Parameterized constructor
	,const std::string& album, const std::string& genre, const std::string& filePath)

	:	PlaylistItem(id,title,dur),
		artist(artist),
		album(album),
		genre(genre),
		playCount(0),
		filePath(filePath)
{}

/**
 * Cleans up any resources
 */
Song::~Song() {}																				// Destructor

/**
 * Displays all song information to the console
 */
void Song::displayItem() {																		// Displays item
	PlaylistItem::displayItem();
	printf("Artist: %s\n",
			"Album: %s\n",
			"Genre: %s\n",
			"Play Count: %d\n",
			"Path: %s\n", artist.c_str(), album.c_str(), genre.c_str(), playCount, filePath.c_str());
}

/**
 * Increments the play count by 1
 */
void Song::incrementPlayCount() {																// Increments the play count
	playCount++;
}	

/**
 * Adds listening time to the song's total
 *
 * @param seconds Number of seconds to add (usually 1, called every second while playing)
 */
void Song::addListeningTime(int seconds) { 
	totalListeningTime += seconds; 
}

/**
 * Compares two songs by their unique ID
 *
 * @param other The song to compare against
 * @return true if both songs have the same itemID, false otherwise
 */
bool Song::operator==(const Song& other) const {
	return itemID == other.itemID;  // Compare by unique ID
}

/**
 * Outputs song information to an output stream
 *
 * @param os Output stream
 * @param song The song to output
 * @return Reference to the output stream for chaining
 */
std::ostream& operator<<(std::ostream& os, const Song& song) {
	os << "Song: " << song.title
		<< " | " << song.artist
		<< " | " << song.duration << "s"
		<< " | " << song.filePath;
	return os;
}

