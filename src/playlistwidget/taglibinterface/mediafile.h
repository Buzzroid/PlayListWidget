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
 * @file mediafile.h
 *This file holds the class for handling of mediafiles.<br>
 *Include this file, if you want to read, change or save some tags.<br>
 *You also need the files globals.h and taginformation.h, if you wan to use this file.<br>
 *As this class needs to know wich tagversion and tagengine it should use, there is the<br>
 *dependency to settings/rcfile.h and the extern variable settingsfile.<br>
 *If you want to use this class in your own app, just replace the expressions starting with settingsfile->...<br>
 *The file taglibinterface.h is a standard include, because all tag-reading is delegated to taglibinterface.<br>
 *@author Roland Weigert
 *@date 17-11-2009
 *@todo add Rating and Volumesetting and readingfunctions
 */

#ifndef _MEDIAFILE_H_
#define _MEDIAFILE_H_

#include <QtCore>
#include <QtGui>

#include "taginformation.h"
#include "taglibinterface.h"
#include "globals.h"

/**
 *@brief Masterclass for all types of media
 *
 *The motherclass for all media.<br>
 *It keeps track of the operations allowed for the files in use.<br>
 *So the routine,that ueses a mediafile, can check if the operation is supported for the current file.<br>
 *All the variables are capsulated, so the only way to get values, is over getter-functions.<br>
 *This will hopefully make improving the code much easier, because the API will not change at all, when improving code.<br>
 *You always must call issupported() after constructor, because this sets the filetype.<br>
 *Without setting of the filetype, nothing will be done on the file.
 */

class mediafile
  {
  public:
    /**
    *@brief constructor with existing file
    *
    *The Constructor just takes a filename.
    *@PARAM QString filename
    */
    mediafile ( const QString );
    /**
    *@brief Check if current file can use pictures (id3v2 can, all others not)
    *
    *@return TRUE when pictures are supported by the current filtype, FALSE if not
    */
    const bool canhandlepicture () const;
    /**
    *@brief Check if current file can save a rating
    *
    *@return TRUE when rating is supported by the current filtype, FALSE if not
    */
    const bool canhandlerating () const;
    /**
    *@brief Check if current file can save volumeinformation
    *
    *@return TRUE when volumesetting is supported by the current filtype, FALSE if not
    */
    const bool canhandlevolume () const;
    /**
    *@brief Get image stored in the file, containig to this object
    *
    *@return QImage with existing image or a Nullimage, if there is no picture in this object
    */
    const QList<coverart>& getpictures() const;
    /**
    *@brief gives back artistname
    *
    *@return QString with artistname
    */
    const QString& getartistname() const;
    /**
    *@brief gives back albumname
    *
    *@return QString with albumname
    */
    const QString& getalbumname() const;
    /**
    *@brief gives back songname
    *
    *@return QString with songname
    */
    const QString& getsongname() const;
    /**
    *@brief gives back genrename
    *
    *@return QString with genrename
    */
    const QString& getgenrename() const;
    /**
    *@brief gives back releasdate
    *
    *@return int with releasedate
    */
    const int getreleasedate() const;
    /**
    *@brief gives back tracknumber
    *
    *@return int with tracknumber
    */
    const int gettracknumber() const;
    /**
     *@brief gives back the length of this file
     *
     *@return int with length in seconds
     */
    const int getlength() const;
    /**
     *@brief gives back the bitrate of this file
     *
     *@return int with bitrate
     */
    const int getbitrate() const;
    /**
    *@brief Set an Image, if it is possible for the filtype
    *
    *@return TRUE if the image was set, FALSE if the filtype doesnt support images
    */
    bool setpicture(const QImage &,const Coverart::pt picturetype);
    /**
    *@brief Set the artistname for object
    */
    void setartistname(const QString&);
    /**
    *@brief Set the albumname for object
    */
    void setalbumname(const QString&);
    /**
    *@brief Set the songname for object
    */
    void setsongname (const QString&);
    /**
    *@brief Set the genrename for object
    */
    void setgenrename(const QString&);
    /**
    *@brief Set the tracknumber for object
    */
    void settracknumber(const int);
    /**
    *@brief Set the releasedate for object
    */
    void setreleasedate(const int);
    /**
    *@brief Method for checking if the file is readable by our application
    *
    *This Method checks, if the file is readable by our application.<br>
    *It also checks out, which format the file has, and sets filetype internal.<br>
    *For the Moment only fileformat::Mp3 and fileformat::Ogg are supported.<br>
    *
    *@return TRUE if the file is valid, FALSE if file has not readable format, or does not exist.
    */
    const bool issupportedfile();
    /**
    *@brief This method checks if the object was modified
    *@return TRUE if something in the tags has changed, FALSE if the tags are same as on fileopen
    */
    const bool ismodified() const;
    /**
    *@brief This method reads the tags of the object
    *
    *If issupportedfile() was not called before this method is called, it does this itself, to get the filetype
    *, and to see, if the file exists;<br>
    *The reading itself is delegated to the taglibinterface, since all reading is done using taglib.<br>
    *If version is not set, Mp3s will always load v2tags, if they exist.<br>
    *Set tagversion::tagtype to choose what tags are preffered. If not set v2tags are the ones that are used.
    *@return TRUE if file could be read, no matter if there where tags inside<br>
    *FALSE if file doesnt exist or file has an unsupported format
     */
    const bool readtags(int version=tagversion::v2tag);
    /**
    *@brief This method saves the tags of the object
    *
    *call ismodified() if you want to know if the file needs saving
    *@param version version with the tagversion. If version is not set V1- and V2tags will be set.<br>
    *Version is ignored on Ogg-Files.
    *@return TRUE if file could be saved, <br>
    *FALSE if file doesnt exist or file was not modified
     */
    const bool savetags(int version=tagversion::v1tag|tagversion::v2tag);
  private:
    /**Only internal use of filename, constructor sets this*/
    QString filename;
    /**Only internal use of albumart. Set it over setpicture(const QImage, const coverart::pt)*/
    QList<coverart> albumartlist;
    /**Only internal use of artistname. Set this over setartistname() and read it over getartistname()*/
    QString artistname;
    /**Only internal use of albumname. Set this over setalbumname() and read it over getalbumname()*/
    QString albumname;
    /**Only internal use of songname. Set this over setsongname() and read it over getsongname()*/
    QString songname;
    /**Only internal use of genrename. Set this over setgenrename() and read it over getgenrename()*/
    QString genrename;
    /**Only internal use of tracknumber. Set this over settracknumber() and read it over gettracknumber()*/
    int tracknumber;
    /**Only internal use of releasedate. Set this over setreleasedate() and read it over getreleasedate()*/
    int releasedate;
    /**Only internal use of tfiltype. Set this over issupportedfile()*/
    int filetype;
    /**Not yet used*/
    int volume;
    /**Not yet used*/
    int rating;
    /**Not yet used*/
    int bitrate;
    /**Only internal use of length. Set this over readtags()*/
    int length;
    /**Only internal use of filesize. Set this over readtags()*/
    int filesize;
    /**Only internal use of audiomode. Set this over readtags()*/
    int audiomode;
    /**Only internal use of modified. This is automatically changed, when some part of tags change*/
    bool modified;
  };

#endif
