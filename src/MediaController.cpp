#include "MediaController.h"

// Media controller class is instantiated for use program-wide.
// QMediaPlayeri intiialized in class object constructor
MediaController::MediaController(QObject* parent)
	: QObject(parent)
	, trueMediaPlayer(new QMediaPlayer(this))
	, audioOutput(new QAudioOutput(this))
{ 
	// Signals go here, surprisingly

	// Linking operations
	trueMediaPlayer->setAudioOutput(audioOutput);
}

MediaController::~MediaController()
{}

