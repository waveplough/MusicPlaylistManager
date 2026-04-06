#pragma once

#include "MediaController.h"
#include "MusicLibrary.h"
#include "Song.h"
#include <string>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QMediaMetaData>
#include <QMediaPlayer>
#include <QUuid>
#include <QFileDialog>
#include "Util.h"

class DataManager
{
public:
	DataManager(std::shared_ptr<MusicLibrary>& lib);                     // Constructor
	~DataManager();                                         // Destructor

	bool saveData(const std::string& filename) const;       // Save Data To File
	bool loadData(const std::string& filename);             // Load Data From File
	std::shared_ptr<Song> parseSongData(const QString& filename, QMediaPlayer& player);	// parses the meta data of newly added songs


private:
	std::shared_ptr<MusicLibrary> musicLibrary;
};