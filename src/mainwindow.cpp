#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "MediaController.h"
#include "Song.h"
#include "LibraryCard.h"
#include <QMediaMetaData>
#include <QUuid>

MainWindow::MainWindow(MediaController &mediaControl, MusicLibrary& library, QWidget *parent)
    : QMainWindow(parent)
    , mediaControl(mediaControl)
    , musicLibrary(library)
    , ui(new Ui::MainWindow)

    
{
    ui->setupUi(this);

    // Forces the main splitter to size. Not doable in create.
    ui->mainSplitter->setSizes({ 551, 240, 240 });

    // Slots and signals manual connections
    connect(ui->actionNewSong, &QAction::triggered, this, &MainWindow::onActionNewSongTriggered);
    
    connect(ui->playerVolumeButton, &QPushButton::clicked, this, &MainWindow::onPlayerVolumeButtonClicked);
    connect(ui->backButton, &QPushButton::clicked, this, &MainWindow::onBackButtonClicked);
    connect(ui->stopButton, &QPushButton::clicked, this, &MainWindow::onStopButtonClicked);
    connect(ui->playButton, &QPushButton::clicked, this, &MainWindow::onPlayButtonClicked);
    connect(ui->pauseButton, &QPushButton::clicked, this, &MainWindow::onPauseButtonClicked);
    connect(ui->forwardButton, &QPushButton::clicked, this, &MainWindow::onForwardButtonClicked);
    
    connect(ui->playerPlaybar, &QSlider::valueChanged, this, &MainWindow::onPlayerPlaybarValueChanged);
    connect(ui->playerVolumeSlider, &QSlider::valueChanged, this, &MainWindow::onPlayerVolumeSliderValueChanged);

    /* Object Manipulation */

    // Playbar Slider
    connect(mediaControl.usePlayer(), &QMediaPlayer::durationChanged, this, &MainWindow::durationChanged);  // Sets maximum slider count.
    connect(mediaControl.usePlayer(), &QMediaPlayer::positionChanged, this, &MainWindow::positionChanged);  // Keeps track of a moving slider.
    connect(mediaControl.usePlayer(), &QMediaPlayer::positionChanged, this, &MainWindow::updateDuration);   // Updates time labels while playing.
    //ui->playerPlaybar->setRange(0, mediaControl.usePlayer()->duration() / 1000);

    // Volume Slider
    ui->playerVolumeSlider->setMinimum(1);
    ui->playerVolumeSlider->setMaximum(100);
    ui->playerVolumeSlider->setValue(30);
    mediaControl.usePlayer()->audioOutput()->setVolume(ui->playerVolumeSlider->value());     // Sets slider volume. May be unneccessary.

}

MainWindow::~MainWindow()
{
    delete ui;
}


// Major Function : Clicking in the top menu bar 'File' to add a Song to the Library.
void MainWindow::onActionNewSongTriggered() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Audio File"), "", tr("MP# Files (*.mp3)"));    // Gives the menu more verbiage.
    mediaControl.usePlayer()->setSource(QUrl(fileName));   
    QFileInfo fileinfo(fileName); // This is getting file info OUTSIDE of the song library. This is important here. Don't change.
    ui->playerFilePathLabel->setText(fileinfo.absoluteFilePath());  // Sets the filepath label
    // For those reading:
        // mediaControl is the instantiated MedialController.cpp class.
        // trueMediaPlayer is instantiated within that object as part of its creation.
        // usePlayer() is a function that returns a pointer to that media player.
        // setSource() replaces setMedia() from QT 5. It is what loads the song.

//-(*)- Suleiman, Ria. This is where a song should now get registered in the library.

/* DAVIDS FUNCTION */

    if (!fileName.isEmpty()) {
        mediaControl.usePlayer()->setSource(QUrl::fromLocalFile(fileName));

        QEventLoop loop;
        connect(mediaControl.usePlayer(), &QMediaPlayer::mediaStatusChanged, &loop, [&loop](QMediaPlayer::MediaStatus status) {
            if (status == QMediaPlayer::LoadedMedia) {
                loop.quit();
            }
            });
        loop.exec();

        QMediaMetaData metaData = mediaControl.usePlayer()->metaData();

        // library information
        std::string title = metaData.stringValue(QMediaMetaData::Title).toStdString();
        std::string artist = metaData.stringValue(QMediaMetaData::AlbumArtist).toStdString();
        std::string album = metaData.stringValue(QMediaMetaData::AlbumTitle).toStdString();
        std::string genre = metaData.stringValue(QMediaMetaData::Genre).toStdString();
        int duration = mediaControl.usePlayer()->duration() / 1000;


        std::string songId = generateSongID();  // Generate a song ID

        if (artist.empty()) {
            artist = "Unknown Artist";
        }

        if (title.empty()) {
            title = QFileInfo(fileName).baseName().toStdString();
        }

        if (album.empty()) {
            album = "N/A";
        }

        ui->playerTitleLabel->setText(QString::fromStdString(title));
        ui->playerArtistLabel->setText(QString::fromStdString(artist));

        std::shared_ptr<Song> newSong = std::make_shared<Song>(Song(songId, title, duration, artist, album, genre));

        musicLibrary.addSong(std::shared_ptr<Song>(newSong));
        addSongCardToLibraryList(newSong);

    }


}

void MainWindow::addSongCardToLibraryList(std::shared_ptr<Song> song) {
    LibraryCard* card = new LibraryCard(song, this);
    QListWidgetItem* item = new QListWidgetItem();
    item->setSizeHint(card->sizeHint());
    ui->libraryList->addItem(item);
    ui->libraryList->setItemWidget(item, card);
}




std::string generateSongID() {
    return QUuid::createUuid().toString().toStdString();
/* END DAVIDS FUNCTION */

    
}

/* Music Player Functions */

// Music Player : Scroll back button functionality.
void MainWindow::onBackButtonClicked() 
{

}

// Music Player : Stop button functionality.
void MainWindow::onStopButtonClicked() 
{
    mediaControl.usePlayer()->stop();
}

// Music Player : Play button functionality.
void MainWindow::onPlayButtonClicked() 
{
    mediaControl.usePlayer()->play();
}

// Music Player : Pause button functionality.
void MainWindow::onPauseButtonClicked() 
{
    mediaControl.usePlayer()->pause();
}

// Music Player : Scroll forward button functionality.
void MainWindow::onForwardButtonClicked() 
{

}

    // Playbar related //
// Music Player : Playbar (slider) manipulation.
void MainWindow::onPlayerPlaybarValueChanged(int value)
{

}

// Music Player : Playbar duration sync
void MainWindow::updateDuration(qint64 duration)
{
    QString timestr;
    if (duration || mDuration)
    {
        // subdivide in hours, minutes, seconds and millises
        QTime currentTime(
            (duration / 3600000) % 24,
            (duration / 60000) % 60,         
            (duration / 1000) % 60,          
            duration % 1000                  
        );
        QTime totalTime(
            (mDuration / 3600000) % 24,
            (mDuration / 60000) % 60,
            (mDuration / 1000) % 60,
            mDuration % 1000
        );

        QString format = "mm:ss";
        if (mDuration > 3600000) {  
            format = "hh:mm:ss";
        }
        ui->playerCurrentTime->setText(currentTime.toString(format));
        ui->playerTimeEnd->setText(totalTime.toString(format));
    }
}

// Music Player : Playbar duration sync
void MainWindow::durationChanged(qint64 duration)
{
    mDuration = duration; 
    ui->playerPlaybar->setMaximum(mDuration / 1000); 
}

// Music Player : Playbar position change
void MainWindow::positionChanged(qint64 progress)
{
    if (!ui->playerPlaybar->isSliderDown()) 
    {
        ui->playerPlaybar->setValue(progress / 1000);
    }
}

    // Volume Bar Related //
// Music Player : Volume Bar (slider, not button) manipulation.
void MainWindow::onPlayerVolumeSliderValueChanged(int value)
{
    mediaControl.usePlayer()->audioOutput()->setVolume(value / 100.00);      // Pay attention to this syntax.
        // QT 6 uses volume levels from 0 - 1. Not 100 like previously.
}

// Adds toggled Mute / Active functionality for the Player's Volume (mute) button.
void MainWindow::onPlayerVolumeButtonClicked()
{
    if (isMuted == false)
    {
        mediaControl.usePlayer()->audioOutput()->setMuted(true);
        ui->playerVolumeButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
        isMuted = true;
    }
    else
    {
        mediaControl.usePlayer()->audioOutput()->setMuted(false);
        ui->playerVolumeButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
        isMuted = false;
    }
}