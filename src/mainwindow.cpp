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


MainWindow::MainWindow(MediaController &mediaControl,DataManager& dataManager, PlaylistManager& playlistManager, QWidget *parent)
    : QMainWindow(parent)
    , mediaControl(mediaControl)
    , dataManager(dataManager)
    , playlistManager(playlistManager)
    , ui(new Ui::MainWindow)

    
{

    ui->setupUi(this);
    ui->stackedWidget->setCurrentWidget(ui->songPlayerPage);

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
    connect(ui->playlistCardBox, &QListWidget::itemClicked, this, [this](QListWidgetItem* item) {                   //  This is for when a user selects a playlist from the playlist selection list.
        onPlaylistSelected(ui->playlistCardBox->row(item));
        });    

    connect(ui->addButton, &QPushButton::clicked, this, &MainWindow::onAddCurrentSongToPlaylistClicked);    //  This is for when a user clicks the "Add Current Song to Playlist" button. 
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

    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Audio File"), "", tr("MP# Files (*.mp3)"));    // Gives the menu more verbiage.

    if (fileName.isEmpty()) return;

    player->setSource(QUrl(fileName));
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
    addSongEditorInformation(newSong);

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

void MainWindow::addSongEditorInformation(std::shared_ptr<Song> song) {
    ui->lineEditSongName->setPlaceholderText(QString::fromStdString(song->getTitle()));
    ui->lineEditGenre->setPlaceholderText(QString::fromStdString(song->getGenre()));
    ui->lineEditArtist->setPlaceholderText(QString::fromStdString(song->getArtist()));
    ui->lineEditAlbum->setPlaceholderText(QString::fromStdString(song->getAlbum()));
}
// Loads the music library from the data manager into the UI. Called in main.cpp after loading the library from file.
void MainWindow::loadLibraryToUI() {
    ui->libraryList->clear();
    ui->playlistCardBox->clear();

    const auto& songs = playlistManager.getMusicLibrary()->getSongs();
    const auto& playlists = playlistManager.getMusicLibrary()->getPlaylists();

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
    }

    for (const auto& p : playlists) {
        playlistCard* card = new playlistCard(p.get(), this);
        QListWidgetItem* item = new QListWidgetItem();
        item->setSizeHint(card->sizeHint());
        ui->playlistCardBox->addItem(item);
        ui->playlistCardBox->setItemWidget(item, card);
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

}

void MainWindow::onAddPlaylistButtonClicked() {
    addPlaylistCard();
}

void MainWindow::onPlaylistSelected(int row) {
    currentPlaylistIndex = row;
	currentPlaylistSongIndex = -1;      //  Resets the current song index in the playlist editor when a new playlist is selected.
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

    ui->playlistCardBox->setCurrentRow(currentPlaylistIndex);
    loadCurrentPlaylistToUI();
    loadPlaylistEditorSongsToUI();
}

// This function is for when a user clicks on a song in the music library. 
// Current Song to Playlist, it adds the correct song.
void MainWindow::onMusicLibrarySongSelected(int row)
{
    const auto& songs = playlistManager.getMusicLibrary()->getSongs();

    if (row < 0 || row >= static_cast<int>(songs.size()))
    {
		currentSongIndex = -1;
        return;
    }

    currentSongIndex = row;
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