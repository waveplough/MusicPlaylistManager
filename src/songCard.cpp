#include <QtCore>
#include <QtMultimedia>
#include <QtWidgets>
#include <qpainter.h>
#include "songCard.h"

songCard::songCard(std::shared_ptr<Song> song, QWidget *parent)
	: QWidget(parent)
	, song(song)
	, ui(new Ui::songCard())
{
	ui->setupUi(this);

    ui->songNameLabel->setText(QString::fromStdString(song->getTitle()));
    ui->artistNameLabel->setText(QString::fromStdString(song->getArtist()));

    // format duration 
    int minutes = song->getDuration() / 60;
    int seconds = song->getDuration() % 60;
    QString durationStr = QString("%1:%2")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));

    ui->songDurationLabel->setText(durationStr);

    // Replace info for a bad file. If the file is bad, the card display will be different.
    bool exists = QFile::exists(QString::fromStdString(song->getFilePath()));               // QFile::exists is a built in method. This converts path to string and checks if it exists in the filesystem.

    if (!exists) {
        ui->songNameLabel->setStyleSheet("color: #9E9E9E;");                                // Sets red label colours
        ui->artistNameLabel->setStyleSheet("color: #9E9E9E;");
        ui->songDurationLabel->setStyleSheet("color: #9E9E9E;");
        ui->songDurationLabel->setText("??:??");
    }
}

songCard::~songCard()
{
	delete ui;
}

void songCard::mouseDoubleClickEvent(QMouseEvent *playSong)
{
    emit songCardDoubleClick(this->song);
    QWidget::mouseDoubleClickEvent(playSong);
}
