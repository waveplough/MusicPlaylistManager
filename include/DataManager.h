#pragma once
#include "MusicLibrary.h"
#include <string>
class DataManager
{
public:
	DataManager(MusicLibrary& library);
	~DataManager();
	bool saveData(const std::string& filename) const;
	bool loadData(const std::string& filename);

private:
	MusicLibrary& library;
};

