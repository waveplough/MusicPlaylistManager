#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include "MediaController.h"
#include "MusicLibrary.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(MediaController &mediaControl, MusicLibrary& library, QWidget *parent = nullptr);
    ~MainWindow() override;

public slots:
    void onActionNewSongTriggered();
    // Source - https://stackoverflow.com/a/74510128
    // Posted by GWD, modified by community. See post 'Timeline' for change history
    // Retrieved 2026-04-04, License - CC BY-SA 4.0
    void addSongCardToLibraryList(std::shared_ptr<Song> song);

private:
    Ui::MainWindow *ui;
    MusicLibrary &musicLibrary;
    MediaController &mediaControl; // What does this do?
};
#endif // MAINWINDOW_H

std::string generateSongID();   // Generates a song ID
