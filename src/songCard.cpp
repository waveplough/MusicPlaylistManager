#include <QtCore>
#include <QtMultimedia>
#include <QtWidgets>
#include <qpainter.h>
#include "songCard.h"


songCard::songCard(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::songCard())
{
	ui->setupUi(this);

    // Background Image: Dynamically sized background image code.
    QLabel* ui_scSongImageLabel = findChild<QLabel*>("label_sc_songImage");
    QImage tempSCSongImage(":/new/images/assetFolder/NoImage.png");  // Load image from resources and scale it to fit the window
    QPixmap SCSongImage = songCard::Translucency(tempSCSongImage);
    ui_scSongImageLabel->setPixmap(SCSongImage.scaled(
        this->size(),
        Qt::KeepAspectRatioByExpanding, // Maintain aspect ratio
        Qt::SmoothTransformation // High-quality scaling
    ));
    connect(this, &songCard::resizeEvent, [this, ui_scSongImageLabel](QResizeEvent* event) {  // Resize image
        QImage tempSCSongImage(":/new/images/assetFolder/NoImage.png");
        QPixmap SCSongImage = songCard::Translucency(tempSCSongImage);
        ui_scSongImageLabel->setPixmap(SCSongImage.scaled(
            this->size(),
            Qt::KeepAspectRatioByExpanding,
            Qt::SmoothTransformation
        ));
        songCard::resizeEvent(event); // Call base class implementation
        });
}

songCard::~songCard()
{
	delete ui;
}

// Background Image : Translucency function for image fadeout.
QPixmap songCard::Translucency(QImage image) {
    //[2] Make a pixmap transparent for a QLabel
    QPainter p;
    p.begin(&image);
    p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    p.fillRect(image.rect(), QColor(0, 0, 0, 50));
    p.end();
    return (QPixmap::fromImage(image));
}

// [1]“How to Add a Background Image to QMainWindow in Qt Creator (Fix: Image Not Showing When Running),” codegenes, Dec. 08, 2025.
// https://www.codegenes.net/blog/how-do-i-add-a-background-image-to-the-qmainwindow/ (accessed Mar. 16, 2026).
// ‌[2]thuga, “Make a pixmap transparent for a QLabel,” Reply Comment, Stack Overflow, Sep. 12, 2013.
// https://stackoverflow.com/questions/18762774/make-a-pixmap-transparent-for-a-qlabel

