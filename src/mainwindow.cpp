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
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Adding a song through the file menu
void MainWindow::onActionNewSongTriggered() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Audio File"), "", tr("MP# Files (*.mp3)"));

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

        
        std::string title = metaData.stringValue(QMediaMetaData::Title).toStdString();
        std::string artist = metaData.stringValue(QMediaMetaData::AlbumArtist).toStdString();
        std::string album = metaData.stringValue(QMediaMetaData::AlbumTitle).toStdString();
        std::string genre = metaData.stringValue(QMediaMetaData::Genre).toStdString();
        int duration = mediaControl.usePlayer()->duration() / 1000;

        std::string songId = generateSongID();  // Generate a song ID

        if (artist.empty()) {
            artist = "Unknown Artist";
        }

        if (title.empty()){
            title = QFileInfo(fileName).baseName().toStdString();
        }

        if (album.empty()) {
            album = "N/A";
        }

        std::shared_ptr<Song> newSong = std::make_shared<Song> (Song(songId, title, duration, artist, album, genre));
        
        musicLibrary.addSong(std::shared_ptr<Song>(newSong));
        addSongCardToLibraryList(newSong);

    }

    
}

void MainWindow::addSongCardToLibraryList(std::shared_ptr<Song> song) {
    LibraryCard *card = new LibraryCard(song, this);
    QListWidgetItem* item = new QListWidgetItem();
    item->setSizeHint(card->sizeHint());
    ui->libraryList->addItem(item);
    ui->libraryList->setItemWidget(item, card);
}




std::string generateSongID() {
    return QUuid::createUuid().toString().toStdString();
}