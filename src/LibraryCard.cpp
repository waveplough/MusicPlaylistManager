#include <QtCore>
#include <QtMultimedia>
#include <QtWidgets>
#include "LibraryCard.h"


LibraryCard::LibraryCard(std::shared_ptr<Song> song, QWidget* parent)
    : QWidget(parent)
    , song(song)
    , ui(new Ui::LibraryCard())
{
    ui->setupUi(this);
    
    ui->titleLabel->setText(QString::fromStdString(song->getTitle()));
    ui->artistLabel->setText(QString::fromStdString(song->getArtist()));
    ui->albumLabel->setText(QString::fromStdString(song->getAlbum()));

    // format duration 
    int minutes = song->getDuration() / 60;
    int seconds = song->getDuration() % 60;
    QString durationStr = QString("%1:%2")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));

    ui->durationLabel->setText(durationStr);
}

LibraryCard::~LibraryCard()
{
    delete ui;
}

