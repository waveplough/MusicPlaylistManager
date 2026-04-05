#pragma once

#include "MusicLibrary.h"
#include "MediaController.h"
#include <string>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QMediaMetaData>
#include <QEventLoop>
#include <QMediaPlayer>
#include <QUuid>
#include <QFileDialog>

class DataManager
{
public:
	DataManager(MusicLibrary& library);                     // Constructor
	~DataManager();                                         // Destructor

	bool saveData(const std::string& filename) const;       // Save Data To File
	bool loadData(const std::string& filename);             // Load Data From File
	std::shared_ptr<Song> parseSongData(const QString& filename, QMediaPlayer& player);	// parses the meta data of newly added songs
	MusicLibrary& getMusicLibrary() { return library; }
	std::string generateSongID();   // Generates a song ID

private:
	MusicLibrary& library;                                  // Reference To Music Library
};