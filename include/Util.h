#pragma once
#include <string>
#include <QUuid>

 inline std::string generateID() {
    return QUuid::createUuid().toString().toStdString();
}