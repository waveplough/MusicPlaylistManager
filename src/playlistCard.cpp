#include <QtCore>
#include <QtMultimedia>
#include <QtWidgets>
#include <qpainter.h>
#include "playlistCard.h"

playlistCard::playlistCard(Playlist* playlist, QWidget *parent)
	: QWidget(parent)
    , playlist(playlist)
	, ui(new Ui::playlistCardClass())
{
	ui->setupUi(this);

}

playlistCard::~playlistCard()
{
	delete ui;
}
