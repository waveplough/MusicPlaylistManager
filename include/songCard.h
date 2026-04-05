#pragma once

#ifndef SONGCARD_H
#define SONGCARD_H

#include <QWidget>
//#include "songCard.ui"

QT_BEGIN_NAMESPACE
namespace Ui { class songCard; };
QT_END_NAMESPACE

class songCard : public QWidget
{
	Q_OBJECT

public:
	explicit songCard(QWidget *parent = nullptr);
	~songCard();

private:
	Ui::songCard *ui;
	QPixmap Translucency(QImage image);
};

#endif // SONGCARD_H