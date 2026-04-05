#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include "MediaController.h"
#include "MusicLibrary.h"
#include "DataManager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(MediaController &mediaControl, DataManager& dataManager, QWidget *parent = nullptr);
    ~MainWindow() override;
    void addSongCardToLibraryList(std::shared_ptr<Song> song);
    void addPlayerInformation(std::shared_ptr<Song> song, QFileInfo fileinfo);

public slots:
    
    // Menu Items
    void onNewSongButtonClicked();  // Clicking'File' in the main menu bar and selection a 'New Song'.
    void onAnalyticsButtonClicked();
    
    // Method to add song cards. But this should be renamed to libraryListItem or something similar.
    // Song card is too similar to the others.

    // Toggles music player volume button press display.
    void onPlayerVolumeButtonClicked();

    // Toggleable music Player buttons. Ie Play, Pause - normal functionality.
    void onBackButtonClicked();
    void onStopButtonClicked();
    void onPlayButtonClicked();
    void onPauseButtonClicked();
    void onForwardButtonClicked();

    // Music Player sliders.
    void onPlayerPlaybarValueChanged(int value);
    void onPlayerVolumeSliderValueChanged(int value);

    // Playbar duration
    void durationChanged(qint64 duration);
    void updateDuration(qint64 duration);
    void positionChanged(qint64 progress);

    // Analytics Page
    void onAnalyticsExitButtonClicked();

    // Playlist Tab
    void onAddPlaylistButtonClicked();
    void onPlaylistEditorExitButtonClicked();

private:
    Ui::MainWindow *ui;

    // A reference to the mediaControl object for use here.
    MediaController &mediaControl;

    // Music Library reference pass?
    DataManager& dataManager;

    // Boolean for music Player Volume toggle.
    bool isMuted = false;

    // maximum Duration var for Player
    qint64 mDuration;
    
    int previousPageIndex;
};
#endif // MAINWINDOW_H

/* Sources:
 Button action listeners:
 Source - https://stackoverflow.com/a/74510128
 Posted by GWD, modified by community. See post 'Timeline' for change history
 Retrieved 2026-04-04, License - CC BY-SA 4.0
*/