/***************************************************************************
*   Copyright (C) 2009 by Roland Weigert   *
*   roweigert@t-online.de   *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/**
 *@file mainwindow.h
 *An example-mainwindow that uses the plalistwidget
 *
 *@author Roland Weigert
 *@date 27-10-2009
 */

#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include <QtGui>

#include "playlistwidgetnew.h"

class mainwindow : public QWidget
{
	Q_OBJECT
public:
	mainwindow();
private:
	QPushButton *prevbutton;
	QPushButton *nextbutton;
	QGridLayout *layout;
	playlistwidgetnew *playlist;
private slots:
	void nextslot();
	void prevslot();
	void clearslot();
	void removeactionslot(const QList<QTreeWidgetItem *> items);
	void trackcountslot(int);
};

#endif
