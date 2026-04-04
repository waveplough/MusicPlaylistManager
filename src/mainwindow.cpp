#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "MediaController.h"

MainWindow::MainWindow(MediaController &_mediaControl, QWidget *parent)
    : QMainWindow(parent)
    , mediaControl(_mediaControl)
    , ui(new Ui::MainWindow)
    
{
    ui->setupUi(this);

    // Forces the main splitter to size. Not doable in create.
    ui->mainSplitter->setSizes({ 551, 240, 240 });

    // Slots and signals manual connections
    connect(ui->actionNew_Song, &QAction::triggered, this, &MainWindow::on_actionNew_Song_triggered);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Adding a song through the file menu
void MainWindow::on_actionNew_Song_triggered() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Audio File"), "", tr("MP# Files (*.mp3)"));
}