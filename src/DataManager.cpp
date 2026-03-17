#include "DataManager.h"
#include <fstream>
#include <iostream>
#include <sstream>

DataManager::DataManager(MusicLibrary& library) : library(library)			// Initialize Reference To Library
{
}

DataManager::~DataManager()													// Destructor
{
}

bool DataManager::saveData(const std::string& filename) const
{
	std::ofstream outFile(filename);										// Open File For Writing

	if (!outFile)
	{
		std::cerr << "Error: Could not open file for saving.\n";			// Print Error If File Fails
		return false;
	}

	outFile << "SONGS\n";													// Mark Start Of Songs Section
	for (const auto& song : library.getSongs())
	{
		outFile << song->getItemID() << ","
			<< song->getTitle() << ","
			<< song->getDuration() << ","
			<< song->getArtist() << ","
			<< song->getAlbum() << ","
			<< song->getGenre() << ","
			<< song->getPlayCount() << "\n";								// Write Song Data
	}

	outFile << "PLAYLISTS\n";												// Mark Start Of Playlists Section
	for (const auto& playlist : library.getPlaylists())
	{
		outFile << playlist->getPlaylistID() << ","
			<< playlist->getName() << "\n";									// Write Playlist Data
	}

	outFile.close();														// Close File
	return true;
}

bool DataManager::loadData(const std::string& filename)
{
	std::ifstream inFile(filename);											// Open File For Reading

	if (!inFile)
	{
		std::cerr << "Error: Could not open file for loading.\n";			// Print Error If File Fails
		return false;
	}

	std::string line;
	bool readingSongs = false;												// Flag For Songs Section
	bool readingPlaylists = false;											// Flag For Playlists Section

	while (std::getline(inFile, line))
	{
		if (line == "SONGS")
		{
			readingSongs = true;											// Switch To Reading Songs
			readingPlaylists = false;
			continue;
		}

		if (line == "PLAYLISTS")
		{
			readingSongs = false;
			readingPlaylists = true;										// Switch To Reading Playlists
			continue;
		}

		if (readingSongs)
		{
			std::stringstream ss(line);										// Create Stream From Line
			std::string id, title, durationStr, artist, album, genre, playCountStr;

			std::getline(ss, id, ',');
			std::getline(ss, title, ',');
			std::getline(ss, durationStr, ',');
			std::getline(ss, artist, ',');
			std::getline(ss, album, ',');
			std::getline(ss, genre, ',');
			std::getline(ss, playCountStr, ',');

			if (!id.empty() && !title.empty() && !durationStr.empty())
			{
				int duration = std::stoi(durationStr);						// Convert Duration To Int

				std::shared_ptr<Song> song = std::make_shared<Song>(
					id, title, duration, artist, album, genre
				);															// Create Song Object

				library.addSong(song);										// Add Song To Library
			}
		}

		if (readingPlaylists)
		{
			std::stringstream ss(line);										// Create Stream From Line
			std::string playlistID, name;

			std::getline(ss, playlistID, ',');
			std::getline(ss, name, ',');

			if (!playlistID.empty() && !name.empty())
			{
				library.createPlaylist(playlistID, name);					// Create Playlist
			}
		}
	}

	inFile.close();															// Close File
	return true;
}