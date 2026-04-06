#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "MediaController.h"
#include "Song.h"
#include "LibraryCard.h"
#include "DataManager.h"
#include <QMediaMetaData>
#include <QUuid>
#include <QCloseEvent>
#include <QMessageBox>

MainWindow::MainWindow(MediaController &mediaControl, DataManager& dataManager, QWidget *parent)
    : QMainWindow(parent)
    , mediaControl(mediaControl)
    , dataManager(dataManager)
    , ui(new Ui::MainWindow)

    
{
    ui->setupUi(this);

    // Forces the main splitter to size. Not doable in create.
    ui->mainSplitter->setSizes({ 551, 240, 240 });
    ui->menuAnalytics->setFocusPolicy(Qt::NoFocus);

    // Slots and signals manual connections

    // Menu
    connect(ui->actionNewSong, &QAction::triggered, this, &MainWindow::onNewSongButtonClicked);
    connect(ui->actionViewAnalytics, &QAction::triggered, this, &MainWindow::onAnalyticsButtonClicked);

    // Playlist
    connect(ui->addPlaylistButton, &QPushButton::clicked, this, &MainWindow::onAddPlaylistButtonClicked);
    connect(ui->exitPlaylistEditor, &QPushButton::clicked, this, &MainWindow::onPlaylistEditorExitButtonClicked);
    connect(ui->savePlaylistChanges, &QPushButton::clicked, this, &MainWindow::onSavePlaylistButtonClicked);
	connect(ui->playlistCardBox, &QListWidget::currentRowChanged, this, &MainWindow::onPlaylistSelected);   //  This is for when a user selects a playlist from the playlist selection list. 
    connect(ui->addButton, &QPushButton::clicked, this, &MainWindow::onAddCurrentSongToPlaylistClicked);    //  This is for when a user clicks the "Add Current Song to Playlist" button. 
	connect(ui->playlistSongsList, &QListWidget::currentRowChanged, this, &MainWindow::onPlaylistEditorSongSelected);    // This is for when a user clicks on a song in the playlist editor. 
	connect(ui->SongList, &QListWidget::currentRowChanged, this, &MainWindow::onPlaylistSongSelected);       //  This is for when a user clicks on a song in the playlist view (the list of songs in the currently selected playlist).

    // Player
    connect(ui->playerVolumeButton, &QPushButton::clicked, this, &MainWindow::onPlayerVolumeButtonClicked);
    connect(ui->backButton, &QPushButton::clicked, this, &MainWindow::onBackButtonClicked);
    connect(ui->stopButton, &QPushButton::clicked, this, &MainWindow::onStopButtonClicked);
    connect(ui->playButton, &QPushButton::clicked, this, &MainWindow::onPlayButtonClicked);
    connect(ui->pauseButton, &QPushButton::clicked, this, &MainWindow::onPauseButtonClicked);
    connect(ui->forwardButton, &QPushButton::clicked, this, &MainWindow::onForwardButtonClicked);
    
    connect(ui->playerPlaybar, &QSlider::valueChanged, this, &MainWindow::onPlayerPlaybarValueChanged);
    connect(ui->playerVolumeSlider, &QSlider::valueChanged, this, &MainWindow::onPlayerVolumeSliderValueChanged);

    // Analytics
    connect(ui->exitButton, &QPushButton::clicked, this, &MainWindow::onAnalyticsExitButtonClicked);

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
    mediaControl.usePlayer()->audioOutput()->setVolume(ui->playerVolumeSlider->value() / 100.0);     // Sets slider volume. May be unneccessary.

}

MainWindow::~MainWindow()
{
    delete ui;
}

// Menu Item Listeners

// Major Function : Clicking in the top menu bar 'File' to add a Song to the Library.
    // For those reading:
        // mediaControl is the instantiated MedialController.cpp class.
        // trueMediaPlayer is instantiated within that object as part of its creation.
        // usePlayer() is a function that returns a pointer to that media player.
        // setSource() replaces setMedia() from QT 5. It is what loads the song.

    //-(*)- Suleiman, Ria. This is where a song should now get registered in the library.
void MainWindow::onNewSongButtonClicked() {

    QMediaPlayer* player = mediaControl.usePlayer();

    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Audio File"), "", tr("MP3 Files (*.mp3) *.wav *.flac *.aac *.ogg);;All Files (*)"));    // Gives the menu more verbiage.

    if (fileName.isEmpty()) return;

    player->setSource(QUrl::fromLocalFile(fileName));
    QFileInfo fileInfo(fileName); // This is getting file info OUTSIDE of the song library. This is important here. Don't change.

    // extract metadata
    QEventLoop loop;
    connect(player, &QMediaPlayer::mediaStatusChanged, &loop, [&loop](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::LoadedMedia) {
            loop.quit();
        }
        });
    loop.exec();

    // pass to datamanager to handle said metadata
    std::shared_ptr<Song> newSong = dataManager.parseSongData(fileName, *player);

    // updated player and library
    addPlayerInformation(newSong, fileInfo);
    addSongCardToLibraryList(newSong);

    // Track the current song index for next/previous navigation
    const auto& songs = dataManager.getMusicLibrary().getSongs();
    for (int i = 0; i < static_cast<int>(songs.size()); i++)
    {
        if (songs[i] == newSong)
        {
            currentSongIndex = i;
            break;
        }
    }
}

void MainWindow::onAnalyticsButtonClicked() {
    previousPageIndex = ui->stackedWidget->currentIndex();
    ui->stackedWidget->setCurrentWidget(ui->AnalyticsPage);
}

// Analytics Page
void MainWindow::onAnalyticsExitButtonClicked() {
    ui->stackedWidget->setCurrentIndex(previousPageIndex);
}

void MainWindow::addSongCardToLibraryList(std::shared_ptr<Song> song) {
    LibraryCard* card = new LibraryCard(song, this);
    QListWidgetItem* item = new QListWidgetItem();
    item->setSizeHint(card->sizeHint());
    ui->libraryList->addItem(item);
    ui->libraryList->setItemWidget(item, card);
   
}

void MainWindow::addPlayerInformation(std::shared_ptr<Song> song, QFileInfo fileInfo) {
    ui->playerTitleLabel->setText(QString::fromStdString(song->getTitle()));
    ui->playerArtistLabel->setText(QString::fromStdString(song->getArtist()));
    ui->playerFilePathLabel->setText(fileInfo.absoluteFilePath());  // Sets the filepath label
}

// Loads the music library from the data manager into the UI. Called in main.cpp after loading the library from file.
void MainWindow::loadLibraryToUI() {
    ui->libraryList->clear();

    const auto& songs = dataManager.getMusicLibrary().getSongs();

    for (const auto& song : songs) {
        if (song) {
            addSongCardToLibraryList(song);
        }
    }

    if (!songs.empty() && songs.front()) {
        QString savedPath = QString::fromStdString(songs.front()->getFilePath());
        QFileInfo fileInfo(savedPath);

		currentSongIndex = 0;                   // Set the current song index to the first song in the library

        mediaControl.usePlayer()->setSource(QUrl::fromLocalFile(savedPath));
        addPlayerInformation(songs.front(), fileInfo);
    }
}

/* Music Player Functions */

// Music Player : Scroll back button functionality.
void MainWindow::onBackButtonClicked()
{
    const auto& songs = dataManager.getMusicLibrary().getSongs();

    if (songs.empty())
    {
        return;
    }

    if (currentSongIndex <= 0)
    {
        return;
    }

    currentSongIndex--;

    std::shared_ptr<Song> prevSong = songs[currentSongIndex];
    if (!prevSong)
    {
        return;
    }

    QString savedPath = QString::fromStdString(prevSong->getFilePath());
    QFileInfo fileInfo(savedPath);

    mediaControl.usePlayer()->setSource(QUrl::fromLocalFile(savedPath));
    addPlayerInformation(prevSong, fileInfo);
    mediaControl.usePlayer()->play();
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
    const auto& songs = dataManager.getMusicLibrary().getSongs();

    if (songs.empty())
    {
        return;
    }

    if (currentSongIndex < 0 || currentSongIndex >= static_cast<int>(songs.size()) - 1)
    {
        return;
    }

    currentSongIndex++;

    std::shared_ptr<Song> nextSong = songs[currentSongIndex];
    if (!nextSong)
    {
        return;
    }

    QString savedPath = QString::fromStdString(nextSong->getFilePath());
    QFileInfo fileInfo(savedPath);

    mediaControl.usePlayer()->setSource(QUrl::fromLocalFile(savedPath));
    addPlayerInformation(nextSong, fileInfo);
    mediaControl.usePlayer()->play();
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

// Auto-save data when closing the application
void MainWindow::closeEvent(QCloseEvent* event)
{
    qDebug() << "Songs in library before save:" << dataManager.getMusicLibrary().getSongs().size();

    if (!dataManager.saveData("data/music_library.json"))
    {
        QMessageBox::warning(this, "Save Error", "Could not save music library data.");
    }

    QMainWindow::closeEvent(event);
}

/* PLAYLIST FUNCTIONALITY */
void MainWindow::onAddPlaylistButtonClicked() {
    previousPageIndex = ui->stackedWidget->currentIndex();

	loadPlaylistEditorSongsToUI(); // Load songs into the playlist editor before showing it

    ui->stackedWidget->setCurrentWidget(ui->editPlaylistPage);
}
void MainWindow::onPlaylistEditorExitButtonClicked() {
    ui->stackedWidget->setCurrentWidget(ui->songPlayerPage);
}









//________________________________________________________________________//
//---------------- NOTE: The PLAYLIST FUNCTIONALITY-----------------------//
//________________________________________________________________________//


// This function loads the list of playlists from the music library into the playlist selection UI. 
// It is called whenever the playlist data changes, 
// such as when a new playlist is created or an existing playlist is deleted.
void MainWindow::loadPlaylistsToUI() {
    ui->playlistCardBox->clear();

    const auto& playlists = dataManager.getMusicLibrary().getPlaylists();

    for (const auto& playlist : playlists) {
        if (playlist) {
            QListWidgetItem* item = new QListWidgetItem(
                QString::fromStdString(playlist->getName())
            );
            ui->playlistCardBox->addItem(item);
        }
    }

    if (!playlists.empty()) {
        currentPlaylistIndex = 0;
        loadCurrentPlaylistToUI();
    }
}

// This function loads the songs of the currently selected playlist into the playlist editor UI. 
// It is called whenever a new playlist is selected or when changes are made to the current playlist.
void MainWindow::loadCurrentPlaylistToUI() {
    ui->SongList->clear();

    const auto& playlists = dataManager.getMusicLibrary().getPlaylists();

    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= static_cast<int>(playlists.size())) {
        return;
    }

    Playlist* currentPlaylist = playlists[currentPlaylistIndex].get();
    if (!currentPlaylist) return;

    for (const auto& song : currentPlaylist->getSongs()) {
        if (song) {
            QListWidgetItem* item = new QListWidgetItem(
                QString::fromStdString(song->getTitle())
            );
            ui->SongList->addItem(item);
        }
    }
}

// This function is for when a user clicks the "Save Playlist" button. 
// It either creates a new playlist or saves changes to an existing playlist, 
// depending on the context. Currently, 
// it only creates new playlists, but it can be expanded to handle both cases.
void MainWindow::onSavePlaylistButtonClicked() {
    QString playlistName = ui->playlistNameLineEdit->text().trimmed();

    if (playlistName.isEmpty()) {
        QMessageBox::warning(this, "Invalid Playlist", "Playlist name cannot be empty.");
        return;
    }

    std::string playlistID = QUuid::createUuid().toString().toStdString();

    dataManager.getMusicLibrary().createPlaylist(playlistID, playlistName.toStdString());

    loadPlaylistsToUI();

    ui->playlistNameLineEdit->clear();
    ui->stackedWidget->setCurrentWidget(ui->songPlayerPage);
}

void MainWindow::onPlaylistSelected(int row) {
    currentPlaylistIndex = row;
    loadCurrentPlaylistToUI();
}

// This function is for when a user clicks the "Add Current Song to Playlist" button. 
// It adds the currently selected song in the library to the currently selected playlist.
void MainWindow::onAddCurrentSongToPlaylistClicked() {
    const auto& playlists = dataManager.getMusicLibrary().getPlaylists();
    const auto& songs = dataManager.getMusicLibrary().getSongs();

    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= static_cast<int>(playlists.size())) {
        QMessageBox::warning(this, "No Playlist Selected", "Please select a playlist first.");
        return;
    }

    if (currentSongIndex < 0 || currentSongIndex >= static_cast<int>(songs.size())) {
        QMessageBox::warning(this, "No Song Selected", "Please select a song first.");
        return;
    }

    Playlist* currentPlaylist = playlists[currentPlaylistIndex].get();
    std::shared_ptr<Song> currentSong = songs[currentSongIndex];

    if (!currentPlaylist || !currentSong) {
        return;
    }

    currentPlaylist->addSong(currentSong);

    ui->playlistCardBox->setCurrentRow(currentPlaylistIndex);
    loadCurrentPlaylistToUI();
}

// This function is for when a user clicks on a song in the playlist editor. 
// Current Song to Playlist, it adds the correct song.
void MainWindow::onPlaylistEditorSongSelected(int row)
{
    const auto& songs = dataManager.getMusicLibrary().getSongs();

    if (row < 0 || row >= static_cast<int>(songs.size()))
    {
        return;
    }

    currentSongIndex = row;
}

// This function is for when a user clicks on a song in the playlist editor.
void MainWindow::onPlaylistSongSelected(int row)
{
    const auto& playlists = dataManager.getMusicLibrary().getPlaylists();

    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= static_cast<int>(playlists.size()))
    {
        return;
    }

    Playlist* currentPlaylist = playlists[currentPlaylistIndex].get();
    if (!currentPlaylist)
    {
        return;
    }

    const auto& playlistSongs = currentPlaylist->getSongs();

    if (row < 0 || row >= static_cast<int>(playlistSongs.size()))
    {
        return;
    }

    std::shared_ptr<Song> selectedSong = playlistSongs[row];
    if (!selectedSong)
    {
        return;
    }

    const auto& librarySongs = dataManager.getMusicLibrary().getSongs();
    for (int i = 0; i < static_cast<int>(librarySongs.size()); i++)
    {
        if (librarySongs[i] == selectedSong)
        {
            currentSongIndex = i;
            break;
        }
    }

    QString savedPath = QString::fromStdString(selectedSong->getFilePath());
    QFileInfo fileInfo(savedPath);

    mediaControl.usePlayer()->setSource(QUrl::fromLocalFile(savedPath));
    addPlayerInformation(selectedSong, fileInfo);
    mediaControl.usePlayer()->play();
}

// This function loads the list of songs from the music library into the playlist editor UI.
void MainWindow::loadPlaylistEditorSongsToUI() {
    ui->playlistSongsList->clear();

    const auto& songs = dataManager.getMusicLibrary().getSongs();

    for (const auto& song : songs) {
        if (song) {
            QListWidgetItem* item = new QListWidgetItem(
                QString::fromStdString(song->getTitle())
            );
            ui->playlistSongsList->addItem(item);
        }
    }
}


