#pragma once

#ifndef PLAYLISTCARD_H
#define PLAYLISTCARD_H

#include <QWidget>
#include "ui_playlistCard.h"
#include "Playlist.h"

QT_BEGIN_NAMESPACE
namespace Ui { class playlistCardClass; };
QT_END_NAMESPACE

class playlistCard : public QWidget
{
	Q_OBJECT

public:
	playlistCard(std::unique_ptr<Playlist> playlist, QWidget *parent = nullptr);
	~playlistCard();

private:
	Ui::playlistCardClass *ui;

	QPixmap Translucency(QImage image);
};

#endif // PLAYLISTCARD_H
