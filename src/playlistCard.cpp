#include <QtCore>
#include <QtMultimedia>
#include <QtWidgets>
#include <qpainter.h>
#include "playlistCard.h"

playlistCard::playlistCard(std::unique_ptr<Playlist>& playlist, QWidget *parent)
	: QWidget(parent)
    , playlist(playlist.get())
	, ui(new Ui::playlistCardClass())
{
	ui->setupUi(this);

}

playlistCard::~playlistCard()
{
	delete ui;
}
