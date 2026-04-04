#pragma once

#include <QObject>
#include <QMediaPlayer>

class MediaController  : public QObject
{
	Q_OBJECT

public:
	explicit MediaController(QObject *parent = nullptr);
	~MediaController();

	QMediaPlayer* usePlayer() const { return trueMediaPlayer; } // Const prevents manipulation

private:
	QMediaPlayer* trueMediaPlayer;
};

