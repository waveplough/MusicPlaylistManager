#include "PlaylistManager.h"
#include <QtWidgets/QApplication>
#include <iostream>

int main(int argc, char *argv[])
{

    QApplication app(argc, argv);
    PlaylistManager window;
    window.show();
    return app.exec();

}
