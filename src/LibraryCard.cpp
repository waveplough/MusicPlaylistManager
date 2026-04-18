#include <QtCore>
#include <QtMultimedia>
#include <QtWidgets>
#include "LibraryCard.h"

/**
 *  Creates a visual card widget representing a song in the library
 *
 * @param song Shared pointer to the Song object this card represents
 * @param parent Parent widget 
 */
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

    // Replace info for a bad file. If the file is bad, the card display will be different.
    bool exists = QFile::exists(QString::fromStdString(song->getFilePath()));               // QFile::exists is a built in method. This converts path to string and checks if it exists in the filesystem.

    if (!exists) {
        ui->titleLabel->setStyleSheet("color: #9E9E9E;");                                  // Sets grey label colours.
        ui->artistLabel->setStyleSheet("color: #9E9E9E;");
        ui->albumLabel->setStyleSheet("color: #9E9E9E;");
        ui->durationLabel->setStyleSheet("color: #9E9E9E;");
        ui->durationLabel->setText("??:??");
    }
}

/**
 * Cleans up the UI components
 */
LibraryCard::~LibraryCard()
{
    delete ui;
}

/**
 * This function is called when the user double - clicks on the library card.
 * It emits a signal with the associated song, which can be connected to a slot in the main window to play the song.
 * 
 * @param event Mouse Event that is transmitted when the card is clicked
 */
void LibraryCard::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (song)
    {
        emit libraryCardDoubleClick(song);
    }

    QWidget::mouseDoubleClickEvent(event);
}

