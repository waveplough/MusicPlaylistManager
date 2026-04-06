#pragma once

#ifndef SONGCARD_H
#define SONGCARD_H

#include <QWidget>
#include "ui_songCard.h"
#include "Song.h"

QT_BEGIN_NAMESPACE
namespace Ui { class songCard; };
QT_END_NAMESPACE

class songCard : public QWidget
{
	Q_OBJECT

public:
	explicit songCard(std::shared_ptr<Song> song, QWidget *parent = nullptr);
	~songCard();

private:
	Ui::songCard *ui;
	std::shared_ptr<Song> song;

protected:
	void mouseDoubleClickEvent(QMouseEvent* playSong) override;		
		// The defauly mouseDoubleClick needs to be overridden here to have these arguments allowed.

signals:
	void songCardDoubleClick(std::shared_ptr<Song> song);
		// Sends the song object outbound. That's how signals work.
};


#endif // SONGCARD_H