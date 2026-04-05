#include "mainwindow.h"
//#include "MediaController.h"

#include <QApplication>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    // Make a media controller object for broad use
    MediaController mediaControl;
    MusicLibrary musicLibrary;

    // Pass to mainwindow
    MainWindow w(mediaControl,musicLibrary);
    w.show();
    return QCoreApplication::exec();

}
