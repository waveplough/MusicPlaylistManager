#pragma once
#include <QtMultimedia>
#include <QtWidgets>

#include "ui_LibraryCard.h"
#include "Song.h"

QT_BEGIN_NAMESPACE
namespace Ui { class LibraryCard; };
QT_END_NAMESPACE

class LibraryCard : public QWidget
{
	Q_OBJECT

public:
	explicit LibraryCard(std::shared_ptr<Song> song, QWidget* parent = nullptr);
	~LibraryCard();

signals:
	void libraryCardDoubleClick(std::shared_ptr<Song> song);

protected:
	void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
	Ui::LibraryCard* ui;
	std::shared_ptr<Song> song;
};