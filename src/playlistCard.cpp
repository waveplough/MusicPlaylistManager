#include <QtCore>
#include <QtMultimedia>
#include <QtWidgets>
#include <qpainter.h>
#include "playlistCard.h"

/**
 * Formats a duration in seconds into a human-readable string
 *
 * @param totalSeconds Duration in seconds to format
 * @return Formatted string with leading zeros
 */
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

/**
 * Creates a visual card widget representing a playlist
 *
 * @param playlist Pointer to the Playlist object this card represents
 * @param parent Parent widget (usually the QListWidget containing this card)d
 */
playlistCard::playlistCard(Playlist* playlist, QWidget* parent)
    : QWidget(parent)
    , playlist(playlist)
    , ui(new Ui::playlistCardClass())
{
    ui->setupUi(this);

    if (playlist) {
        ui->playlistNameLabel->setText(QString::fromStdString(playlist->getName()));
        ui->totalPlaylistTime->setText(formatDuration(playlist->computeTotalDuration()));
    }

    connect(ui->deletePlaylistButton, &QPushButton::clicked, this, [this]()
        {
            if (this->playlist)
            {
                emit deletePlaylistRequested(QString::fromStdString(this->playlist->getPlaylistID()));
            }
        });
}

/**
 * Cleans up UI components
 */
playlistCard::~playlistCard()
{
	delete ui;
}
