#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include "MediaController.h"
#include "MusicLibrary.h"
#include "DataManager.h"
#include "PlaylistManager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(MediaController &mediaControl,DataManager& dataManager, PlaylistManager& playlistManager, QWidget *parent = nullptr);
    ~MainWindow() override;

    void loadLibraryToUI();
    void addSongCardToLibraryList(std::shared_ptr<Song> song);
    void addPlayerInformation(std::shared_ptr<Song> song, QFileInfo fileinfo);

public slots:
    
    // Menu Items
    void onNewSongButtonClicked();  // Clicking'File' in the main menu bar and selection a 'New Song'.
    void onAnalyticsButtonClicked();
    void onImportM3UButtonClicked();
    
    // Method to add song cards. But this should be renamed to libraryListItem or something similar.
    // Song card is too similar to the others.

    // Toggles music player volume button press display.
    void onPlayerVolumeButtonClicked();

    // Toggleable music Player buttons. Ie Play, Pause - normal functionality.
    void onBackButtonClicked();
    //void onBackButtonPressed();       // UNUSED
    void onStopButtonClicked();
    void onPlayButtonClicked();
    void onPauseButtonClicked();
    void onForwardButtonClicked();
    //void onForwardButtonPressed();    // UNUSED

    // Music Player sliders.
    void onPlayerPlaybarValueChanged(int value);
    void onPlayerPlaybarMoved(int value);
    void onPlayerVolumeSliderValueChanged(int value);

    // Playbar duration
    void durationChanged(qint64 duration);
    void updateDuration(qint64 duration);
    void positionChanged(qint64 progress);

    // Analytics Page
    void onAnalyticsExitButtonClicked();
    void updateListeningTime();
    void updateAnalyticsDisplay();

    // Playlist Tab
    void addPlaylistCard();
    void onAddPlaylistButtonClicked();
    void onPlaylistEditorExitButtonClicked();
    void onPlaylistSelected(int row);

    // Current Playlist (songs) Tab
    void onSongCardDoubleClicked(std::shared_ptr<Song> song);

    // Playlist Editor Buttons
    void onAddCurrentSongToPlaylistClicked();
    void loadCurrentPlaylistToUI();
    void loadPlaylistEditorSongsToUI();
    void onPlaylistEditorSongSelected(int row);
    void onMoveUpClicked();
    void onMoveDownClicked();
    void onReorderClicked();
    void onTrashButtonClicked();
    

    // Music Library
    void onMusicLibrarySongSelected(int row);
    void onSearchTextChanged(const QString& text);

    // Song Editor

    void addSongEditorInformation(std::shared_ptr<Song> song);
    void onSongEditorSubmitButtonClicked();
    void onSongEditorDeleteButtonClicked();
    void addSongCardToSongsList(std::shared_ptr<Song> song);

protected:
    void closeEvent(QCloseEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    Ui::MainWindow *ui;

    // A reference to the mediaControl object for use here.
    MediaController &mediaControl;

    // Music Library reference pass?
    PlaylistManager& playlistManager;
    DataManager& dataManager;

    // current playlist index for playlist functionality. -1 means no playlist is currently selected.
    int currentPlaylistIndex = -1;

    // current song index for playlist functionality. - 1 means no song is currently selected.
    int currentSongIndex = -1;

	//` current song index for playlist editor functionality. -1 means no song is currently selected.
    int currentPlaylistSongIndex = -1;

    // Boolean for music Player Volume toggle.
    bool isMuted = false;

    // maximum Duration var for Player
    qint64 mDuration;
    
    int previousPageIndex;

	void refreshPlaylistViewsAndKeepSelection(int row); //  A helper function to refresh the playlist views while keeping the current selection.

    void onDeletePlaylistButtonClicked(QString playlistID);

	// This vector holds the current search results for the music library. 
    // It is used to keep track of which songs are currently being displayed in the library list, 
    // so that when a user clicks on a song, we can get the correct song object from this vector.
    std::vector<std::shared_ptr<Song>> currentSearchResults;

    QTimer* listeningTimer = nullptr;

};
#endif // MAINWINDOW_H

/* Sources:
 Button action listeners:
 Source - https://stackoverflow.com/a/74510128
 Posted by GWD, modified by community. See post 'Timeline' for change history
 Retrieved 2026-04-04, License - CC BY-SA 4.0
*/