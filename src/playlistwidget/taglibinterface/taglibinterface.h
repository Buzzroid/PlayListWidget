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
 *@file taglibinterface.h
 *The class for taglibhandling is declared in this file.<br>
 *Include this file, if you want to use taglib-related functions.<br>
 *You also need the files globals.h and taginformation.h when you want to use this file.
 *Since the taglib-interface is also a part of my programm k-yamo, there are more possibilities
 *included into this interafce than actually used. You can use this interface also standalone if you want to.
 *It supports mp3 and ogg-files for reading and writing.
 *I would be glad to get any improvements you make on this.
 *
 *@author Roland Weigert
 *@date 17-11-2009
 *@todo Add Rating and volume support
 */

#ifndef _TAGLIBINTERFACE_H_
#define _TAGLIBINTERFACE_H_

#include <QtCore>

//taglib-library-includes
#include <taglib/audioproperties.h>
#include <taglib/mpegfile.h>
#include <taglib/vorbisfile.h>
#include <taglib/xiphcomment.h>
#include <taglib/id3v2tag.h>
#include <taglib/id3v1tag.h>
#include <taglib/attachedpictureframe.h>
#include <taglib/id3v2frame.h>
#include <taglib/textidentificationframe.h>
#include <taglib/fileref.h>
#include <taglib/mpegheader.h>


#include "globals.h"
#include "taginformation.h"

/**
*@brief The Interface class to all taglibactions
*
*All actions that use taglib, are handled over this interfaceclass
*There is no way to set single tag informations.
*Reading and setting tags is done only over a complete Taginformation
*/
class taglibinterface
  {
  public:
    /**
    *@brief Constructor
    *
    *Takes over the filename, and initializes the tags, to a zerovalue
    *@PARAM QString that takes the filename
    *@PARAM int containing the filetype to open
    */
    taglibinterface(const QString,const int );
    /**
    *@brief Load a tag
    *
    *Loads the filename given in constructor and gives back a taginformation-struct.
    */
    const taginformation& load();
    /**
    *@brief Saves a tag
    *
    *Saves the given taginformation into the filename that was given in constructor.<br>
    *As default it saves both id3v1- and id3v2-tags, when no version is given.
    *@PARAM tag Contains all informations that should be saved.
    *@PARAM filetype Tell taglibinterface what filetype the object is
    *@PARAM version Contains the tagversion the file will be saved if it is mp3(use tagversion::tagtype to set filetype)
    */
    bool save(const taginformation& tag,const int filetype,const int version=tagversion::v1tag|tagversion::v2tag);
  private:
    /**
    *@brief Memberfunction that reads in mp3
    *
    *This is the MP3-specific reading-Method.<br>
    *It loads V1tags and V2tags, if they are there.<br>
    *Also it tries to find apicture stored in the file.
    *@return TRUE if file could be read, FALSE if not.
    */
    bool readinmp3();
    /**
    *@brief Memberfunction that reads in Oggtags
    *
    *This is the Ogg-specific reading-Method.<br>
    *@return TRUE if file could be read, FALSE if not.
    */
    bool readinogg();
    /**
    *@brief Memberfunction that writes Ogg-tags
    *
    *This is the Ogg-specific writing-Method.<br>
    *@return TRUE if file could be written, FALSE if not.
    */
    bool writeogg(const taginformation&);
    /**
    *@brief Memberfunction that writes Mp3-tags
    *
    *This is the Mp3-specific writing-Method.<br>
    *@param tag The tag that should be saved
    *@param version tagversion to save (use tagversion::tagtype to set filetype)
    *@return TRUE if file could be written, FALSE if not.
    */
    bool writemp3(const taginformation& tag,const int version=tagversion::v1tag|tagversion::v2tag);
    /**
    *@brief Store picture in this file
    *
    *@param picture The QImage that has to be included in file.
    *@param picposition Use TagLib::ID3v2::AttachedPictureFrame to choose wich picture to save.
    *@param filetosave The file that is used for saving.
    */
    void storepicture(const QImage& picture,const TagLib::ID3v2::AttachedPictureFrame::Type picposition,TagLib::MPEG::File& filetosave);
    /**
    *@brief converts string to wchar_t in Utf-format
    *@param text text for conversion
    *@return wchar_t * pointer to wchar_t. If it is not needed anymore, it must be deleted
    */
    wchar_t * converttowchar(const QString& text);
    /**Internal taginformation, for handing over to and from the real read and save-functions*/
    taginformation tag;
    /**Only internal use of filename. It is automaticly set by constructor*/
    QString filename;
    /**Only internal use of filetype. It is automaticly set by constructor*/
    int filetype;
  };

#endif
