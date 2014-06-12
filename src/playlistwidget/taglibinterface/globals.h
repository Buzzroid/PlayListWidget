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
 *@file globals.h
 * All global constants and macros are defined in here.<br>
 *Include this file, if you want to use the namespaces, and there enums.
 *
 *@author Roland Weigert
 *@date 17-11-2009
 */

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

/**
*@brief Holds the enum for filetypes
*/
namespace fileformat
  {
  /**
  *Enum for the filetype.
  */
  enum mediafiletype
  {
    /**MP3-File*/
    Mp3=1,
    /**OGG-File*/
    Ogg=2,
    /**APE-File*/
    Ape=8,
    /**Flac-File*/
    Flac=16,
    /**Unknown Format*/
    Unknown=128
  };
}

/**
*@brief Holds the enum for audiomodes
*/
namespace audiomode
  {
  /**
  *Enum for the Audiomodes.
  */
  enum msjs
  {
    /**Stereo-mode*/
    Stereo = 0,
    /**Jointstereo-mode*/
    JointStereo = 1,
    /**Mono-mode 2 Channels*/
    DualChannel = 2,
    /**Mono-mode 1 Channel*/
    SingleChannel = 3
  };
}

/**
  *@brief Holds the enum for tagversions
  */
namespace tagversion
  {
  /**
  *Enum for the tagversions.<br>
  *Use v1&v2 to save both tagversions.
  */
  enum tagtype
  {
    /**Only v1 tag*/
    v1tag=1,
    /**Only v2 tag*/
    v2tag=2,
  };
}

/**
  *@brief Holds the enum for tagvengine
  */
namespace tagengine
  {
  enum engine
  {
    /**Use taglib for saving of the tags (v1 and or v2.4-tags)*/
    taglib=1,
    /**Use id3lib for saving of the tags (v1 and or v2.3-tags)*/
    id3lib=2
  };
};

/**
  * @brief  The Namespace that holds the pt picturetype enum
  */
namespace Coverart
  {
  /**
  *Its the same enum as is in taglib, that numbers the various coverart.
  */
  enum    pt
  {
    Other = 0x00,
    FileIcon = 0x01,
    OtherFileIcon = 0x02,
    FrontCover = 0x03,
    BackCover = 0x04,
    LeafletPage = 0x05,
    Media = 0x06,
    LeadArtist = 0x07,
    Artist = 0x08,
    Conductor = 0x09,
    Band = 0x0A,
    Composer = 0x0B,
    Lyricist = 0x0C,
    RecordingLocation = 0x0D,
    DuringRecording = 0x0E,
    DuringPerformance = 0x0F,
    MovieScreenCapture = 0x10,
    ColouredFish = 0x11,
    Illustration = 0x12,
    BandLogo = 0x13,
    PublisherLogo = 0x14
  };
}

namespace picturetype
  {
  enum types
  {
    bmp=0,
    jpg=1,
    gif=2,
    png=3
  };
};

#endif
