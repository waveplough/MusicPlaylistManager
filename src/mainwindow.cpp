#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "MediaController.h"

MainWindow::MainWindow(MediaController &mediaControl, QWidget *parent)
    : QMainWindow(parent)
    , mediaControl(mediaControl)
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
}