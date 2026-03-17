#pragma once

#include "MusicLibrary.h"
#include <string>

class DataManager
{
public:
	DataManager(MusicLibrary& library);                     // Constructor
	~DataManager();                                         // Destructor

	bool saveData(const std::string& filename) const;       // Save Data To File
	bool loadData(const std::string& filename);             // Load Data From File

private:
	MusicLibrary& library;                                  // Reference To Music Library
};