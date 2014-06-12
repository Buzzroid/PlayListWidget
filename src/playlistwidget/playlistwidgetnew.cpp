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
#include "playlistwidgetnew.h"

playlistwidgetnew::playlistwidgetnew(QWidget *parent)    :
	QTreeWidget(parent),
	contextmenu(new QMenu(this)),
	blingtimer(new QTimer(this)),
	rescantimer(new QTimer(this)),
	scrolltimer(new QTimer(this)),
	trackcount(0),
	repeatmode(true),
	delegate(new playlistdelegate(this))
{
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setColumnCount(3);
	setIconSize(QSize(72,72));
	setColumnWidth(0,80);
	setColumnWidth(1,200);
	setColumnWidth(2,80);
	setRootIsDecorated(false);
	setAutoScroll(true);
	QStringList headerlist;
	headerlist<<""<<tr("Playlist")<<tr("Time");
	setHeaderLabels(headerlist);
	this->setAcceptDrops(true);
	blingtimer->start(100);
	rescantimer->start(2000);
	connect(blingtimer,SIGNAL(timeout()),this,SLOT(bling()));
	connect(rescantimer,SIGNAL(timeout()),this,SLOT(rescanexistingfiles()));
	connect(this,SIGNAL(itemDoubleClicked(QTreeWidgetItem *,int)),this,SLOT(doubleclicked(QTreeWidgetItem *,int)));
	connect(this,SIGNAL(itemSelectionChanged ()),this,SLOT(selectionupdate()));
	this->setItemDelegate(delegate);
}

void playlistwidgetnew::recalcall()
{
	for (int i=0; i<topLevelItemCount(); i++)
		if (!static_cast<playlistalbum *>(topLevelItem(i))->recalctime())
			delete topLevelItem(i);

	//Now check, if some albums can be merged again, because 2 albums following each others could match
	int i=1;

	while (i<topLevelItemCount())
	{
		if (static_cast<playlistalbum *>(topLevelItem(i))->artistname==static_cast<playlistalbum *>(topLevelItem(i-1))->artistname&&static_cast<playlistalbum *>(topLevelItem(i))->albumname==static_cast<playlistalbum *>(topLevelItem(i-1))->albumname)
		{
			//Merge the two albums
			topLevelItem(i-1)->addChildren(topLevelItem(i)->takeChildren());
			static_cast<playlistalbum *>(topLevelItem(i-1))->recalctime();
			delete topLevelItem(i);
		}
		else
			i++;
	}
}

void playlistwidgetnew::addsong(const QString file)
{
	QString filename=file;
	bool playedbefore=false;

	if (filename.startsWith("####ACTIVE####"))
	{
		playedbefore=true;
		filename=filename.remove("####ACTIVE####");
	}

	QFileInfo testfile(filename);

	if (testfile.exists())
	{
		playlisttrack *curtrack=new playlisttrack(filename,0);

		//Check if the lowest album seems the right one, and create a new one if not
		if (topLevelItemCount())
		{
			playlistalbum *checkalbum=static_cast<playlistalbum *> (topLevelItem(topLevelItemCount()-1));

			if (checkalbum->artistname==curtrack->artistname&&checkalbum->albumname==curtrack->albumname)
			{
				checkalbum->addChild(curtrack);
				checkalbum->recalctime();
			}
			else
			{
				playlistalbum *curalbum=new playlistalbum(curtrack,this);
				expandItem(curalbum);
				curalbum->recalctime();
			}
		}
		else
		{
			curtrack->setactive(true);
			playlistalbum *curalbum=new playlistalbum(curtrack,this);
			expandItem(curalbum);
			curalbum->setactive(true);
			curalbum->recalctime();
		}

		if (playedbefore)
		{
			playlisttrack *curactivetrack=getactivetrack();

			if (curactivetrack)
			{
				static_cast<playlistalbum *>(curactivetrack->parent())->setactive(false);
				curactivetrack->setactive(false);
			}

			curtrack->setactive(true);
		}
	}
}

void playlistwidgetnew::addsong(playlisttrack *track)
{
	//Check if the lowest album seems the right one, and create a new one if not
	if (topLevelItemCount())
	{
		playlistalbum *checkalbum=static_cast<playlistalbum *> (topLevelItem(topLevelItemCount()-1));

		if (checkalbum->artistname==track->artistname&&checkalbum->albumname==track->albumname)
		{
			checkalbum->addChild(track);
			checkalbum->recalctime();
		}
		else
		{
			playlistalbum *curalbum=new playlistalbum(track,this);
			expandItem(curalbum);
			curalbum->recalctime();
		}
	}
	else
	{
		playlistalbum *curalbum=new playlistalbum(track,this);
		expandItem(curalbum);
		curalbum->recalctime();
	}

}

void playlistwidgetnew::insertsong(const QString filename,QTreeWidgetItem *item)
{
	QFileInfo testfile(filename);

	if (testfile.exists())
	{
		playlisttrack *curtrack=new playlisttrack(filename,0);

		//Check the Item at insertionpoint
		if (item->childCount())
		{
			playlistalbum *insertionalbum=static_cast<playlistalbum *>(item);

			if (curtrack->artistname==insertionalbum->artistname&&curtrack->albumname==insertionalbum->albumname)
			{
				//Insert on top of the album, if the album is right
				insertionalbum->addChild(curtrack);
				insertionalbum->recalctime();
			}
			else if (indexOfTopLevelItem(insertionalbum)&&(curtrack->artistname==static_cast<playlistalbum *>(topLevelItem(indexOfTopLevelItem(insertionalbum)-1))->artistname&&curtrack->albumname==static_cast<playlistalbum *>(topLevelItem(indexOfTopLevelItem(insertionalbum)-1))->albumname))
			{
				//Insert on album above, if there is on, and the album is right
				static_cast<playlistalbum *>(topLevelItem(indexOfTopLevelItem(insertionalbum)-1))->addChild(curtrack);
				static_cast<playlistalbum *>(topLevelItem(indexOfTopLevelItem(insertionalbum)-1))->recalctime();
			}
			else
			{
				//Looks like we need a new album, set up above the insertionalbum
				playlistalbum *curalbum=new playlistalbum(curtrack,0);
				insertTopLevelItem(indexOfTopLevelItem(insertionalbum),curalbum);
				expandItem(curalbum);
				curalbum->addChild(curtrack);
				curalbum->recalctime();

			}
		}
		else
		{
			playlisttrack *insertiontrack=static_cast<playlisttrack *>(item);

			if (curtrack->artistname==insertiontrack->artistname&&curtrack->albumname==insertiontrack->albumname)
				insertiontrack->parent()->insertChild(insertiontrack->parent()->indexOfChild(insertiontrack),curtrack);
			//Check if the album above matches
			else if (insertiontrack->parent()->indexOfChild(insertiontrack)==0)
			{
				if (indexOfTopLevelItem(insertiontrack->parent())>0)
				{
					playlistalbum *prevalbum=static_cast<playlistalbum *> (topLevelItem(indexOfTopLevelItem(insertiontrack->parent()-1)));

					if (curtrack->artistname==prevalbum->artistname&&curtrack->albumname==prevalbum->albumname)
					{
						prevalbum->addChild(curtrack);
					}
					else
					{
						playlistalbum *curalbum=new playlistalbum(curtrack,0);
						curalbum->addChild(curtrack);
						curalbum->recalctime();
						insertTopLevelItem(indexOfTopLevelItem(insertiontrack->parent()),curalbum);
						expandItem(curalbum);
					}
				}
			}
			else
			{
				//Ok insert anywhere in between, and split up into subalbums!!!!!!!!!!
				//First create the same album again
				playlistalbum *splitalbum=new playlistalbum(insertiontrack,0);

				if ((topLevelItemCount()-1)>indexOfTopLevelItem(insertiontrack->parent()))
					insertTopLevelItem(indexOfTopLevelItem(insertiontrack->parent())+1,splitalbum);
				else
					addTopLevelItem(splitalbum);

				playlistalbum *oldalbum=static_cast<playlistalbum *>(insertiontrack->parent());
				expandItem(splitalbum);
				//Now move all the tracks in the splitted album, beginning with the insertiontrack
				int i=oldalbum->indexOfChild(insertiontrack)-1;

				while (i!=oldalbum->childCount())
				{
					QTreeWidgetItem *movetrack=oldalbum->takeChild(i);
					splitalbum->addChild(movetrack);
				}

				splitalbum->recalctime();
				//Now paste the track between the splitup albums
				playlistalbum *movealbum=new playlistalbum(curtrack,0);
				insertTopLevelItem(indexOfTopLevelItem(splitalbum),movealbum);
				movealbum->addChild(curtrack);
				movealbum->recalctime();
				expandItem(movealbum);
			}
		}
	}
}

void playlistwidgetnew::insertsong(QTreeWidgetItem *insert,QTreeWidgetItem *item)
{
	playlisttrack *curtrack=static_cast<playlisttrack *>(insert);

	//Check the Item at insertionpoint
	if (item)
	{
		if (item->childCount())
		{
			playlistalbum *insertionalbum=static_cast<playlistalbum *>(item);

			if (curtrack->artistname==insertionalbum->artistname&&curtrack->albumname==insertionalbum->albumname)
			{
				//Insert on top of the album, if the album is right
				insertionalbum->addChild(curtrack);
				insertionalbum->recalctime();
			}
			else if (indexOfTopLevelItem(insertionalbum)&&(curtrack->artistname==static_cast<playlistalbum *>(topLevelItem(indexOfTopLevelItem(insertionalbum)-1))->artistname&&curtrack->albumname==static_cast<playlistalbum *>(topLevelItem(indexOfTopLevelItem(insertionalbum)-1))->albumname))
			{
				//Insert on album above, if there is on, and the album is right
				static_cast<playlistalbum *>(topLevelItem(indexOfTopLevelItem(insertionalbum)-1))->addChild(curtrack);
				static_cast<playlistalbum *>(topLevelItem(indexOfTopLevelItem(insertionalbum)-1))->recalctime();
			}
			else
			{
				//Looks like we need a new album, set up above the insertionalbum
				playlistalbum *curalbum=new playlistalbum(curtrack,0);
				insertTopLevelItem(indexOfTopLevelItem(insertionalbum),curalbum);
				expandItem(curalbum);
				curalbum->addChild(curtrack);
				curalbum->recalctime();

			}
		}
		else
		{
			playlisttrack *insertiontrack=static_cast<playlisttrack *>(item);

			if (curtrack->artistname==insertiontrack->artistname&&curtrack->albumname==insertiontrack->albumname)
				insertiontrack->parent()->insertChild(insertiontrack->parent()->indexOfChild(insertiontrack),curtrack);
			//Check if the album above matches
			else if (insertiontrack->parent()->indexOfChild(insertiontrack)==0)
			{
				if (indexOfTopLevelItem(insertiontrack->parent())>0)
				{
					playlistalbum *prevalbum=static_cast<playlistalbum *> (topLevelItem(indexOfTopLevelItem(insertiontrack->parent()-1)));

					if (curtrack->artistname==prevalbum->artistname&&curtrack->albumname==prevalbum->albumname)
					{
						prevalbum->addChild(curtrack);
					}
					else
					{
						playlistalbum *curalbum=new playlistalbum(curtrack,0);
						curalbum->addChild(curtrack);
						curalbum->recalctime();
						insertTopLevelItem(indexOfTopLevelItem(insertiontrack->parent()),curalbum);
						expandItem(curalbum);
					}
				}
			}
			else
			{
				//Ok insert anywhere in between, and split up into subalbums!!!!!!!!!!
				//First create the same album again
				playlistalbum *splitalbum=new playlistalbum(insertiontrack,0);

				if ((topLevelItemCount()-1)>indexOfTopLevelItem(insertiontrack->parent()))
					insertTopLevelItem(indexOfTopLevelItem(insertiontrack->parent())+1,splitalbum);
				else
					addTopLevelItem(splitalbum);

				playlistalbum *oldalbum=static_cast<playlistalbum *>(insertiontrack->parent());
				expandItem(splitalbum);
				//Now move all the tracks in the splitted album, beginning with the insertiontrack
				int i=oldalbum->indexOfChild(insertiontrack)-1;

				while (i!=oldalbum->childCount())
				{
					QTreeWidgetItem *movetrack=oldalbum->takeChild(i);
					splitalbum->addChild(movetrack);
				}

				splitalbum->recalctime();
				//Now paste the track between the splitup albums
				playlistalbum *movealbum=new playlistalbum(curtrack,0);
				insertTopLevelItem(indexOfTopLevelItem(splitalbum),movealbum);
				movealbum->addChild(curtrack);
				movealbum->recalctime();
				expandItem(movealbum);
			}
		}
	}
	else
	{
		//Append on end, cause nothing was selected as insertionpoint
	}
}

const QStringList playlistwidgetnew::getplaylist()
{
	QStringList playlist;

	for (int i=0; i<topLevelItemCount(); i++)
	{
		for (int j=0; j<topLevelItem(i)->childCount(); j++)
		{
			if ((static_cast<playlisttrack *>(topLevelItem(i)->child(j))->isactive()))
				playlist<<"####ACTIVE####"+static_cast<playlisttrack *>(topLevelItem(i)->child(j))->getfile();
			else
				playlist<<static_cast<playlisttrack *>(topLevelItem(i)->child(j))->getfile();
		}
	}

	return playlist;
}

void playlistwidgetnew::checkfile(const QString &filename)
{
	qDebug("File %s is changed",qPrintable(filename));
}

void playlistwidgetnew::setrepeatmode(bool mode)
{
	repeatmode=mode;
}


void playlistwidgetnew::contextMenuEvent ( QContextMenuEvent * event )
{
	contextmenu->clear();
	storedpoint=event->pos();
	QList<QTreeWidgetItem *> selection=selectedItems () ;

	if (selection.count())
	{
		if (selection.count()>1)
		{
			QAction *removeaction=contextmenu->addAction ( QIcon(":/icons/playlistwidget/icons/edit_delete.png"),QString(tr("Remove from playlist")));
			QAction *clearaction=contextmenu->addAction ( QIcon(":/icons/playlistwidget/icons/edit_shred.png"),QString(tr("Clear playlist")));
			connect(removeaction,SIGNAL(triggered( bool )),this,SLOT(removeactionslot()));
			connect(clearaction,SIGNAL(triggered( bool )),this,SIGNAL(clearactionsignal()));
			contextmenu->popup(mapToGlobal(storedpoint));
		}
		else
		{
			QAction *playaction=contextmenu->addAction ( QIcon(":/icons/playlistwidget/icons/player_play.png"),QString(tr("Play this song")));
			QAction *removeaction=contextmenu->addAction ( QIcon(":/icons/playlistwidget/icons/edit_delete.png"),QString(tr("Remove from playlist")));
			QAction *clearaction=contextmenu->addAction ( QIcon(":/icons/playlistwidget/icons/edit_shred.png"),QString(tr("Clear playlist")));
			connect(removeaction,SIGNAL(triggered( bool )),this,SLOT(removeactionslot()));
			connect(clearaction,SIGNAL(triggered( bool )),this,SIGNAL(clearactionsignal()));
			connect(playaction,SIGNAL(triggered( bool )),this,SLOT(playactionslot()));
			contextmenu->popup(mapToGlobal(storedpoint));
		}
	}
	else
	{
		//Show contextmenu for empty selection
		if (topLevelItemCount())
		{
			QAction *clearaction=contextmenu->addAction ( QIcon(":/icons/edit_shred.png"),QString(tr("Clear playlist")));
			connect(clearaction,SIGNAL(triggered( bool )),this,SIGNAL(clearactionsignal()));
			contextmenu->popup(mapToGlobal(storedpoint));
		}
	}
}

void playlistwidgetnew::mousePressEvent(QMouseEvent *event)
{
	QTreeWidget::mousePressEvent(event);

	if (event->button() == Qt::LeftButton)
	{
		dragStartPosition = event->pos();
	}
}

void playlistwidgetnew::mouseMoveEvent(QMouseEvent *event)
{
	lastmousepos=event->pos();

	if ((event->pos()-dragStartPosition).manhattanLength()>QApplication::startDragDistance())
	{
		scrolltimer->start(100);
		connect (scrolltimer,SIGNAL(timeout()),this,SLOT(scrollcheck()));
		QList<QTreeWidgetItem *> selection=selectedItems ();
		QString filelist;

		if (selection.count()&&event->buttons()==Qt::LeftButton)
		{
			for (int i=0; i<selection.count(); i++)
			{
				if (!selection.at(i)->childCount())
					filelist+=QString::number(static_cast<playlisttrack *>(selection.at(i))->getindex())+"\n";
			}

			/*          mediafile actualfile(static_cast<playlisttrack *>(selection.at(0))->getfile());
			              QPixmap cover;
			              if (actualfile.issupportedfile())
			                {
			                  actualfile.readtags(tagversion::v2tag);
			                  if (actualfile.canhandlepicture())
			                    {
			                      const QList<coverart> coverlist=actualfile.getpictures();
			                      for (int i=0;i<coverlist.count();i++)
			                        {
			                          if (coverlist.at(i).picposition==Coverart::FrontCover)
			                            cover=QPixmap::fromImage(coverlist.at(i).image);
			                        }
			                      if (cover.isNull())
			                        {
			                          if (coverlist.count())
			                            cover=QPixmap::fromImage(coverlist.at(0).image);
			                          else
			                            cover=QPixmap(":/icons/audio.png");
			                        }
			                    }
			                  else
			                    cover=QPixmap(":/icons/audio.png");
			                }*/
			QMimeData *mimedata=new QMimeData;
			mimedata->setText(filelist);
			QDrag *drag=new QDrag(this);
			drag->setMimeData(mimedata);
			//drag->setPixmap(cover.scaled(64,64,Qt::KeepAspectRatio));
			drag->start();
		}
	}
}

void playlistwidgetnew::dragEnterEvent(QDragEnterEvent *event)
{
	/*  QStringList formatlist=event->mimeData()->formats();
	  for (int i=0;i<formatlist.count();i++)
	    qDebug(qPrintable(formatlist.at(i)));*/
	if (event->mimeData()->hasUrls()||event->mimeData()->hasText())
	{
		event->acceptProposedAction();
	}
}

void playlistwidgetnew::dropEvent(QDropEvent *event)
{
	QPoint storedpoint=event->pos();
	QTreeWidgetItem *insertionpoint=itemAt(storedpoint);

	if (event->mimeData()->hasUrls())
	{
		QStringList insertlist;
		QList<QUrl> list=event->mimeData()->urls();

		for (int i=0; i<list.count(); i++)
		{
			QString path=list.at(i).path();
			QFileInfo checkfile(path);

			if (checkfile.isDir())
			{
				insertlist<<getallfiles(path);
			}
			else if (path.toLower().endsWith("mp3")||path.toLower().endsWith("ogg")||path.toLower().endsWith("ape")||path.toLower().endsWith("flac"))
				insertlist<<path;
		}

		for (int i=0; i<insertlist.count(); i++)
		{
			if (i%10)
				qApp->processEvents();

			if (insertionpoint)
				insertsong(insertlist.at(i),insertionpoint);
			else
				addsong(insertlist.at(i));
		}
	}
	else if (event->mimeData()->hasText())
	{
		//find all files in the \n-terminated list
		QString filelist=event->mimeData()->text();
		QStringList realfiles;

		while (filelist.contains("\n"))
		{
			realfiles<<filelist.left(filelist.indexOf("\n"));
			filelist=filelist.right(filelist.length()-(filelist.indexOf("\n")+1));
		}

		QList<QTreeWidgetItem *> movelist;
		bool selfinsertion=false;

		for (int i=0; i<realfiles.count(); i++)
		{
			for (int j=0; j<topLevelItemCount()&&!selfinsertion; j++)
			{
				//Check the playlistalbum for our indexed file
				for (int k=0; k<topLevelItem(j)->childCount()&&!selfinsertion; k++)
				{
					if (static_cast<playlisttrack *>(topLevelItem(j)->child(k))->getindex()==realfiles.at(i).toInt())
					{
						QTreeWidgetItem * Item=topLevelItem(j)->child(k);

						if (Item==insertionpoint||Item->parent()==insertionpoint)
							selfinsertion=true;
					}
				}
			}
		}

		if (!selfinsertion)
		{
			for (int i=0; i<realfiles.count(); i++)
			{
				for (int j=0; j<topLevelItemCount(); j++)
				{
					for (int k=0; k<topLevelItem(j)->childCount(); k++)
					{
						if (static_cast<playlisttrack *>(topLevelItem(j)->child(k))->getindex()==realfiles.at(i).toInt())
						{
							QTreeWidgetItem * Item=topLevelItem(j)->takeChild(k);
							movelist<<Item;
						}
					}
				}
			}

			if (insertionpoint)
			{
				if (!selfinsertion)
				{
					for (int i=0; i<movelist.count(); i++)
						insertsong(movelist.at(i),insertionpoint);
				}
			}
			else
			{
				for (int i=0; i<movelist.count(); i++)
					addsong(static_cast<playlisttrack *>(movelist.at(i)));
			}
		}
	}

	// Ok all done, clear out all albums, that seem empty
	for (int i=0; i<topLevelItemCount(); i++)
		if (!static_cast<playlistalbum *>(topLevelItem(i))->recalctime())
			delete topLevelItem(i);

	//Now check, if some albums can be merged again, because 2 albums following each others could match
	int i=1;

	while (i<topLevelItemCount())
	{
		if (static_cast<playlistalbum *>(topLevelItem(i))->artistname==static_cast<playlistalbum *>(topLevelItem(i-1))->artistname&&static_cast<playlistalbum *>(topLevelItem(i))->albumname==static_cast<playlistalbum *>(topLevelItem(i-1))->albumname)
		{
			//Merge the two albums
			topLevelItem(i-1)->addChildren(topLevelItem(i)->takeChildren());
			static_cast<playlistalbum *>(topLevelItem(i-1))->recalctime();
			delete topLevelItem(i);
		}
		else
			i++;
	}
}

void playlistwidgetnew::dragMoveEvent(QDragMoveEvent *event)
{
	lastmousepos=event->pos();
}

void playlistwidgetnew::scrollcheck()
{
	if (lastmousepos.y()<100&&verticalScrollBar()->value()>0)
		scroll(-((100-lastmousepos.y())/20));
	else if (lastmousepos.y()>height()-100&&verticalScrollBar()->value()<verticalScrollBar()->maximum())
		scroll((100-(height()-lastmousepos.y()))/20);

	if (QApplication::mouseButtons()==Qt::NoButton)
		scrolltimer->stop();
}

void playlistwidgetnew::scroll(int direction)
{
	verticalScrollBar()->setValue(verticalScrollBar()->value()+direction);
}

void playlistwidgetnew::playactionslot()
{
	//Its the easiest way to use this existing signal
	emit itemDoubleClicked(currentItem(),0);
}

void playlistwidgetnew::removeactionslot()
{
	QList<QTreeWidgetItem *> selection=selectedItems ();
	QList<QTreeWidgetItem *> trackselection;

	//Pick out all tracks, and leave the albums where they are. They get deleted by the next recalcrun
	for (int i=0; i<selection.count(); i++)
		if (!selection.at(i)->childCount())
			trackselection.append(selection.at(i));

	emit removeactionsignal(trackselection);
}

void playlistwidgetnew::bling()
{
	int newcount=0;
	int timeall=0;

	for (int i=0; i<topLevelItemCount(); i++)
	{
		newcount+=topLevelItem(i)->childCount();
		timeall+=static_cast<playlistalbum *>(topLevelItem(i))->recalctime();

		if (static_cast<playlistalbum *>(topLevelItem(i))->isplaying)
			static_cast<playlistalbum *>(topLevelItem(i))->bling();
	}

	if (newcount!=trackcount)
	{
		emit trackcountchange(newcount);
		int seconds=0;
		int minutes=0;
		int hours=0;
		hours=timeall/3600;
		minutes=(timeall-hours*3600)/60;
		seconds=timeall-hours*3600-minutes*60;
		QString hourstring,minutestring,secondsstring;

		if (seconds<10)
			secondsstring="0"+QString::number(seconds);
		else
			secondsstring=QString::number(seconds);

		if (minutes<10)
			minutestring="0"+QString::number(minutes);
		else
			minutestring=QString::number(minutes);

		if (hours<10)
			hourstring="0"+QString::number(hours);
		else
			hourstring=QString::number(hours);

		QString timestring=QString("%1:%2:%3").arg(hourstring,minutestring,secondsstring);


		QStringList headerlist;
		headerlist<<QString ("%1 Tracks").arg(QString::number(newcount))<<tr("Playlist")<<timestring;
		setHeaderLabels(headerlist);
	}

	trackcount=newcount;
}

void playlistwidgetnew::selectionupdate()
{
	disconnect(this,SIGNAL(itemSelectionChanged ()),this,SLOT(selectionupdate()));
	//Select all tracks of selected albums
	QList<QTreeWidgetItem *> selection=selectedItems ();

	for (int i=0; i<selection.count(); i++)
	{
		if (selection.at(i)->childCount())
		{
			for (int j=0; j<selection.at(i)->childCount(); j++)
				selection.at(i)->child(j)->setSelected(true);
		}
	}

	connect(this,SIGNAL(itemSelectionChanged ()),this,SLOT(selectionupdate()));
}

const playlisttrack * playlistwidgetnew::skiptonext()
{
	playlisttrack *actualtrack=NULL;
	playlisttrack *nextfile=NULL;

	for (int i=0; i<topLevelItemCount(); i++)
	{
		if (static_cast<playlistalbum *>(topLevelItem(i))->isplaying)
		{
			for (int j=0; j<topLevelItem(i)->childCount(); j++)
			{
				actualtrack=static_cast<playlisttrack *>(topLevelItem(i)->child(j));

				if (actualtrack->isplaying)
				{
					actualtrack->setactive(false);

					if (j<topLevelItem(i)->childCount()-1)
					{
						static_cast<playlisttrack *>(topLevelItem(i)->child(j+1))->setactive(true);
						nextfile=static_cast<playlisttrack *>(topLevelItem(i)->child(j+1));
						break;
					}
					else
					{
						if (i<topLevelItemCount()-1)
						{
							//select the first track of the next album
							static_cast<playlistalbum *>(topLevelItem(i))->setactive(false);
							static_cast<playlistalbum *>(topLevelItem(i+1))->setactive(true);
							nextfile=static_cast<playlisttrack *>(topLevelItem(i+1)->child(0));
							nextfile->setactive(true);
							break;
						}
						else if (repeatmode)
						{
							//select the first album
							static_cast<playlistalbum *>(topLevelItem(i))->setactive(false);
							static_cast<playlistalbum *>(topLevelItem(0))->setactive(true);
							nextfile=static_cast<playlisttrack *>(topLevelItem(0)->child(0));
							nextfile->setactive(true);
							break;
						}
						else
						{
							nextfile=actualtrack;
							nextfile->setactive(true);
							break;
						}
					}
				}
			}
		}

		if (nextfile)
			break;
	}

	if (nextfile==actualtrack)
		nextfile=NULL;

	return nextfile;
}

const playlisttrack * playlistwidgetnew::skiptoprev()
{
	playlisttrack *actualtrack=NULL;
	playlisttrack *prevfile=NULL;

	for (int i=0; i<topLevelItemCount(); i++)
	{
		if (static_cast<playlistalbum *>(topLevelItem(i))->isplaying)
		{
			for (int j=topLevelItem(i)->childCount()-1; j>=0; j--)
			{
				actualtrack=static_cast<playlisttrack *>(topLevelItem(i)->child(j));

				if (actualtrack->isplaying)
				{
					actualtrack->setactive(false);

					if (j>0)
					{
						static_cast<playlisttrack *>(topLevelItem(i)->child(j-1))->setactive(true);
						prevfile=static_cast<playlisttrack *>(topLevelItem(i)->child(j-1));
						break;
					}
					else
					{
						if (i>0)
						{
							//select the first track of the next album
							static_cast<playlistalbum *>(topLevelItem(i))->setactive(false);
							static_cast<playlistalbum *>(topLevelItem(i-1))->setactive(true);
							prevfile=static_cast<playlisttrack *>(topLevelItem(i-1)->child(topLevelItem(i-1)->childCount()-1));
							prevfile->setactive(true);
							break;
						}
						else if (repeatmode)
						{
							//select the last album
							static_cast<playlistalbum *>(topLevelItem(i))->setactive(false);
							static_cast<playlistalbum *>(topLevelItem(topLevelItemCount()-1))->setactive(true);
							prevfile=static_cast<playlisttrack *>(topLevelItem(topLevelItemCount()-1)->child(topLevelItem(topLevelItemCount()-1)->childCount()-1));
							prevfile->setactive(true);
							break;
						}
						else
						{
							prevfile=actualtrack;
							prevfile->setactive(true);
							break;
						}
					}
				}
			}
		}

		if (prevfile)
			break;
	}

	if (prevfile==actualtrack)
		prevfile=NULL;

	return prevfile;
}

playlisttrack *  playlistwidgetnew::getactivetrack()
{
	playlisttrack *activefile=NULL;

	for (int i=0; i<topLevelItemCount(); i++)
	{
		if (static_cast<playlistalbum *>(topLevelItem(i))->isplaying)
		{
			for (int j=topLevelItem(i)->childCount()-1; j>=0; j--)
			{
				if (static_cast<playlisttrack *>(topLevelItem(i)->child(j))->isplaying)
				{
					activefile=static_cast<playlisttrack *>(topLevelItem(i)->child(j));
					break;
				}
			}
		}

		if (activefile)
			break;
	}

	return activefile;
}

void playlistwidgetnew::doubleclicked(QTreeWidgetItem *item,int row)
{
	playlisttrack *track=0;

	if (item->childCount()&&row)
		track=static_cast<playlisttrack *>(item->child(0));
	else if (!item->childCount())
		track=static_cast<playlisttrack *>(item);

	if (track)
	{
		for (int i=0; i<topLevelItemCount(); i++)
		{
			if (static_cast<playlistalbum *>(topLevelItem(i))->isplaying)
			{
				static_cast<playlistalbum *>(topLevelItem(i))->setactive(false);

				for (int j=topLevelItem(i)->childCount()-1; j>=0; j--)
				{
					if (static_cast<playlisttrack *>(topLevelItem(i)->child(j))->isplaying)
					{
						static_cast<playlisttrack *>(topLevelItem(i)->child(j))->setactive(false);
						break;
					}
				}
			}
		}

		track->setactive(true);
		static_cast<playlistalbum *>(track->parent())->setactive(true);
		emit(actualchange(track));
	}
	else
		qDebug("No emit");

}

QStringList playlistwidgetnew::getallfiles(QString subdir)
{
	QStringList files;
	QDir readindir(subdir);
	QStringList dirresult=readindir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);

	for (int i=0; i< dirresult.count(); i++)
	{
		if (subdir.endsWith("/"))
			files<<getallfiles(QString("%1%2").arg(subdir,dirresult.at(i)));
		else
			files<<getallfiles(QString("%1/%2").arg(subdir,dirresult.at(i)));
	}

	QStringList extensionfilters;
	extensionfilters<<"*.mp3"<<"*.ogg";//<<"*.flac"<<"*.ape";
	readindir.setNameFilters(extensionfilters);
	dirresult=readindir.entryList(QDir::Files|QDir::NoDotAndDotDot);

	for (int i=0; i< dirresult.count(); i++)
	{
		if (subdir.endsWith("/"))
			files<<(QString("%1%2").arg(subdir,dirresult.at(i)));
		else
			files<<(QString("%1/%2").arg(subdir,dirresult.at(i)));
	}

	return files;
}

void playlistwidgetnew::rescanexistingfiles()
{
	disconnect(blingtimer,SIGNAL(timeout()),this,SLOT(bling()));
	disconnect(rescantimer,SIGNAL(timeout()),this,SLOT(rescanexistingfiles()));
	bool changedone=FALSE;
	int i=0;

	while (i<topLevelItemCount())
	{
		int j=0;
		int count=topLevelItemCount();

		while (j<topLevelItem(i)->childCount())
		{
			QTreeWidgetItem *insertionpoint=0;
			playlisttrack *actualtrack=static_cast<playlisttrack *>(topLevelItem(i)->child(j));
			QFileInfo checkfile(actualtrack->filename);

			if (checkfile.exists())
			{
				if (actualtrack->modifieddate!=checkfile.lastModified())
				{
					if (actualtrack->isactive())
						emit (actualfilechangedtags());

					changedone=TRUE;

					if (topLevelItem(i)->childCount()==1)
					{
						//We can not take the parent as insertionpoint, because it will be deletet, when we take our item
						if (i>0)
							insertionpoint=topLevelItem(i-1);

						if (i==0&&topLevelItemCount()>1)
							insertionpoint=topLevelItem(i+1);
					}
					else
					{
						if (j==0)
						{
							insertionpoint=actualtrack->parent();
						}
						else if (j==(topLevelItem(i)->childCount()-1))
						{
							if (i<(topLevelItemCount()-1))
								insertionpoint=topLevelItem(i+1);
							else
								insertionpoint=NULL;
						}
						else
						{
							insertionpoint=topLevelItem(i)->child(j+1);
						}
					}

					//Take out the item,modify it and insert it on the same position again
					actualtrack->reread();
					topLevelItem(i)->takeChild(j);

					if (insertionpoint)
					{
						insertsong(actualtrack,insertionpoint);
						static_cast<playlistalbum *>(actualtrack->parent())->repaintcover(actualtrack);
					}
					else
						addsong(actualtrack);
				}
				else
					j++;
			}
			else
			{
				changedone=TRUE;
				QList<QTreeWidgetItem *> deletefile;
				deletefile<<actualtrack;
				emit removeactionsignal(deletefile);
			}

			//Pop out of loop, if the last album is deleted
			if (i>=topLevelItemCount())
				break;
		}

		//If the number of albums did not change in the meanwhile
		if (count==topLevelItemCount())
			i++;
	}

	if (changedone)
		recalcall();

	connect(rescantimer,SIGNAL(timeout()),this,SLOT(rescanexistingfiles()));
	connect(blingtimer,SIGNAL(timeout()),this,SLOT(bling()));
}

//############################################################################
//Playlistalbum-Part

playlistalbum::playlistalbum(playlisttrack *track,QTreeWidget *parent):
	QTreeWidgetItem(parent),
	length(0),
	albumname(track->albumname),
	artistname(track->artistname),
	albumart(track->albumart),
	isplaying(false),
	blingindex(0),
	reverse(0)
{
	setIcon(0,albumart);
	this->setText(1,QString("%1<br>%2").arg(artistname,albumname));
	this->setFont(1,QFont("Serif",-1,QFont::Bold));
	addChild(track);

	for (int i=0; i<4; i++)
	{
		QLinearGradient gradient(QPointF(0, 0), QPointF(0, 32));
		gradient.setColorAt(0, QColor(100,100,100));
		gradient.setColorAt(1, QColor(240,240,240));
		gradient.setSpread(QGradient::ReflectSpread);
		QBrush brush(gradient);
		this->setBackground(i,brush);
	}
}

void playlistalbum::repaintcover(playlisttrack *changedtrack)
{
	albumart=changedtrack->albumart;
	setIcon(0,albumart);
}

int playlistalbum::recalctime()
{
	length=0;

	for (int i=0; i<childCount(); i++)
	{
		length+=static_cast<playlisttrack *>(child(i))->length;

		if (!(static_cast<playlisttrack *>(child(i))->isplaying))
		{
			if (i%2)
			{
				for (int j=0; j<4; j++)
					child(i)->setBackground(j,QBrush(Qt::lightGray));
			}
			else
			{
				for (int j=0; j<4; j++)
					child(i)->setBackground(j,QBrush(Qt::white));
			}
		}
		else
			setactive(true);//Set this album active, since a track is in, that is active
	}

	int seconds=0;
	int minutes=0;
	minutes=length/60;
	seconds=length-minutes*60;
	QString minutestring,secondsstring;

	if (seconds<10)
		secondsstring="0"+QString::number(seconds);
	else
		secondsstring=QString::number(seconds);

	if (minutes<10)
		minutestring="0"+QString::number(minutes);
	else
		minutestring=QString::number(minutes);

	QString timestring=QString("%1 : %2").arg(minutestring,secondsstring);
	this->setText(2,timestring);
	return length;
}

void playlistalbum::setactive(bool active)
{
	if (active)
		isplaying=true;
	else
		for (int i=0; i<4; i++)
		{
			isplaying=false;
			QLinearGradient gradient(QPointF(0, 0), QPointF(0, 32));
			gradient.setColorAt(0, QColor(100,100,100));
			gradient.setColorAt(1, QColor(240,240,240));
			gradient.setSpread(QGradient::ReflectSpread);
			QBrush brush(gradient);
			this->setBackground(i,brush);
		}
}

void playlistalbum::bling()
{
	if (reverse)
		blingindex-=4;
	else
		blingindex+=4;

	if (blingindex>80)
		reverse=true;

	if (blingindex<4)
		reverse=false;

	QLinearGradient gradient(QPointF(0, 0), QPointF(0, 32));
	gradient.setColorAt(0, QColor(50+blingindex,80+blingindex,80+blingindex));
	gradient.setColorAt(1, QColor(200,240,255));
	gradient.setSpread(QGradient::ReflectSpread);
	QBrush brush(gradient);

	for (int i=0; i<4; i++)
	{
		this->setBackground(i,brush);
	}

	for (int j=0; j<childCount(); j++)
		if (static_cast<playlisttrack *>(child(j))->isplaying)
			static_cast<playlisttrack *>(child(j))->bling();
}

//############################################################################
//Playlisttrack-Part

long playlisttrack::runningindex=0;

playlisttrack::playlisttrack(const QString file, QTreeWidget *parent):
	QTreeWidgetItem(parent,QTreeWidgetItem::UserType),
	filename(file),
	track(0),
	length(0),
	viewindex(runningindex++),
	isplaying(false),
	blingindex(0),
	reverse(false)
{
	mediafile actualfile(filename);

	if (actualfile.issupportedfile())
	{
		QFileInfo checkfile(filename);
		modifieddate=checkfile.lastModified();
		this->setTextAlignment(0,Qt::AlignRight);
		actualfile.readtags(tagversion::v2tag);
		artistname=actualfile.getartistname();
		albumname=actualfile.getalbumname();
		songname=actualfile.getsongname();
		track=actualfile.gettracknumber();
		length=actualfile.getlength();

        if (artistname.isNull()) {
			artistname=QString("Unknown");
        }

        if (albumname.isNull()) {
			albumname=QString("Unknown");
            albumname = checkfile.dir().absolutePath();
        }

        if (songname.isNull()) {
			songname=QString("Unknown");
            songname = checkfile.baseName();
        }

		this->setText(0,QString("%1.").arg(QString::number(track)));
		this->setText(1,QString("%1").arg(songname));
		int seconds=0;
		int minutes=0;
		minutes=length/60;
		seconds=length-minutes*60;
		QString minutestring,secondsstring;

		if (seconds<10)
			secondsstring="0"+QString::number(seconds);
		else
			secondsstring=QString::number(seconds);

		if (minutes<10)
			minutestring="0"+QString::number(minutes);
		else
			minutestring=QString::number(minutes);

		QString timestring=QString("%1 : %2").arg(minutestring,secondsstring);
		this->setText(2,timestring);

		if (actualfile.canhandlepicture())
		{
			const QList<coverart> coverlist=actualfile.getpictures();

			for (int i=0; i<coverlist.count(); i++)
			{
				if (coverlist.at(i).picposition==Coverart::FrontCover)
					albumart=QIcon(QPixmap::fromImage(coverlist.at(i).image));
			}

			if (albumart.isNull())
			{
				if (coverlist.count())
					albumart=QIcon(QPixmap::fromImage(coverlist.at(0).image));
                else {
                    if (artistname == "Unknown")
                        albumart=QIcon(":/icons/playlistwidget/icons/Folders-OS-Windows-8-Metro-icon.png");
                    else
                        albumart=QIcon(":/icons/playlistwidget/icons/audio.png");
                }
			}
		}
		else
			albumart=QIcon(":/icons/playlistwidget/icons/audio.png");
	}
	else
	{
		qDebug("File not supported or deleted.Setting filename to QString::null to notify caller");
		filename=QString::null;
	}
}

void playlisttrack::reread()
{
	mediafile actualfile(filename);

	if (actualfile.issupportedfile())
	{
		QFileInfo checkfile(filename);
		modifieddate=checkfile.lastModified();
		actualfile.readtags(tagversion::v2tag);
		artistname=actualfile.getartistname();
		albumname=actualfile.getalbumname();
		songname=actualfile.getsongname();
		track=actualfile.gettracknumber();
		length=actualfile.getlength();

		if (artistname.isNull())
			artistname=QString("Unknown");

		if (albumname.isNull())
			albumname=QString("Unknown");

		if (songname.isNull())
			songname=QString("Unknown");

		this->setText(0,QString("%1.").arg(QString::number(track)));
		this->setText(1,QString("%1").arg(songname));
		int seconds=0;
		int minutes=0;
		minutes=length/60;
		seconds=length-minutes*60;
		QString minutestring,secondsstring;

		if (seconds<10)
			secondsstring="0"+QString::number(seconds);
		else
			secondsstring=QString::number(seconds);

		if (minutes<10)
			minutestring="0"+QString::number(minutes);
		else
			minutestring=QString::number(minutes);

		QString timestring=QString("%1 : %2").arg(minutestring,secondsstring);
		this->setText(2,timestring);

		if (actualfile.canhandlepicture())
		{
			const QList<coverart> coverlist=actualfile.getpictures();

			for (int i=0; i<coverlist.count(); i++)
			{
				if (coverlist.at(i).picposition==Coverart::FrontCover)
					albumart=QIcon(QPixmap::fromImage(coverlist.at(i).image));
			}

			if (albumart.isNull())
			{
				if (coverlist.count())
					albumart=QIcon(QPixmap::fromImage(coverlist.at(0).image));
				else
					albumart=QIcon(":/icons/playlistwidget/icons/audio.png");
			}
		}
		else
			albumart=QIcon(":/icons/playlistwidget/icons/audio.png");
	}
	else
	{
		qDebug("File not supported or deleted.Setting filename to QString::null to notify caller");
		filename=QString::null;
	}
}

void playlisttrack::setactive(bool active)
{
	if (active)
		isplaying=true;
	else
	{
		isplaying=false;

		if (parent()->indexOfChild(this)%2)
		{
			for (int j=0; j<4; j++)
				setBackground(j,QBrush(Qt::lightGray));
		}
		else
		{
			for (int j=0; j<4; j++)
				setBackground(j,QBrush(Qt::white));
		}
	}
}

void playlisttrack::bling()
{
	if (reverse)
		blingindex-=4;
	else
		blingindex+=4;

	if (blingindex>80)
		reverse=true;

	if (blingindex<4)
		reverse=false;

	QLinearGradient gradient(QPointF(0, 0), QPointF(0, 10));
	gradient.setColorAt(1, QColor(100+blingindex,130+blingindex,130+blingindex));
	gradient.setColorAt(0, QColor(200,240,255));
	gradient.setSpread(QGradient::ReflectSpread);
	QBrush brush(gradient);

	for (int i=0; i<4; i++)
	{
		this->setBackground(i,brush);
	}

	for (int j=0; j<childCount(); j++)
		if (static_cast<playlisttrack *>(child(j))->isplaying)
			static_cast<playlisttrack *>(child(j))->bling();
}

playlistdelegate::playlistdelegate(QObject *parent):QItemDelegate(parent)
{
}

void playlistdelegate::paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
	QString text = index.data().toString();

	if (text.contains("<br>"))
	{
		this->drawBackground (  painter, option,index );
		QRectF artistrect=option.rect;
		artistrect.setTop(option.rect.y()+(option.rect.height()*0.1));
		artistrect.setBottom(option.rect.y()+(option.rect.height()*0.6));
		QRectF albumrect=option.rect;
		albumrect.setTop(artistrect.bottom());
		QStringList list=text.split("<br>");
		painter->setFont(QFont("Serif",14,QFont::Black,0));
		bool clipped=FALSE;

		while(painter->boundingRect(artistrect,Qt::TextSingleLine,list.at(0)).width()+20>artistrect.width()&!list.at(0).isEmpty())
		{
			clipped=TRUE;
			list.replace(0,list.at(0).left(list.at(0).length()-1));
		}

		QRectF boundalbum;
		painter->drawText(artistrect,list.at(0));

		if (clipped)
			painter->drawText(artistrect,Qt::AlignRight,"...");

		painter->setFont(QFont("Serif",12,QFont::Bold,1));
		clipped=FALSE;

		while(painter->boundingRect(albumrect,Qt::TextSingleLine,list.at(1)).width()+20>albumrect.width()&!list.at(1).isEmpty())
		{
			clipped=TRUE;
			list.replace(1,list.at(1).left(list.at(1).length()-1));
		}

		painter->drawText(albumrect,list.at(1));

		if (clipped)
			painter->drawText(albumrect,Qt::AlignRight,"...");
	}
	else
		QItemDelegate::paint(painter, option, index);
}
