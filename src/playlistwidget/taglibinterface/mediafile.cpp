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
#include "mediafile.h"

mediafile::mediafile ( const QString text):
    filename(text),
    filetype(0),
    modified(FALSE),
    tracknumber(0),
    releasedate(0),
    length(0)
{}

const bool mediafile::canhandlepicture() const
  {
    if (this->filetype==fileformat::Mp3)
      return TRUE;
    else if (this->filetype==fileformat::Ogg)
      return FALSE;
    else
      return FALSE;
  }

const bool mediafile::canhandlerating () const
  {
    if (this->filetype==fileformat::Mp3)
      return TRUE;
    else if (this->filetype==fileformat::Ogg)
      return FALSE;
    else
      return FALSE;
  }

const bool mediafile::canhandlevolume () const
  {
    if (this->filetype==fileformat::Mp3||filetype==fileformat::Ogg)
      return TRUE;
    else
      return FALSE;
  }

const QList<coverart>& mediafile::getpictures() const
  {
    return albumartlist;
  }

const QString& mediafile::getartistname() const
  {
    return artistname;
  }

const QString& mediafile::getalbumname() const
  {
    return albumname;
  }

const QString& mediafile::getsongname() const
  {
    return songname;
  }

const QString& mediafile::getgenrename() const
  {
    return genrename;
  }

bool mediafile::setpicture(const QImage& image,const Coverart::pt picturetype)
{
  if (this->canhandlepicture())
    {
      QList<coverart>::Iterator coverindex=this->albumartlist.begin();
      bool found=FALSE;
      while (coverindex!=albumartlist.end()&&!found)
        {
          if ((*coverindex).picposition==picturetype)
            {
              found=TRUE;
              (*coverindex).image=image.copy();
            }
          coverindex++;
        }
      if (!found)
        {
          coverart newelement;
          newelement.image=image.copy();
          newelement.picposition=picturetype;
          this->albumartlist.append(newelement);
        }
      this->modified=TRUE;
      return TRUE;
    }
  else
    return FALSE;
}

void mediafile::setartistname(const QString& text)
{
  this->artistname=text;
  this->modified=TRUE;
}

void mediafile::setalbumname(const QString& text)
{
  this->albumname=text;
  this->modified=TRUE;
}

void mediafile::setsongname (const QString& text)
{
  this->songname=text;
  this->modified=TRUE;
}

void mediafile::setgenrename(const QString& text)
{
  this->genrename=text;
  this->modified=TRUE;
}

const int mediafile::gettracknumber() const
  {
    return this->tracknumber;
  }

const int mediafile::getreleasedate() const
  {
    return this->releasedate;
  }

const int mediafile::getbitrate() const
  {
    return this->bitrate;
  }

const int mediafile::getlength() const
  {
    return this->length;
  }

void mediafile::settracknumber(const int number)
{
  if (number>=0&&number <=99)
    {
      this->tracknumber=number;
      this->modified=TRUE;
    }
}

void mediafile::setreleasedate(const int number)
{
  if (number>=0)
    {
      this->releasedate=number;
      this->modified=TRUE;
    }
}

const bool mediafile::issupportedfile()
{
  QFileInfo filecheck(this->filename);
  if (filecheck.exists())
    {
      QString suffix=filecheck.completeSuffix().toLower();
      if (suffix.endsWith("mp3"))
        {
          this->filetype=fileformat::Mp3;
        }
      else if (suffix.endsWith("ogg"))
        {
          this->filetype=fileformat::Ogg;
        }
      else if (suffix.endsWith("ape"))
        {
          this->filetype=fileformat::Ape;
        }
      else if (suffix.endsWith("flac"))
        {
          this->filetype=fileformat::Flac;
        }
      else
        {
          this->filetype=fileformat::Unknown;
        }
      if (this->filetype==fileformat::Mp3||this->filetype==fileformat::Ogg)
        return TRUE;
      else
        return FALSE;
    }
  else
    {
      this->filetype=-1;
      return FALSE;
    }
}

const bool mediafile::readtags(int  version)
{
  if (!this->filetype)
    this->issupportedfile();
  if (this->filetype==fileformat::Mp3)
    {
      taglibinterface file(filename,fileformat::Mp3);
      taginformation tags=file.load();
      this->bitrate=tags.filedata.bitrate;
      this->length=tags.filedata.length;
      this->filesize=tags.filedata.filesize;
      if (tags.v1tagexists&&version==tagversion::v1tag)
        {
          this->artistname=tags.v1tag.artist;
          this->albumname=tags.v1tag.album;
          this->songname=tags.v1tag.song;
          this->genrename=tags.v1tag.genre;
          this->tracknumber=tags.v1tag.track;
          this->releasedate=tags.v1tag.release;
        }
      if (tags.v2tagexists&&version==tagversion::v2tag)
        {
          this->artistname=tags.v2tag.artist;
          this->albumname=tags.v2tag.album;
          this->songname=tags.v2tag.song;
          this->genrename=tags.v2tag.genre;
          this->tracknumber=tags.v2tag.track;
          this->releasedate=tags.v2tag.release;
          this->rating=tags.v2tag.rating;
          this->volume=tags.v2tag.volume;
          QList <coverart>::Iterator pictureindex=tags.v2tag.pictures.begin();
          while (pictureindex!=tags.v2tag.pictures.end())
            {
              coverart newelement;
              newelement.image=(*pictureindex).image.copy();
              newelement.picposition=(*pictureindex).picposition;
              this->albumartlist.append(newelement);
              pictureindex++;
            }
        }
      else if (tags.v1tagexists)
        {
          this->artistname=tags.v1tag.artist;
          this->albumname=tags.v1tag.album;
          this->songname=tags.v1tag.song;
          this->genrename=tags.v1tag.genre;
          this->tracknumber=tags.v1tag.track;
          this->releasedate=tags.v1tag.release;
        }
      else if (tags.v2tagexists)
        {
          this->artistname=tags.v2tag.artist;
          this->albumname=tags.v2tag.album;
          this->songname=tags.v2tag.song;
          this->genrename=tags.v2tag.genre;
          this->tracknumber=tags.v2tag.track;
          this->releasedate=tags.v2tag.release;
          this->rating=tags.v2tag.rating;
          this->volume=tags.v2tag.volume;
          QList <coverart>::Iterator pictureindex=tags.v2tag.pictures.begin();
          while (pictureindex!=tags.v2tag.pictures.end())
            {
              coverart newelement;
              newelement.image=(*pictureindex).image.copy();
              newelement.picposition=(*pictureindex).picposition;
              this->albumartlist.append(newelement);
              pictureindex++;
            }
        }
      return TRUE;
    }
  else if (this->filetype==fileformat::Ogg)
    {
      taglibinterface file(filename,fileformat::Ogg);
      taginformation tags=file.load();
      if (tags.v1tagexists)
        {
          this->artistname=tags.v1tag.artist;
          this->albumname=tags.v1tag.album;
          this->songname=tags.v1tag.song;
          this->genrename=tags.v1tag.genre;
          this->tracknumber=tags.v1tag.track;
          this->releasedate=tags.v1tag.release;
          this->bitrate=tags.filedata.bitrate;
          this->length=tags.filedata.length;
          this->filesize=tags.filedata.filesize;
        }
      return TRUE;
    }
  else
    return FALSE;
}

const bool mediafile::savetags(int version)
{
  //First make a taginformation, containing all stuff needed
  taginformation savetag;
  savetag.v1tag.artist=this->artistname;
  savetag.v1tag.album=this->albumname;
  savetag.v1tag.song=this->songname;
  savetag.v1tag.genre=this->genrename;
  savetag.v1tag.track=this->tracknumber;
  savetag.v1tag.release=this->releasedate;
  savetag.v2tag.artist=this->artistname;
  savetag.v2tag.album=this->albumname;
  savetag.v2tag.song=this->songname;
  savetag.v2tag.genre=this->genrename;
  savetag.v2tag.track=this->tracknumber;
  savetag.v2tag.release=this->releasedate;
  savetag.v2tag.rating=this->rating;
  savetag.v2tag.volume=this->volume;
  //copy all stored pictures in the taginformation
  QList<coverart>::ConstIterator coverindex=this->albumartlist.begin();
  while (coverindex!=this->albumartlist.end())
    {
      savetag.v2tag.pictures.append(*coverindex);
      coverindex++;
    }
      taglibinterface file(filename,this->filetype);
      return file.save(savetag,this->filetype,tagversion::v1tag|tagversion::v2tag);
}

const bool mediafile::ismodified() const
  {
    return this->modified;
  }
