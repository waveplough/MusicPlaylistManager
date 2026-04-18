#include "MediaController.h"

/**
 * Initializes the media player and audio output components
 *
 * @param parent Parent QObject (main window)
 *
 * The MediaController is instantiated once at program startup and provides
 * a global interface for audio playback throughout the application.
 */
MediaController::MediaController(QObject* parent)
	: QObject(parent)
	, trueMediaPlayer(new QMediaPlayer(this))
	, audioOutput(new QAudioOutput(this))
{ 
	// Signals go here, surprisingly

	// Linking operations
	trueMediaPlayer->setAudioOutput(audioOutput);
}

/**
 * Cleans up the media player and audio output
 */
MediaController::~MediaController()
{}

