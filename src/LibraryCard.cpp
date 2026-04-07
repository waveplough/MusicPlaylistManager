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
    
    // add fields and check add default text if empty
    std::string title = song ? song->getTitle() : "";
    ui->titleLabel->setText(QString::fromStdString(title.empty() ? "No Title" : title));

    std::string artist = song ? song->getArtist() : "";
    ui->artistLabel->setText(QString::fromStdString(artist.empty() ? "Unknown Artist" : artist));

    std::string album = song ? song->getAlbum() : "";
    ui->albumLabel->setText(QString::fromStdString(album.empty() ? "N/A" : album));

    int duration = song ? song->getDuration() : 0;
    int minutes = duration / 60;
    int seconds = duration % 60;
    QString durationStr = QString("%1:%2")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));

    ui->durationLabel->setText(durationStr);
}

LibraryCard::~LibraryCard()
{
    delete ui;
}

// This function is called when the user double-clicks on the library card. 
// It emits a signal with the associated song, which can be connected to a slot in the main window to play the song.
void LibraryCard::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (song)
    {
        emit libraryCardDoubleClick(song);
    }

    QWidget::mouseDoubleClickEvent(event);
}

