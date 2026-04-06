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

private:
	QMediaPlayer* trueMediaPlayer;
	QAudioOutput* audioOutput;
};

