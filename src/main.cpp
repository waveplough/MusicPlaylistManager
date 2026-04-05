#include "mainwindow.h"
//#include "MediaController.h"

#include <QApplication>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    // Make a media controller object for broad use
    MediaController mediaControl;
    MusicLibrary musicLibrary;
    DataManager dataManager(musicLibrary);
    

    // Pass to mainwindow
    MainWindow w(mediaControl, dataManager);
	dataManager.loadData("data/music_library.json");    // Loads the music library from the JSON file. This should be done before the main window is shown, so that the library is populated when the user opens the app.
    w.loadLibraryToUI();
    w.show();
    return QCoreApplication::exec();

}
