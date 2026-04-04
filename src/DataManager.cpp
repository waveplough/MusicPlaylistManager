#include "DataManager.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

#include <unordered_map>

DataManager::DataManager(MusicLibrary& library) : library(library)             // Initialize Reference To Library
{
}

DataManager::~DataManager()                                                    // Destructor
{
}

bool DataManager::saveData(const std::string& filename) const
{
    QFile outFile(QString::fromStdString(filename));                           // Open File For Writing

    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        return false;
    }

    QJsonObject rootObject;
    QJsonArray songsArray;
    QJsonArray playlistsArray;

    // Save Songs
    for (const auto& song : library.getSongs())
    {
        if (!song) continue;

        QJsonObject songObject;
        songObject["id"] = QString::fromStdString(song->getItemID());
        songObject["title"] = QString::fromStdString(song->getTitle());
        songObject["duration"] = song->getDuration();
        songObject["artist"] = QString::fromStdString(song->getArtist());
        songObject["album"] = QString::fromStdString(song->getAlbum());
        songObject["genre"] = QString::fromStdString(song->getGenre());
        songObject["playCount"] = song->getPlayCount();

        songsArray.append(songObject);
    }

    // Save Playlists
    for (const auto& playlist : library.getPlaylists())
    {
        if (!playlist) continue;

        QJsonObject playlistObject;
        QJsonArray playlistSongsArray;

        playlistObject["playlistID"] = QString::fromStdString(playlist->getPlaylistID());     // Save Playlist ID For Lookup During Load
        playlistObject["name"] = QString::fromStdString(playlist->getName());

        for (const auto& song : playlist->getSongs())
        {
            if (!song) continue;
            playlistSongsArray.append(QString::fromStdString(song->getItemID()));
        }

        playlistObject["songs"] = playlistSongsArray;
        playlistsArray.append(playlistObject);
    }

    rootObject["songs"] = songsArray;
    rootObject["playlists"] = playlistsArray;

    QJsonDocument doc(rootObject);
    outFile.write(doc.toJson(QJsonDocument::Indented));
    outFile.close();

    return true;
}

bool DataManager::loadData(const std::string& filename)
{
    QFile inFile(QString::fromStdString(filename));                            // Open File For Reading

    if (!inFile.open(QIODevice::ReadOnly))
    {
        return false;
    }

    QByteArray fileData = inFile.readAll();
    inFile.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(fileData, &parseError);      // Parse JSON Data

    if (parseError.error != QJsonParseError::NoError || !doc.isObject())
    {
        return false;
    }

    QJsonObject rootObject = doc.object();
    QJsonArray songsArray = rootObject["songs"].toArray();
    QJsonArray playlistsArray = rootObject["playlists"].toArray();

    // Build a quick lookup table from the library after songs are added
    std::unordered_map<std::string, std::shared_ptr<Song>> songMap;

    // Load Songs
    for (const QJsonValue& value : songsArray)
    {
        if (!value.isObject()) continue;

        QJsonObject songObject = value.toObject();

        std::string id = songObject["id"].toString().toStdString();
        std::string title = songObject["title"].toString().toStdString();
        int duration = songObject["duration"].toInt();
        std::string artist = songObject["artist"].toString().toStdString();
        std::string album = songObject["album"].toString().toStdString();
        std::string genre = songObject["genre"].toString().toStdString();

        if (id.empty() || title.empty())
        {
            continue;
        }

        std::shared_ptr<Song> song = std::make_shared<Song>(
            id, title, duration, artist, album, genre
        );

        library.addSong(song);
    }

    // Rebuild song map from the library after addSong()
    for (const auto& song : library.getSongs())
    {
        if (song)
        {
            songMap[song->getItemID()] = song;
        }
    }

    // Load Playlists
    for (const QJsonValue& value : playlistsArray)
    {
        if (!value.isObject()) continue;

        QJsonObject playlistObject = value.toObject();

        std::string playlistID = playlistObject["playlistID"].toString().toStdString();
        std::string name = playlistObject["name"].toString().toStdString();

        if (playlistID.empty() || name.empty())
        {
            continue;
        }

        library.createPlaylist(playlistID, name);

        // Find the playlist that was just created
        Playlist* createdPlaylist = nullptr;
        for (const auto& playlist : library.getPlaylists())
        {
            if (playlist && playlist->getPlaylistID() == playlistID)
            {
                createdPlaylist = playlist.get();
                break;
            }
        }

        if (!createdPlaylist)
        {
            continue;
        }

        // Add songs into this playlist using saved song IDs
        QJsonArray playlistSongsArray = playlistObject["songs"].toArray();
        for (const QJsonValue& songIdValue : playlistSongsArray)
        {
            std::string songID = songIdValue.toString().toStdString();

            auto found = songMap.find(songID);
            if (found != songMap.end())
            {
                createdPlaylist->addSong(found->second);
            }
        }
    }

    return true;
}