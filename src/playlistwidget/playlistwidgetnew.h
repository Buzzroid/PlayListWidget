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
 *@file playlistwidgetnew.h
 *The file that has all classes for the playlist.(playlistwidgetnew,playlistalbum,playlisttrack)<br>
 *
 *@author Roland Weigert
 *@date 17-11-2009
 */

/**
 * If the watcher brings memorywarnings, you have to start this command as root:<br>
 * echo 16384 > /proc/sys/fs/inotify/max_user_watches
 * This enables more watchable files in the kernel.
 */

#ifndef _PLAYLISTWIDGETNEW_H_
#define _PLAYLISTWIDGETNEW_H_

#include <QtGui>

/**
 *@brief Include for Tagengine
 * This include is needed, because there must be some sort of tagengine present, that does all tagreading.
 */
#include "mediafile.h"


//prototypes
class playlistalbum;
class playlisttrack;
class playlistdelegate;

/**
 *@brief The playlistwidgetnew-class itself
 * This widget organises a playlist, with all functionality needed for a playlist.<br>
 * It automatically groups all playlisttracks into playlistalbums, and reorganises them on drag and drop.<br>
 * Also all tracks get actualised automatically, when the files change on disc:<br>
 * -Regrouping in new albums, if the album changes.<br>
 * -Update the Coverpicture of playlistalbum.<br>
 * -Remove of Tracks from the playlist, if file got deleted on disc.<br>
 */
class playlistwidgetnew :public QTreeWidget
{
	Q_OBJECT
public:
	/**
	   *@brief Constructor
	   *Sets up the widget, and activates blingtimer, and rescantimer.<br>
	   *@param parent The parent-widget
	   */
	playlistwidgetnew(QWidget *parent);
	/**
	   *@brief addsong-function
	   *Adds a song on the eend of the playlist. It automatically creates a new album, if the track does not match with the last album.<br>
	   *@param filename The filename of the file to append on playlist. If there stands a ####ACTIVE#### in front of it, it gets set active
	   */
	void addsong(const QString filename);
	/**
	   *@brief addsong-function for internal drag and drop
	   *This function is used for dragging internally normally.<br>
	   *It appends the track at the end of the playlist.
	   *@param track a playlisttrack
	   */
	void addsong(playlisttrack *track);
	/**
	   *@brief insertsong-function for filenames
	   *Inserts a song in front of the given insertionpoint.<br>
	   *It is automatically checked if the track matches to the albums in it is between.<br>
	   *If the albums don´t match, there is a new album generated, that is inserted in between<br>
	   *@param filename the filename, that should be inserted.
	   *@param insertionpoint The item of the playlist the insert is in front of. (All inserts allways go in front)
	   */
	void insertsong(const QString filename,QTreeWidgetItem *insertionpoint);
	/**
	   *@brief insertsong-function for QTreeWidgetItem
	   *This function is used for the drag and drop-part, since playlisttrack and playlistalbum are directly derived from QTreeWidgetItem
	   *this is the function for dragging internally.
	   *@param draggeditem The item that is moved somewhere else
	   *@param insertionpoint The item the draggeditem is inserted in front
	   */
	void insertsong(QTreeWidgetItem *draggeditem,QTreeWidgetItem *insertionpoint);
	/**
	   *@brief getplaylist-function
	   *@returns a QStringList with the files that are in playlist. The active track gets a ####ACTIVE#### in front
	   */
	const QStringList getplaylist();
	/**
	   *@brief skiptonext-function
	   *This function skips to the next track in playlist. It automatically sets the activetrack to the next playlisttrack in list.<br>
	   *If repeatmode is on, after the last track the first track gets active.<br>
	   *If there is a valid next track, a pointer to it is returned.<br>
	   *If there is no valid next track a NULL-pointer is returned.
	   *@returns a pointer to the next playlisttrack
	   */
	const playlisttrack * skiptonext();
	/**
	   *@brief skiptonprev-function
	   *This function skips to the previous track in playlist. It automatically sets the activetrack to the prev playlisttrack in list.<br>
	   *If repeatmode is on, after the first track the last track gets active.<br>
	   *If there is a valid prev track, a pointer to it is returned.<br>
	   *If there is no valid prev track a NULL-pointer is returned.
	   *@returns a pointer to the prev playlisttrack
	   */
	const playlisttrack * skiptoprev();
	/**
	   *@brief getactivetrack-function
	   *@returns a pointer to the active track, or a NULL-pointer if there is no activetrack
	   */
	playlisttrack * getactivetrack();
	/**
	   *@brief recalcall-function
	   *This function recalcs all albums, and deletes albums not containing any tracks.<br>
	   *It also recalculates the shown time per album and the whole time displayed at top right of the widget.<br>
	   */
	void recalcall();
	/**
	   *@brief setrepeatmode-function
	   *@param mode the new repeatmode. TRUE means repeat on.
	   */
	void setrepeatmode(bool mode);
private:
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);
	void dragMoveEvent(QDragMoveEvent *event);
	QStringList getallfiles(QString);
	QMenu *contextmenu;
	QPoint storedpoint;
	QPoint dragStartPosition;
	int fileindex;
	QTimer *blingtimer;
	QTimer *rescantimer;
	QTimer *scrolltimer;
	int trackcount;
	QPoint lastmousepos;
	bool repeatmode;
	playlistdelegate *delegate;
private slots:
	void scrollcheck();
	void scroll(int);
	void bling();
	void playactionslot();
	void removeactionslot();
	//This needs to be reimplemented for the contextmenu to work
	void contextMenuEvent ( QContextMenuEvent * event );
	void doubleclicked(QTreeWidgetItem *,int);
	void selectionupdate();
	void checkfile(const QString &);
	void rescanexistingfiles();
signals:
	/**
	   *@brief trackcountchange-signal
	   *This signal is emitted, when the number of tracks in the widget changes.<br>
	   *It does not matter which way is used to in or decrease the tracks. (addsong,dragging,...)<br>
	   */
	void trackcountchange(int);
	/**
	   *@brief clearactionsignal
	   *This signal is emitted, when clear playlist is selected in the contextmenu of the widget
	   */
	void clearactionsignal();
	/**
	   *@brief removeactionsignal
	   *This signal is emitted, when delete tracks is selected in the contextmenu of the widget.
	   *@param items a QList of QTreeWidgetItem that should be removed from the playist
	   */
	void removeactionsignal(const QList<QTreeWidgetItem *> items);
	/**
	   *@brief actualchange-signal
	   *This signal is emitted, when the actual-tarck is changed.
	   */
	void actualchange(const playlisttrack *);
	/**
	   *@brief actualfilechangedtags-sgnal
	   *This signal is emitted, when the actualfile has changed tags, so the playing-widget can update its contents.
	   */
	void actualfilechangedtags();
};

/**
* @brief This class handles all album-stuff.
*/
class playlistalbum : public QTreeWidgetItem
{
	friend class playlistwidgetnew;
public:
	/**
	   * @brief Constructs a new album with the data in the given playlisttrack
	   * The track is also appended to the tracklist
	   * @param track a valid pointer to a playlisttrack
	   */
	playlistalbum(playlisttrack *track,QTreeWidget *parent);
	/**
	   * @brief add a track to the tracklist,and sort in under this playlistalbum in the tree
	   * @param track a valid pointer to a playlisttrack
	   */
	void addtrack(playlisttrack *);
	/**
	   * @brief removes a track from the tracklist and the tree
	   * The playlisttrack is not deleted, so you have to delete the pointer or append it to an other album
	   * @param track The track you want to remove
	   */
	void removetrack(playlisttrack *);
	/**
	   * @brief recalculate the time the albumtracks have together.
	   * @return the time calculated. If it is 0 delete the album
	   */
	int recalctime();
	/**
	  *@brief Sets this album active
	  */
	void setactive(bool);
	/**
	  * @brief This function is called to have the blinging-effect
	  */
	void bling();
	/**
	  * @brief This function gets called to update the coverpicture of an album, when a playlisttrack is modified
	  */
	void repaintcover(playlisttrack *);
private:
	int length;
	long viewindex;
	static long realindex;
	QString albumname;
	QString artistname;
	QIcon albumart;
	bool isplaying;
	int blingindex;
	bool reverse;
};

/**
* @brief This class handles all track-contents
*
*/
class playlisttrack : public QTreeWidgetItem
{
	friend class playlistalbum;
	friend class playlistwidgetnew;
public:
	/**
	    * @brief Constructs a new playlisttrack with the given filename
	    * @param filename The file to add to list
	    * @param parent The parent TreeWidget
	    */
	playlisttrack(const QString filename,QTreeWidget *parent);
	/**
	    *@brief Rereads the tags, and changes the displayed Text if it changed
	    */
	void reread();
	/**
	    * @brief Sets this track as active track, and highlights it
	    */
	void setactive(bool);
	/**
	    * @ Checks for the activestate of this track
	    * @returns True if the track is active otherwise FALSE
	    */
	inline bool isactive()
	{
		return isplaying;
	};
	/**
	    * @brief Get the albumname of this track
	    * @returns The albumname
	    */
	inline const QString getalbum()const
	{
		return albumname;
	};
	/**
	    * @brief Get the Artistname of this track
	    * @returns The artistname
	    */
	inline const QString getartist()const
	{
		return artistname;
	};
	/**
	    * @brief Get the filname of this track
	    * @returns The filname
	    */
	inline const QString getfile()const
	{
		return filename;
	};
	/**
	    * @brief Get the songlength of this track
	    * @returns The songlength
	    */
	inline const int getlength()const
	{
		return length;
	};
	/**
	    * @brief Get the internal index of the track
	    * @returns The index
	    */
	inline const long getindex()const
	{
		return viewindex;
	};
private:
	QDateTime modifieddate;
	QString filename;
	QString albumname;
	QString artistname;
	QString songname;
	int track;
	QIcon albumart;
	int length;
	int viewindex;
	static long runningindex;
	bool isplaying;
	int blingindex;
	bool reverse;
	void bling();
};

/**
  *@brief This is needed to show the Artistname and albumname splitted on two lines
  */
class playlistdelegate : public QItemDelegate
{
public:
	/**
	  * @brief This only passes throught ot the QItemdelegate-Constructor
	  */
	playlistdelegate(QObject *parent);
	/**
	*@brief This does the paint of the text on two lines if there is a <br> in the text
	*/
	void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
};

#endif
