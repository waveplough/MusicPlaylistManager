#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "MediaController.h"
#include "Song.h"
#include "LibraryCard.h"
#include "PlaylistManager.h"
#include "PlaylistCard.h"
#include "DataManager.h"
#include "SongCard.h"
#include <QMediaMetaData>
#include <QUuid>
#include <QCloseEvent>
#include <QMessageBox>
#include <QSignalBlocker>       // For blocking signals when updating the playbar position programmatically.
#include <QInputDialog>
#include <QEvent>
#include <QMouseEvent>
#include <QLineEdit>
#include <AnalyticsEngine.h>
#include <QStandardItemModel>
#include <QStandardItem>
#include <string>


MainWindow::MainWindow(MediaController &mediaControl,DataManager& dataManager, PlaylistManager& playlistManager, QWidget *parent)
    : QMainWindow(parent)
    , mediaControl(mediaControl)
    , dataManager(dataManager)
    , playlistManager(playlistManager)
    , ui(new Ui::MainWindow)

    
{

    ui->setupUi(this);
    ui->playlistNameLabel->installEventFilter(this);
    ui->playlistNameLabel->setCursor(Qt::PointingHandCursor);
    ui->stackedWidget->setCurrentWidget(ui->songPlayerPage);

    // Forces the main splitter to size. Not doable in create.
    ui->mainSplitter->setSizes({ 551, 240, 240 });
    ui->menuAnalytics->setFocusPolicy(Qt::NoFocus);

    // Slots and signals manual connections

    // Menu
    connect(ui->actionNewSong, &QAction::triggered, this, &MainWindow::onNewSongButtonClicked);
    connect(ui->actionViewAnalytics, &QAction::triggered, this, &MainWindow::onAnalyticsButtonClicked);
    connect(ui->actionImport_Playlist_m3u, &QAction::triggered, this, &MainWindow::onImportM3UButtonClicked);

    // Playlist
    connect(ui->addPlaylistButton, &QPushButton::clicked, this, &MainWindow::onAddPlaylistButtonClicked);
    connect(ui->exitPlaylistEditor, &QPushButton::clicked, this, &MainWindow::onPlaylistEditorExitButtonClicked);
    connect(ui->playlistCardBox, &QListWidget::itemClicked, this, [this](QListWidgetItem* item) {                   //  This is for when a user selects a playlist from the playlist selection list.
        onPlaylistSelected(ui->playlistCardBox->row(item));
        }); 
	connect(ui->trashButton, &QPushButton::clicked, this, &MainWindow::onTrashButtonClicked);//  This is for when a user clicks the "Delete a song from Playlist" button in the playlist editor.
	

    connect(ui->addButton, &QPushButton::clicked, this, &MainWindow::onAddCurrentSongToPlaylistClicked);    //  This is for when a user clicks the "Add Current Song to Playlist" button. 
	connect(ui->playlistNameLineEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);     // This is for when a user searches the song in the library card editor.
    connect(ui->libraryList, &QListWidget::currentRowChanged, this, &MainWindow::onMusicLibrarySongSelected);    // This is for when a user clicks on a song in the music library.
    connect(ui->playlistSongsList, &QListWidget::currentRowChanged,this, &MainWindow::onPlaylistEditorSongSelected);
	connect(ui->moveUpButton, &QPushButton::clicked, this, &MainWindow::onMoveUpClicked);  //   This is for when a user clicks the "Move Up" button in the playlist editor.
	connect(ui->moveDownButton, &QPushButton::clicked, this, &MainWindow::onMoveDownClicked);//   This is for when a user clicks the "Move Down" button in the playlist editor.
	connect(ui->reorderButton, &QPushButton::clicked, this, &MainWindow::onReorderClicked); //   This is for when a user clicks the "Reorder Playlist" button in the playlist editor.
    // Player
    connect(ui->playerVolumeButton, &QPushButton::clicked, this, &MainWindow::onPlayerVolumeButtonClicked);
    connect(ui->backButton, &QPushButton::clicked, this, &MainWindow::onBackButtonClicked);
    //connect(ui->backButton, &QPushButton::pressed, this, &MainWindow::onBackButtonPressed);       // UNUSED
    connect(ui->stopButton, &QPushButton::clicked, this, &MainWindow::onStopButtonClicked);
    connect(ui->playButton, &QPushButton::clicked, this, &MainWindow::onPlayButtonClicked);
    connect(ui->pauseButton, &QPushButton::clicked, this, &MainWindow::onPauseButtonClicked);
    connect(ui->forwardButton, &QPushButton::clicked, this, &MainWindow::onForwardButtonClicked);
    //connect(ui->forwardButton, &QPushButton::pressed, this, &MainWindow::onForwardButtonPressed); // UNUSED
    
    connect(ui->playerPlaybar, &QSlider::valueChanged, this, &MainWindow::onPlayerPlaybarValueChanged);
    connect(ui->playerPlaybar, &QSlider::sliderMoved, this, &MainWindow::onPlayerPlaybarMoved);
    connect(ui->playerVolumeSlider, &QSlider::valueChanged, this, &MainWindow::onPlayerVolumeSliderValueChanged);

    // Analytics
    connect(ui->exitButton, &QPushButton::clicked, this, &MainWindow::onAnalyticsExitButtonClicked);

    listeningTimer = new QTimer(this);
    connect(listeningTimer, &QTimer::timeout, this, &MainWindow::updateListeningTime);
    listeningTimer->start(1000);

    /* Object Manipulation */

    // Player Forward Button
    ui->forwardButton->setAutoRepeat(true);         // Must set auto repeat to happen on a press button
    ui->forwardButton->setAutoRepeatDelay(100);     // Set the delay for the functionality to start.
    ui->forwardButton->setAutoRepeatInterval(100);  // Set the autorepeat interval of increase.

    // Player Backward Button
    ui->backButton->setAutoRepeat(true);            // Must set auto repeat to happen on a press button
    ui->backButton->setAutoRepeatDelay(100);        // Set the delay for the functionality to start.
    ui->backButton->setAutoRepeatInterval(100);     // Set the autorepeat interval of increase.

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


    // Song Editor
    connect(ui->submitButton, &QPushButton::clicked, this, &MainWindow::onSongEditorSubmitButtonClicked);
    connect(ui->deleteButton, &QPushButton::clicked, this, &MainWindow::onSongEditorDeleteButtonClicked);
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

void MainWindow::onImportM3UButtonClicked() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Playlist Import"), "", tr("M3U Files (*.m3u *.m3u8)"));

    QString playlistName = QFileInfo(fileName).baseName();             // Basename gets the non-filepath name
    
    std::string id = generateID();                                     // Use playlist create functions
    playlistManager.getMusicLibrary()->createPlaylist(id, playlistName.toStdString());

    // Have to stop to get a pointer to the playlist
    Playlist* ourNewPL = playlistManager.getMusicLibrary()->findPlaylist(id);
    if (!ourNewPL) {
        QMessageBox::warning(this, "Error", "Failed to create new playlist."); // A popup warning
        return;
    }

    QDir dir(QCoreApplication::applicationDirPath());                   // 

    QFile importedPL(fileName);
    QTextStream infoStream(&importedPL);
    QString line;
    Song tempSong;
    bool found = false;

    importedPL.open(QIODevice::ReadOnly | QIODevice::Text);             // Open the file

    while (infoStream.readLineInto(&line)) {

        line = line.trimmed();                                          // Trim whitespace
        
        if (line.startsWith('#') || line.isEmpty()) {                   // If comment, continue.
            continue;
        }
            
        QString absolute = dir.absoluteFilePath(line);                  // Retrieve absolute path from the line. Could be done other ways, but is safer.

        std::shared_ptr<Song> existing = playlistManager.getMusicLibrary()->findSongByPath(absolute.toStdString()); // Calls find song by path, which I was smart enough to add a return to.

        if (existing) {
            ourNewPL->addSong(existing);
            continue;
        }

        // Else - Make a new song to add                                // Code comes from DataManager
        
        std::string songId = generateID();                              // Generate a song ID

        // Create generic song variables
        std::string title = QFileInfo(absolute).baseName().toStdString();
        std::string artist = "Unknown Artist";
        std::string album = "N/a";
        std::string genre = "Unknown Genre";
        int duration = UNKNOWN_DUR;

        // Make a song with default variables
        std::shared_ptr<Song> newSong = std::make_shared<Song>(
            songId,
            title,
            duration,
            artist,
            album,
            genre,
            absolute.toStdString()                                      // absolute is a QString. They are not compatible.
        );

        playlistManager.getMusicLibrary()->addSong(newSong);            // add song to library song vector

        ourNewPL->addSong(newSong);                                     // Add to the new playlist.

    }

    loadLibraryToUI();                                                  // Should update the UI

// https://doc.qt.io/qt-6/qtextstream.html

// Source - https://stackoverflow.com/a/17338470
// Posted by Pavel Strakhov
// Retrieved 2026-04-07, License - CC BY-SA 3.0


}
void MainWindow::onNewSongButtonClicked() {

    QMediaPlayer tempPlayer;
    QAudioOutput tempOutput;
    tempPlayer.setAudioOutput(&tempOutput);

    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Audio File"), "", tr("MP3 Files (*.mp3)"));
    if (fileName.isEmpty()) return;

    tempPlayer.setSource(QUrl::fromLocalFile(fileName));

    // Extract metadata
    QEventLoop loop;
    connect(&tempPlayer, &QMediaPlayer::mediaStatusChanged, &loop, [&loop](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::LoadedMedia) {
            loop.quit();
        }
        });
    loop.exec();

    std::shared_ptr<Song> newSong = dataManager.parseSongData(fileName, tempPlayer);

    if (!newSong) {
        QMessageBox::information(this, "Duplicate", "This song is already in your library.");
        return;
    }

    // Stop current playback
    mediaControl.usePlayer()->stop();

    // Set the new song as current
    mediaControl.setCurrentSong(newSong);

    playCountedForCurrentSong = false;

    // Load the new song into the main player
    mediaControl.usePlayer()->setSource(QUrl::fromLocalFile(fileName));

    QFileInfo fileInfo(fileName);
    addPlayerInformation(newSong, fileInfo);
    addSongCardToLibraryList(newSong);
    addSongEditorInformation(newSong);

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

    connect(card, &LibraryCard::libraryCardDoubleClick,
        this, &MainWindow::onSongCardDoubleClicked);
}

void MainWindow::addSongCardToSongsList(std::shared_ptr<Song> song) {
    ui->playlistSongsList->clear();
    songCard* card = new songCard(song, this);      // gets shared pointer reference of its own
    QListWidgetItem* item = new QListWidgetItem();
    item->setSizeHint(card->sizeHint());
    ui->SongList->addItem(item);
    ui->SongList->setItemWidget(item, card);
}

void MainWindow::addPlayerInformation(std::shared_ptr<Song> song, QFileInfo fileInfo) {
    if (!song) {
        ui->playerTitleLabel->setText("No Title");
        ui->playerArtistLabel->setText("Unknown Artist");
        ui->playerFilePathLabel->setText("No file selected");
        return;
    }

    // Title
    std::string title = song->getTitle();
    ui->playerTitleLabel->setText(QString::fromStdString(title.empty() ? "No Title" : title));

    // Artist
    std::string artist = song->getArtist();
    ui->playerArtistLabel->setText(QString::fromStdString(artist.empty() ? "Unknown Artist" : artist));

    // File path
    if (fileInfo.exists() && !fileInfo.absoluteFilePath().isEmpty()) {
        ui->playerFilePathLabel->setText(fileInfo.absoluteFilePath());
    }
    else {
        ui->playerFilePathLabel->setText("File not found");
    }
}

void MainWindow::addSongEditorInformation(std::shared_ptr<Song> song) {
    if (!song) {
        // Set placeholder hints to defaults if song is null
        ui->lineEditSongName->setText("No Title");
        ui->lineEditGenre->setText("Unknown");
        ui->lineEditArtist->setText("Unknown Artist");
        ui->lineEditAlbum->setText("N/A");
        return;
    }

    // Title
    std::string title = song->getTitle();
    ui->lineEditSongName->setText(QString::fromStdString(title.empty() ? "No Title" : title));

    // Genre
    std::string genre = song->getGenre();
    ui->lineEditGenre->setText(QString::fromStdString(genre.empty() ? "Unknown" : genre));

    // Artist
    std::string artist = song->getArtist();
    ui->lineEditArtist->setText(QString::fromStdString(artist.empty() ? "Unknown Artist" : artist));

    // Album
    std::string album = song->getAlbum();
    ui->lineEditAlbum->setText(QString::fromStdString(album.empty() ? "N/A" : album));


}

// Loads the music library from the data manager into the UI. Called in main.cpp after loading the library from file.
void MainWindow::loadLibraryToUI() {
    ui->libraryList->clear();
    ui->playlistCardBox->clear();

    const auto& songs = playlistManager.getMusicLibrary()->getSongs();
    const auto& playlists = playlistManager.getMusicLibrary()->getPlaylists();

    currentSearchResults = songs;

    for (const auto& song : songs) {
        if (song) {
            addSongCardToLibraryList(song);

        }
    }

    if (!songs.empty() && songs.front()) {
        QString savedPath = QString::fromStdString(songs.front()->getFilePath());
        QFileInfo fileInfo(savedPath);

        mediaControl.usePlayer()->setSource(QUrl::fromLocalFile(savedPath));
        addPlayerInformation(songs.front(), fileInfo);
        addSongEditorInformation(songs.front());
        mediaControl.setCurrentSong(songs.front());
    }
    else {
        ui->playerTitleLabel->setText("No Title");
        ui->playerArtistLabel->setText("Unknown Artist");
        ui->playerFilePathLabel->setText("Filepath");

        ui->lineEditSongName->setPlaceholderText("No Title");
        ui->lineEditGenre->setPlaceholderText("Unknown");
        ui->lineEditArtist->setPlaceholderText("Unknown Artist");
        ui->lineEditAlbum->setPlaceholderText("N/A");
    }

    for (const auto& p : playlists) {
        playlistCard* card = new playlistCard(p.get(), this);
        QListWidgetItem* item = new QListWidgetItem();
        item->setSizeHint(card->sizeHint());
        ui->playlistCardBox->addItem(item);
        ui->playlistCardBox->setItemWidget(item, card);

        connect(card, &playlistCard::deletePlaylistRequested,
            this, &MainWindow::onDeletePlaylistButtonClicked);
    }
   
}

// Music Library Search Functionality. Connected to the textChanged signal of the search bar in the library section. 
// Filters the songs in the library based on the search query and updates the displayed list accordingly.
void MainWindow::onSearchTextChanged(const QString& text)
{
    QString trimmedText = text.trimmed();

    ui->libraryList->clear();

    if (trimmedText.isEmpty()) {
        const auto& songs = playlistManager.getMusicLibrary()->getSongs();

        currentSearchResults = songs;

        for (const auto& song : songs) {
            if (song) {
                addSongCardToLibraryList(song);
            }
        }
        return;
    }

    currentSearchResults = playlistManager.getMusicLibrary()->searchSongs(trimmedText.toStdString());

    for (const auto& song : currentSearchResults) {
        if (song) {
            addSongCardToLibraryList(song);
        }
    }
}

/* Music Player Functions */

// Music Player : Scroll back button functionality. One-time click.
void MainWindow::onBackButtonClicked() 
{
    ui->playerPlaybar->setValue(ui->playerPlaybar->value() - 2);
        // Jumps backward by 30 seconds + the current playerbar value.
    mediaControl.usePlayer()->setPosition(ui->playerPlaybar->value() * 1000);
        // Sets the actual song to that position.
}

// UNUSED
// Music Player : Scroll back button functionality. Held down scrolling.
//void MainWindow::onBackButtonPressed()
//{
//    ui->playerPlaybar->setValue(ui->playerPlaybar->value() - 0.01);
//    // Jumps backward by 0.01 second + the current playerbar value.
//    mediaControl.usePlayer()->setPosition(ui->playerPlaybar->value() * 1000);
//    // Sets the actual song to that position.
//}

// Music Player : Stop button functionality.
void MainWindow::onStopButtonClicked() 
{
    mediaControl.usePlayer()->stop();
}

// Music Player : Play button functionality.
void MainWindow::onPlayButtonClicked()
{
    auto currentSong = mediaControl.getCurrentSong();

    if (currentSong && mediaControl.usePlayer()->playbackState() != QMediaPlayer::PlayingState) {
        // Only count if not already counted for this song play session
        if (!playCountedForCurrentSong) {
            currentSong->incrementPlayCount();
            playCountedForCurrentSong = true;
        }
    }

    mediaControl.usePlayer()->play();
}

// Music Player : Pause button functionality.
void MainWindow::onPauseButtonClicked() 
{
    mediaControl.usePlayer()->pause();
}

// Music Player : Scroll forward button functionality - one-time click.
void MainWindow::onForwardButtonClicked() 
{
    ui->playerPlaybar->setValue(ui->playerPlaybar->value() + 3);
        // Jumps forward by 30 seconds + the current playerbar value.
    mediaControl.usePlayer()->setPosition(ui->playerPlaybar->value() * 1000);
        // Sets the actual song to that position.
}

// UNUSED
// Music Player : Scroll forward button functionality - held down scrolling.
//void MainWindow::onForwardButtonPressed()
//{
//    ui->playerPlaybar->setValue(ui->playerPlaybar->value() + 0.01);
//    // Jumps forward by 0.01 seconds + the current playerbar value.
//    mediaControl.usePlayer()->setPosition(ui->playerPlaybar->value() * 1000);
//    // Sets the actual song to that position.
//}

    // Playbar related //
// Music Player : Playbar (slider) value adjustment.
void MainWindow::onPlayerPlaybarValueChanged(int value)
{
    if (ui->playerPlaybar->isSliderDown()) 
    {
        mediaControl.usePlayer()->setPosition(value * 1000);
    }
}

// Music Player : Playbar (slider) clicked the slider to move it.
void MainWindow::onPlayerPlaybarMoved(int value)
{
    mediaControl.usePlayer()->setPosition(value * 1000);
}


// Music Player : Playbar duration sync
void MainWindow::updateDuration(qint64 duration)
    // Note: QT is weird and often works in Miliseconds.
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

// Music Player: Adds toggled Mute / Active functionality for the Player's Volume (mute) button.
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

/* PERSISTENCE FUNCTIONS */

// Temporary: Auto-save data when closing the application
void MainWindow::closeEvent(QCloseEvent* event)
{
    qDebug() << "Songs in library before save:" << playlistManager.getMusicLibrary()->getSongs().size();

    if (!dataManager.saveData("data/music_library.json"))
    {
        QMessageBox::warning(this, "Save Error", "Could not save music library data.");
    }

    QMainWindow::closeEvent(event);
}

/* PLAYLIST FUNCTIONALITY */

void MainWindow::addPlaylistCard() {
    std::string id = generateID();
    playlistManager.getMusicLibrary()->createPlaylist(id, "Name");

    Playlist* newPlaylist = nullptr;
    for (const auto& p : playlistManager.getMusicLibrary()->getPlaylists()) {
        if (p->getPlaylistID() == id) {
            newPlaylist = p.get();
            break;
        }
    }

    playlistCard* card = new playlistCard(newPlaylist, this);
    QListWidgetItem* item = new QListWidgetItem();
    item->setSizeHint(card->sizeHint());
    ui->playlistCardBox->addItem(item);
    ui->playlistCardBox->setItemWidget(item, card);

    connect(card, &playlistCard::deletePlaylistRequested,
        this, &MainWindow::onDeletePlaylistButtonClicked);
}

void MainWindow::onAddPlaylistButtonClicked() {
    addPlaylistCard();
}

void MainWindow::onPlaylistSelected(int row) {
    currentPlaylistIndex = row;
	currentPlaylistSongIndex = -1;      //  Resets the current song index in the playlist editor when a new playlist is selected.
    const auto& playlists = playlistManager.getMusicLibrary()->getPlaylists();
    if (row >= 0 && row < static_cast<int>(playlists.size()) && playlists[row]) {
        ui->playlistNameLabel->setText(QString::fromStdString(playlists[row]->getName()));
    }
    loadCurrentPlaylistToUI();
    loadPlaylistEditorSongsToUI();
    previousPageIndex = ui->stackedWidget->currentIndex();
    ui->stackedWidget->setCurrentWidget(ui->editPlaylistPage);
}

void MainWindow::onPlaylistEditorExitButtonClicked() {
    currentPlaylistIndex = -1;
	currentPlaylistSongIndex = -1;
    ui->stackedWidget->setCurrentWidget(ui->songPlayerPage);
}


// This function loads the songs of the currently selected playlist into the playlist editor UI. 
// It is called whenever a new playlist is selected or when changes are made to the current playlist.
void MainWindow::loadCurrentPlaylistToUI() {
    ui->SongList->clear();

    const auto& playlists = playlistManager.getMusicLibrary()->getPlaylists();

    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= static_cast<int>(playlists.size())) {
        return;
    }

    Playlist* currentPlaylist = playlists[currentPlaylistIndex].get();
    if (!currentPlaylist) return;

    for (const auto& song : currentPlaylist->getSongs()) {
        if (song) {
            songCard* card = new songCard(song, this);      // gets shared pointer reference of its own
            QListWidgetItem* item = new QListWidgetItem();
            item->setSizeHint(card->sizeHint());
            ui->SongList->addItem(item);
            ui->SongList->setItemWidget(item, card);

            // Connect for double click button functionality to activate player.
            connect(card, &songCard::songCardDoubleClick, this, &MainWindow::onSongCardDoubleClicked);
        }
    }
}

void MainWindow::onAddCurrentSongToPlaylistClicked() {
    const auto& playlists = playlistManager.getMusicLibrary()->getPlaylists();
    const auto& songs = playlistManager.getMusicLibrary()->getSongs();

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

    const std::string& playlistId = currentPlaylist->getPlaylistID(); // get their id's
    const std::string& songId = currentSong->getItemID();

    if (!currentPlaylist || !currentSong) {
        return;
    }

    playlistManager.addSongToPlaylist(songId, playlistId);

    loadLibraryToUI();
    ui->playlistCardBox->setCurrentRow(currentPlaylistIndex);
    loadCurrentPlaylistToUI();
    loadPlaylistEditorSongsToUI();
}

// This function is for when a user clicks on a song in the music library. 
// Current Song to Playlist, it adds the correct song.
void MainWindow::onMusicLibrarySongSelected(int row)
{
    if (row < 0 || row >= static_cast<int>(currentSearchResults.size()))
    {
        currentSongIndex = -1;
        return;
    }

    std::shared_ptr<Song> selectedSong = currentSearchResults[row];

    if (!selectedSong) {
        currentSongIndex = -1;
        return;
    }

    const auto& allSongs = playlistManager.getMusicLibrary()->getSongs();

    currentSongIndex = -1;

    for (int i = 0; i < static_cast<int>(allSongs.size()); ++i) {
        if (allSongs[i] && allSongs[i]->getItemID() == selectedSong->getItemID()) {
            currentSongIndex = i;
            break;
        }
    }
    mediaControl.setCurrentSong(selectedSong);
}

// This function loads the list of songs from the music library into the playlist editor UI.
void MainWindow::loadPlaylistEditorSongsToUI() {
    ui->playlistSongsList->clear();


    const auto& playlists = playlistManager.getMusicLibrary()->getPlaylists();

    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= static_cast<int>(playlists.size())) {
        return;
    }

    Playlist* currentPlaylist = playlists[currentPlaylistIndex].get();
    if (!currentPlaylist) return;

    for (const auto& song : currentPlaylist->getSongs()) {
        if (song) {
            songCard* card = new songCard(song, this);      // gets shared pointer reference of its own
            QListWidgetItem* item = new QListWidgetItem();
            item->setSizeHint(card->sizeHint());
            ui->playlistSongsList->addItem(item);
            ui->playlistSongsList->setItemWidget(item, card);

            // Connect for double click button functionality to activate player.
            connect(card, &songCard::songCardDoubleClick, this, &MainWindow::onSongCardDoubleClicked);
        }
    }
}

// A function to play a song from a double clicked song card
void MainWindow::onSongCardDoubleClicked(std::shared_ptr<Song> song)
{
    if (!song) return;

    // Get currently playing song
    std::shared_ptr<Song> currentSong = mediaControl.getCurrentSong();

    // Only increment play count if it's a different song
    song->incrementPlayCount();

    mediaControl.setCurrentSong(song);

    playCountedForCurrentSong = false;

    // Set media in the player
    mediaControl.usePlayer()->setSource(QUrl::fromLocalFile(QString::fromStdString(song->getFilePath())));
    // Takes the filepath from the song object.

    // Update the Ui info pane
    QFileInfo fileInfo(QString::fromStdString(song->getFilePath()));
    // Has to turn the filepath from the song into a QFileInfo object. Complicated but necessary.
    addPlayerInformation(song, fileInfo);
    addSongEditorInformation(song);
    // Calls the player info update method

    // Activate the song page
    previousPageIndex = ui->stackedWidget->currentIndex();
    ui->stackedWidget->setCurrentWidget(ui->songPlayerPage);

    // The player plays the song
    // Note: The if clause does nothing. Unsure why. Keep for now, but non-functional currently.
    if (mediaControl.usePlayer()->playbackState() != QMediaPlayer::PlaybackState::PlayingState)
        // It glitches if it is already playing and tries to play again. QT 6 Syntax is strong here.
    {
        mediaControl.usePlayer()->play();
    }
}


//  This function allows the user to click on the playlist name label in the playlist editor to rename the playlist. 
// It uses an input dialog to get the new name and updates the playlist and UI accordingly.
bool MainWindow::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == ui->playlistNameLabel && event->type() == QEvent::MouseButtonPress)
    {
        const auto& playlists = playlistManager.getMusicLibrary()->getPlaylists();

        if (currentPlaylistIndex < 0 || currentPlaylistIndex >= static_cast<int>(playlists.size())) {
            return true;
        }

        Playlist* currentPlaylist = playlists[currentPlaylistIndex].get();
        if (!currentPlaylist) return true;

        bool ok = false;
        QString newName;
        bool validName = false;

        while (!validName) {
            newName = QInputDialog::getText(
                this,
                "Rename Playlist",
                "Enter playlist name (max 20 characters):",
                QLineEdit::Normal,
                QString::fromStdString(currentPlaylist->getName()),
                &ok
            );

            // User cancelled
            if (!ok) {
                return true;
            }

            // Check if empty
            if (newName.trimmed().isEmpty()) {
                QMessageBox::warning(this, "Invalid Name",
                    "Playlist name cannot be empty. Please enter a name.");
                continue;
            }

            // Check length
            if (newName.length() > 20) {
                QMessageBox::warning(this, "Name Too Long",
                    QString("Playlist name cannot exceed 20 characters.\n"
                        "You entered %1 characters.\n\n"
                        "Please enter a shorter name.").arg(newName.length()));
                continue;
            }

            // Name is valid
            validName = true;
        }

        // Apply the new name
        currentPlaylist->setName(newName.toStdString());
        ui->playlistNameLabel->setText(newName);

        loadLibraryToUI();
        ui->playlistCardBox->setCurrentRow(currentPlaylistIndex);
        loadCurrentPlaylistToUI();
        loadPlaylistEditorSongsToUI();

        return true;
    }

    return QMainWindow::eventFilter(watched, event);
}


//___________________________________________________________________________________________________________________________________//
//____________________________Data Manipulation Functions for Playlist Editor Song List______________________________________________//
//__________________________________________________________________________________________________________________________________ //


// This function refreshes the playlist editor song list UI while keeping the current selection highlighted.
void MainWindow::refreshPlaylistViewsAndKeepSelection(int row)
{
    {
        QSignalBlocker blocker(ui->playlistSongsList);
        loadCurrentPlaylistToUI();
        loadPlaylistEditorSongsToUI();
    }

    currentPlaylistSongIndex = row;

    if (row >= 0 && row < ui->playlistSongsList->count()) {
        ui->playlistSongsList->setCurrentRow(row);
    }
    else {
        currentPlaylistSongIndex = -1;
    }
}

// This function is for when a user clicks on a song in the playlist editor's song list.
void MainWindow::onPlaylistEditorSongSelected(int row)
{
    const auto& playlists = playlistManager.getMusicLibrary()->getPlaylists();

    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= static_cast<int>(playlists.size())) {
        currentPlaylistSongIndex = -1;
        return;
    }

    Playlist* currentPlaylist = playlists[currentPlaylistIndex].get();
    if (!currentPlaylist) {
        currentPlaylistSongIndex = -1;
        return;
    }

    const auto& songs = currentPlaylist->getSongs();

    if (row < 0 || row >= static_cast<int>(songs.size())) {
        currentPlaylistSongIndex = -1;
        return;
    }

    currentPlaylistSongIndex = row;
}

// This function is for when a user clicks the "Move Up" button in the playlist editor. 
// It moves the selected song up in the playlist order.
void MainWindow::onMoveUpClicked()
{
    const auto& playlists = playlistManager.getMusicLibrary()->getPlaylists();

    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= static_cast<int>(playlists.size())) {
        QMessageBox::warning(this, "No Playlist Selected", "Please select a playlist first.");
        return;
    }

    if (currentPlaylistSongIndex <= 0) {
        return;
    }

    Playlist* currentPlaylist = playlists[currentPlaylistIndex].get();
    if (!currentPlaylist) {
        return;
    }

    int newRow = currentPlaylistSongIndex - 1;

    currentPlaylist->reorderSong(
        static_cast<size_t>(currentPlaylistSongIndex),
        static_cast<size_t>(newRow)
    );

    refreshPlaylistViewsAndKeepSelection(newRow);
}

// This function is for when a user clicks the "Move Down" button in the playlist editor.
void MainWindow::onMoveDownClicked()
{
    const auto& playlists = playlistManager.getMusicLibrary()->getPlaylists();

    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= static_cast<int>(playlists.size())) {
        QMessageBox::warning(this, "No Playlist Selected", "Please select a playlist first.");
        return;
    }

    Playlist* currentPlaylist = playlists[currentPlaylistIndex].get();
    if (!currentPlaylist) {
        return;
    }

    const auto& songs = currentPlaylist->getSongs();

    if (currentPlaylistSongIndex < 0 || currentPlaylistSongIndex >= static_cast<int>(songs.size()) - 1) {
        return;
    }

    int newRow = currentPlaylistSongIndex + 1;

    currentPlaylist->reorderSong(
        static_cast<size_t>(currentPlaylistSongIndex),
        static_cast<size_t>(newRow)
    );

    refreshPlaylistViewsAndKeepSelection(newRow);
}

// This function is for when a user clicks the "Reorder Playlist" button in the playlist editor.
void MainWindow::onReorderClicked()
{
    const auto& playlists = playlistManager.getMusicLibrary()->getPlaylists();

    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= static_cast<int>(playlists.size())) {
        QMessageBox::warning(this, "No Playlist Selected", "Please select a playlist first.");
        return;
    }

    Playlist* currentPlaylist = playlists[currentPlaylistIndex].get();
    if (!currentPlaylist) return;

    const auto& songs = currentPlaylist->getSongs();

    if (currentPlaylistSongIndex < 0 || currentPlaylistSongIndex >= static_cast<int>(songs.size())) {
        QMessageBox::warning(this, "No Song Selected", "Please select a song first.");
        return;
    }

    bool ok = false;

    int newPosition = QInputDialog::getInt(
        this,
        "Reorder Song",
        "Enter new position:",
        currentPlaylistSongIndex + 1,     // default position
        1,
        static_cast<int>(songs.size()),
        1,
        &ok
    );

    if (!ok) return;

    int newRow = newPosition - 1;

    if (newRow == currentPlaylistSongIndex) return;

    currentPlaylist->reorderSong(
        static_cast<size_t>(currentPlaylistSongIndex),
        static_cast<size_t>(newRow)
    );

    refreshPlaylistViewsAndKeepSelection(newRow);
}

//  This function is for when a user clicks the "Remove from Playlist" button in the playlist editor.
void MainWindow::onTrashButtonClicked()
{
    const auto& playlists = playlistManager.getMusicLibrary()->getPlaylists();

    if (currentPlaylistIndex < 0 || currentPlaylistIndex >= static_cast<int>(playlists.size())) {
        QMessageBox::warning(this, "No Playlist Selected", "Please select a playlist first.");
        return;
    }

    Playlist* currentPlaylist = playlists[currentPlaylistIndex].get();
    if (!currentPlaylist) {
        return;
    }

    const auto& songs = currentPlaylist->getSongs();

    if (currentPlaylistSongIndex < 0 || currentPlaylistSongIndex >= static_cast<int>(songs.size())) {
        QMessageBox::warning(this, "No Song Selected", "Please select a song from the playlist first.");
        return;
    }

    std::shared_ptr<Song> selectedSong = songs[currentPlaylistSongIndex];
    if (!selectedSong) {
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Remove Song",
        "Remove this song from the playlist?",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply != QMessageBox::Yes) {
        return;
    }

    currentPlaylist->removeSong(selectedSong->getItemID());

    loadLibraryToUI();

    currentPlaylistSongIndex = -1;

    loadCurrentPlaylistToUI();
    loadPlaylistEditorSongsToUI();

    dataManager.saveData("data/music_library.json");
}






























//___________________________________________________________________________________________________________________________________//
//___________________________________________________SONG EDITOR FUNCTION______________________________________________//
//__________________________________________________________________________________________________________________________________ //

void MainWindow::onSongEditorSubmitButtonClicked() {
    std::shared_ptr<Song> currentSong = mediaControl.getCurrentSong();

    // Check if the current song is in an invalid state
    if (!currentSong) {
        QMessageBox::warning(this, "No Song Selected",
            "No song is currently selected. Please select a song from the library or playlist first.");
        return;
    }

    // Get the text from UI
    QString titleText = ui->lineEditSongName->text();
    QString artistText = ui->lineEditArtist->text();
    QString albumText = ui->lineEditAlbum->text();
    QString genreText = ui->lineEditGenre->text();

    bool hasError = false;
    QString errorMessage;

    // Validate title name
    if (titleText.length() > 20) {
        errorMessage += QString("Title (you entered %1)\n").arg(titleText.length());
        hasError = true;
    }

    // Validate artist name
    if (artistText.length() > 20) {
        errorMessage += QString("Artist (you entered %1)\n").arg(artistText.length());
        hasError = true;
    }

    // Validate album name 
    if (albumText.length() > 20) {
        errorMessage += QString("Album (you entered %1)\n").arg(albumText.length());
        hasError = true;
    }

    // Validate genre 
    if (genreText.length() > 20) {
        errorMessage += QString("Genre (you entered %1)\n").arg(genreText.length());
        hasError = true;
    }

    if (hasError) {
        QMessageBox::warning(this, "Invalid Form Submission",
            QString("The following fields exceed the 20 character limit:\n\n%1\n\Please try again.").arg(errorMessage));
        return;
    }

    // Assign to the current song
    QFileInfo fileInfo(QString::fromStdString(currentSong->getFilePath()));
    currentSong->setTitle(titleText.toStdString());
    currentSong->setArtist(artistText.toStdString());
    currentSong->setAlbum(albumText.toStdString());
    currentSong->setGenre(genreText.toStdString());

    // Repaint 
    addPlayerInformation(currentSong, fileInfo);
    addSongEditorInformation(currentSong);

    ui->libraryList->clear();

    const auto& songs = playlistManager.getMusicLibrary()->getSongs();

    for (const auto& song : songs) {
        if (song) {
            addSongCardToLibraryList(song);
        }
    }
}

void MainWindow::onSongEditorDeleteButtonClicked() {
    std::shared_ptr<Song> currentSong = mediaControl.getCurrentSong();

    // If no current song is in place, display a warning
    if (!currentSong) {
        QMessageBox::warning(this, "No Song Selected",
            "No song is currently selected. Please select a song first.");
        return;
    }

    // Check if the song being deleted is currently playing
    bool isCurrentlyPlaying = (mediaControl.usePlayer()->playbackState() == QMediaPlayer::PlayingState);

    // Stop playback if this song is playing
    if (isCurrentlyPlaying) {
        mediaControl.usePlayer()->stop();
    }

    // CRITICAL: Clear the player's source so it doesn't keep the deleted file
    mediaControl.usePlayer()->setSource(QUrl()); 

    const std::string songID = currentSong->getItemID();

    // Use the id to recursively delete every instance of the song
    playlistManager.getMusicLibrary()->deleteSong(songID);

    // Clear the current song reference in MediaController since it's deleted
    mediaControl.setCurrentSong(nullptr);

    // Repaint UI
    loadLibraryToUI();
    loadCurrentPlaylistToUI();

    // Clear the editor fields
    ui->lineEditSongName->clear();
    ui->lineEditArtist->clear();
    ui->lineEditAlbum->clear();
    ui->lineEditGenre->clear();

}

void MainWindow::onAnalyticsButtonClicked() {
    // First switch to the Analytics page
    previousPageIndex = ui->stackedWidget->currentIndex();
    ui->stackedWidget->setCurrentWidget(ui->AnalyticsPage);

    // Get all songs
    const auto& allSongs = playlistManager.getMusicLibrary()->getSongs();
    AnalyticsEngine<Song> engine(allSongs);

    // Most Played Songs Table 
    auto topSongs = engine.computeMostPlayedSongs(10);
    auto playCountModel = new QStandardItemModel(static_cast<int>(topSongs.size()), 3, this);
    playCountModel->setHorizontalHeaderLabels({ "Title", "Artist", "Plays" });

    for (int i = 0; i < static_cast<int>(topSongs.size()); ++i) {
        std::string title = topSongs[i]->getTitle();
        QString titleText = title.empty() ? "No Title" : QString::fromStdString(title);
        playCountModel->setItem(i, 0, new QStandardItem(titleText));

        std::string artist = topSongs[i]->getArtist();
        QString artistText = artist.empty() ? "Unknown Artist" : QString::fromStdString(artist);
        playCountModel->setItem(i, 1, new QStandardItem(artistText));

        playCountModel->setItem(i, 2, new QStandardItem(QString::number(topSongs[i]->getPlayCount())));
    }

    ui->playCountTable->setModel(playCountModel);
    ui->playCountTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->playCountTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Update total listening time display
    updateAnalyticsDisplay();   

    // Average Song Duration
    double avgSeconds = engine.computeAverageSongDuration();
    int avgMins = static_cast<int>(avgSeconds) / 60;
    int avgSecs = static_cast<int>(avgSeconds) % 60;

    ui->songDurationLabel->setText(QString("%1:%2")
        .arg(avgMins)
        .arg(avgSecs, 2, 10, QChar('0')));

    // Populate Genre Table 
    auto genreCounts = engine.computeSongsByGenre();
    auto genreModel = new QStandardItemModel(static_cast<int>(genreCounts.size()), 2, this);
    genreModel->setHorizontalHeaderLabels({ "Genre", "Count" });

    int row = 0;
    for (const auto& [genre, count] : genreCounts) {
        genreModel->setItem(row, 0, new QStandardItem(QString::fromStdString(genre)));
        genreModel->setItem(row, 1, new QStandardItem(QString::number(count)));
        ++row;
    }

    ui->genreTable->setModel(genreModel);
    ui->genreTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->genreTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void MainWindow::updateAnalyticsDisplay() {
    // Get all songs
    const auto& allSongs = playlistManager.getMusicLibrary()->getSongs();
    AnalyticsEngine<Song> engine(allSongs);

    // Format the time
    int totalSeconds = engine.computeTotalListeningTime();
    int hours = totalSeconds / 3600;
    int minutes = (totalSeconds % 3600) / 60;
    int seconds = totalSeconds % 60;

    if (hours > 0) {
        ui->timeLabel->setText(QString("%1:%2:%3")
            .arg(hours, 2, 10, QChar('0'))
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0')));
    }
    else {
        ui->timeLabel->setText(QString("%1:%2")
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0')));
    }
}


// This function is for when a user clicks the "Delete Playlist" button in the playlist editor.
void MainWindow::onDeletePlaylistButtonClicked(QString playlistID)
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Delete Playlist",
        "Are you sure you want to delete this playlist?",
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply != QMessageBox::Yes)
    {
        return;
    }

    playlistManager.getMusicLibrary()->deletePlaylist(playlistID.toStdString());
    dataManager.saveData("data/music_library.json");

    currentPlaylistIndex = -1;
    currentPlaylistSongIndex = -1;

    loadLibraryToUI();
    ui->SongList->clear();
    ui->playlistSongsList->clear();
    ui->playlistNameLabel->setText("Playlist Name");
    ui->stackedWidget->setCurrentWidget(ui->songPlayerPage);
}

void MainWindow::updateListeningTime() {
    if (mediaControl.usePlayer()->playbackState() == QMediaPlayer::PlayingState) {
        auto currentSong = mediaControl.getCurrentSong();
        if (currentSong) {
            currentSong->addListeningTime(1);  // Add 1 second
        }

        // Update analytics display if currently visible
        if (ui->stackedWidget->currentWidget() == ui->AnalyticsPage) {
            updateAnalyticsDisplay();
        }
    }
}