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
 *@file taginformation.h
 *This file only holds the declaration of the structs, for holding of metadata.<br>
 *Extend new tags in here, if you want to enhance the functionality of the tags.
 *
 *@author Roland Weigert
 *@date 25-10-2009
 */

#ifndef _TAGINFORMATION_H_
#define _TAGINFORMATION_H_

#include <QtCore>
#include <QtGui>

struct coverart;

/**
*@brief a simple struct, that can hold tags
*
*Any part of the programm that has to swap tags, must include this header.<br>
*Because this is the struct used for transporting complete tags.
*/
struct taginformation
  {
    /**
    *Check this, if you want to know, if the read or save process worked.
    */
    bool readok;
    /**
    *Check this, if you want to know, if there is a v1tag.
    */
    bool v1tagexists;
    /**
    *Check this, if you want to know, if there is v2tag.
    */
    bool v2tagexists;
    /**
    *@brief Holds the v1tags, if they are existing
    */
    struct v1taginformation
      {
        /**holds the Artistname*/
        QString artist;
        /**holds the Albumname*/
        QString album;
        /**holds the Songname*/
        QString song;
        /**holds the Genrename*/
        QString genre;
        /**holds the Tracknumber*/
        int track;
        /**holds the Releasedate*/
        int release;
      }
    v1tag;
    /**
    *@brief Holds the v2tags, if they are existing
    */
    struct v2taginformation
      {
        /**holds the Artistname*/
        QString artist;
        /**holds the Albumname*/
        QString album;
        /**holds the Songname*/
        QString song;
        /**holds the Genrename*/
        QString genre;
        /**holds the Tracknumber*/
        int track;
        /**holds the Releasedate*/
        int release;
        /**holds the Albumart*/
        QList<coverart> pictures;
        /**holds the Userrating*/
        int rating;
        /**holds the Volumeinformation*/
        int volume;
      }
    v2tag;
    /**
    *@brief Holds the filespecific values
    */
    struct filedatainformation
      {
        /**holds the bitrate of file*/
        int bitrate;
        /**holds the songlength in seconds*/
        int length;
        /** holds the filesize in kb*/
        int filesize;
        /** holds the audiomode of the file*/
        int audiomode;
      }
    filedata;
  };

/**
*@brief Holds a coverart-object
*/
struct coverart
  {
    /**Image*/
    QImage image;
    /**Picturetype*/
    int picposition;
  };

#endif
