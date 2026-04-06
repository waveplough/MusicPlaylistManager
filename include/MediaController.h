#pragma once

#include <QObject>
#include <QAudioOutput>
#include <QMediaPlayer>
#include "Song.h"

class MediaController  : public QObject
{
	Q_OBJECT

public:
	explicit MediaController(QObject *parent = nullptr);
	~MediaController();

	QMediaPlayer* usePlayer() const { return trueMediaPlayer; } // Const prevents manipulation
	void setCurrentSong(std::shared_ptr<Song> song) { this->currentSong = song; }

private:
	QMediaPlayer* trueMediaPlayer;
	QAudioOutput* audioOutput;
	std::shared_ptr<Song> currentSong;
};

