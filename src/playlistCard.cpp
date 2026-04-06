#include <QtCore>
#include <QtMultimedia>
#include <QtWidgets>
#include <qpainter.h>
#include "playlistCard.h"

static QString formatDuration(int totalSeconds)
{
    int hours = totalSeconds / 3600;
    int minutes = (totalSeconds % 3600) / 60;
    int seconds = totalSeconds % 60;

    return QString("%1:%2:%3")
        .arg(hours, 2, 10, QChar('0'))
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
}

playlistCard::playlistCard(Playlist* playlist, QWidget *parent)
	: QWidget(parent)
    , playlist(playlist)
	, ui(new Ui::playlistCardClass())
{
	ui->setupUi(this);
    if (playlist) {
        ui->playlistNoticeLabel->setText(QString::fromStdString(playlist->getName()));
        ui->totalPlaylistTime->setText(formatDuration(playlist->computeTotalDuration()));
    }
}

playlistCard::~playlistCard()
{
	delete ui;
}
