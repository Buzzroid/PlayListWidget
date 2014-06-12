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

#include "taglibinterface.h"

taglibinterface::taglibinterface(const QString text,const int type):
    filename(text),
    filetype(type)
{
  //Init all taginformation with empty values
  tag.v1tag.track=0;
  tag.v1tag.release=0;
  tag.v2tag.track=0;
  tag.v2tag.release=0;
  tag.v2tag.rating=0;
  tag.v2tag.volume=0;
  tag.filedata.audiomode=0;
  tag.filedata.length=0;
  tag.filedata.length=0;
  tag.filedata.filesize=0;
  tag.filedata.bitrate=0;
  tag.readok=FALSE;
  tag.v1tagexists=FALSE;
  tag.v2tagexists=FALSE;
}

const taginformation& taglibinterface::load()
{
  if (filetype==fileformat::Mp3)
    readinmp3();
  else if (filetype==fileformat::Ogg)
    readinogg();
  return tag;
}

bool taglibinterface::save(const taginformation& savetag,const int type, int version)
{
  if (type==fileformat::Mp3)
    return writemp3(savetag,version);
  else if (type==fileformat::Ogg)
    return writeogg(savetag);
  else
    return FALSE;
}

bool taglibinterface::readinmp3()
{
  QFileInfo filecheck(this->filename);
  if (filecheck.exists()&&filecheck.isFile())
    {
      //Since we want to know both tags, we can not just use a fileref
      TagLib::MPEG::File filetoread(QFile::encodeName ( this->filename ));
      tag.filedata.bitrate=filetoread.audioProperties()->bitrate();
      tag.filedata.length=filetoread.audioProperties()->length();
      tag.filedata.audiomode=filetoread.audioProperties()->channelMode();
      if (filetoread.ID3v1Tag(false))
        {
          if (!filetoread.ID3v1Tag()->isEmpty())
            {
              tag.v1tagexists=TRUE;
              tag.v1tag.song=TStringToQString ( filetoread.ID3v1Tag()->title() );
              tag.v1tag.album=TStringToQString ( filetoread.ID3v1Tag()->album() );
              tag.v1tag.artist=TStringToQString ( filetoread.ID3v1Tag()->artist() );
              tag.v1tag.genre=TStringToQString ( filetoread.ID3v1Tag()->genre() );
              tag.v1tag.track=filetoread.ID3v1Tag()->track();
              tag.v1tag.release=filetoread.ID3v1Tag()->year();
            }
        }
      if (filetoread.ID3v2Tag(false))
        {
//I simply don't know, why I get a valid pointer, if there are no tags. So I check if they are empty, if i get a pointer.
          if (!filetoread.ID3v2Tag()->isEmpty())
            {
              tag.v2tagexists=TRUE;
              tag.v2tag.song=TStringToQString ( filetoread.ID3v2Tag()->title() );
              tag.v2tag.album=TStringToQString ( filetoread.ID3v2Tag()->album() );
              tag.v2tag.artist=TStringToQString ( filetoread.ID3v2Tag()->artist() );
              tag.v2tag.genre=TStringToQString ( filetoread.ID3v2Tag()->genre() );
              if (!tag.v2tag.genre.size())
                {
                  //hm empty, try it with TIT1-Frame, since we used that in older kyamo-version already
                  TagLib::ID3v2::FrameList searchedframe=filetoread.ID3v2Tag()->frameListMap() ["TIT1"];
                  if ( !searchedframe.isEmpty() )
                    {
                      tag.v2tag.genre= TStringToQString ( searchedframe.front()->toString() );
                    }
                }
              tag.v2tag.track=filetoread.ID3v2Tag()->track();
              tag.v2tag.release=filetoread.ID3v2Tag()->year();
              //Only if some data  exists, we can say the tags are existing
              //Tags are read, now get the coverart, if it is existing
              TagLib::ID3v2::AttachedPictureFrame *pictureframe=NULL;
              TagLib::ID3v2::FrameList searchedframe=filetoread.ID3v2Tag()->frameListMap() ["APIC"];
              if ( !searchedframe.isEmpty() )
                {
                  TagLib::ID3v2::FrameList::ConstIterator frameindex=searchedframe.begin();
                  while ( frameindex!=searchedframe.end())
                    {
                      pictureframe=static_cast<TagLib::ID3v2::AttachedPictureFrame *> (*frameindex);
                      TagLib::ByteVector datavector=pictureframe->picture();
                      coverart newcoverart;
                      newcoverart.image.loadFromData ( QByteArray(pictureframe->picture().data(),pictureframe->size()-1));
                      newcoverart.picposition=pictureframe->type();
                      tag.v2tag.pictures.append(newcoverart);
                      ++frameindex;
                    }
                }
            }
        }
      return TRUE;
    }
  else
    return FALSE;
}

bool taglibinterface::readinogg()
{
  QFileInfo filecheck(this->filename);
  if (filecheck.exists()&&filecheck.isFile())
    {
      TagLib::FileRef filetoread(QFile::encodeName ( this->filename ));
      tag.filedata.bitrate=filetoread.audioProperties()->bitrate();
      tag.filedata.length=filetoread.audioProperties()->length();
      tag.v1tagexists=TRUE;
      tag.v1tag.song=TStringToQString ( filetoread.tag()->title() );
      tag.v1tag.album=TStringToQString ( filetoread.tag()->album() );
      tag.v1tag.artist=TStringToQString ( filetoread.tag()->artist() );
      tag.v1tag.genre=TStringToQString ( filetoread.tag()->genre() );
      tag.v1tag.track=filetoread.tag()->track();
      tag.v1tag.release=filetoread.tag()->year();
      return TRUE;
    }
  else
    return FALSE;
}

bool taglibinterface::writeogg(const taginformation& tag)
{
  QFileInfo filecheck(this->filename);
  if (filecheck.exists()&&filecheck.isFile()&&filecheck.isWritable())
    {
      TagLib::Ogg::Vorbis::File filetosave(QFile::encodeName(this->filename));
      TagLib::Ogg::XiphComment *newcomment=filetosave.tag();
      wchar_t *warray=converttowchar(tag.v1tag.artist);
      newcomment->setArtist(TagLib::String(warray,TagLib::String::UTF8));
      delete []warray;
      warray=converttowchar(tag.v1tag.song);
      newcomment->setTitle(TagLib::String(warray,TagLib::String::UTF8));
      delete []warray;
      warray=converttowchar(tag.v1tag.album);
      newcomment->setAlbum(TagLib::String(warray,TagLib::String::UTF8));
      delete []warray;
      warray=converttowchar(tag.v1tag.genre);
      newcomment->setGenre(TagLib::String(warray,TagLib::String::UTF8));
      delete []warray;
      newcomment->setTrack(tag.v1tag.track);
      newcomment->setYear(tag.v1tag.release);
      filetosave.save();
      return TRUE;
    }
  else
    return FALSE;
}

bool taglibinterface::writemp3(const taginformation& tag,const int version)
{
  QFileInfo filecheck(this->filename);
  if (filecheck.exists()&&filecheck.isFile()&&filecheck.isWritable())
    {
      TagLib::MPEG::File filetosave(QFile::encodeName(this->filename));
      filetosave.strip();
      if (version&tagversion::v1tag)
        {
          TagLib::ID3v1::Tag *newtag=filetosave.ID3v1Tag(TRUE);
          wchar_t *warray=converttowchar(tag.v1tag.artist);
          newtag->setArtist(TagLib::String(warray,TagLib::String::UTF8));
          delete []warray;
          warray=converttowchar(tag.v1tag.song);
          newtag->setTitle(TagLib::String(warray,TagLib::String::UTF8));
          delete []warray;
          warray=converttowchar(tag.v1tag.album);
          newtag->setAlbum(TagLib::String(warray,TagLib::String::UTF8));
          delete []warray;
          warray=converttowchar(tag.v1tag.genre);
          newtag->setGenre(TagLib::String(warray,TagLib::String::UTF8));
          delete []warray;
          newtag->setTrack(tag.v1tag.track);
          newtag->setYear(tag.v1tag.release);
          filetosave.save(TagLib::MPEG::File::ID3v1);
          //Get rid off the v2-tag, if we don't want to write one
          if ( version==tagversion::v1tag&&filetosave.ID3v2Tag() )
            {
              filetosave.strip ( TagLib::MPEG::File::ID3v2,TRUE );
            }
        }
      if (version&tagversion::v2tag)
        {
          TagLib::ID3v2::Tag *newtag=filetosave.ID3v2Tag(TRUE);
          wchar_t *warray=converttowchar(tag.v2tag.artist);
          newtag->setArtist(TagLib::String(warray,TagLib::String::UTF8));
          delete []warray;
          warray=converttowchar(tag.v2tag.song);
          newtag->setTitle(TagLib::String(warray,TagLib::String::UTF8));
          delete []warray;
          warray=converttowchar(tag.v2tag.album);
          newtag->setAlbum(TagLib::String(warray,TagLib::String::UTF8));
          delete []warray;
          warray=converttowchar(tag.v2tag.genre);
          newtag->setGenre(TagLib::String(warray,TagLib::String::UTF8));
          delete []warray;
          newtag->setTrack(tag.v2tag.track);
          newtag->setYear(tag.v2tag.release);
          //Store pictures in the list in the v2tag
          QList<coverart>::ConstIterator coverindex=tag.v2tag.pictures.begin();
          while (coverindex!=tag.v2tag.pictures.end())
            {
              storepicture((*coverindex).image,(TagLib::ID3v2::AttachedPictureFrame::Type)(*coverindex).picposition,filetosave);
              coverindex++;
            }
          newtag->render();
          filetosave.save(TagLib::MPEG::File::ID3v2);
          //Get rid off the v1-tag, if we don't want to write one
          if ( version==tagversion::v2tag&&filetosave.ID3v1Tag() )
            {
              filetosave.strip ( TagLib::MPEG::File::ID3v1,TRUE );
            }
        }
      return TRUE;
    }
  else
    return FALSE;
}

void taglibinterface::storepicture(const QImage& picture,const TagLib::ID3v2::AttachedPictureFrame::Type picposition,TagLib::MPEG::File& filetosave)
{
  bool newframe=FALSE;
  TagLib::ID3v2::AttachedPictureFrame *pictureframe=NULL;
  TagLib::ID3v2::FrameList searchedframe=filetosave.ID3v2Tag()->frameListMap() ["APIC"];
  bool found=FALSE;
  if ( !searchedframe.isEmpty() )
    {
      TagLib::ID3v2::FrameList::ConstIterator frameindex=searchedframe.begin();
      while ( frameindex!=searchedframe.end()&&!found )
        {
          pictureframe=static_cast<TagLib::ID3v2::AttachedPictureFrame *> (*frameindex);
          if ( pictureframe->type()==picposition )
            found=TRUE;
          ++frameindex;
        }
    }
  if (!found)
    {
      newframe=TRUE;
      pictureframe=new TagLib::ID3v2::AttachedPictureFrame;
    }
  QByteArray picturedata;
  QBuffer buffer ( &picturedata );
  buffer.open ( QIODevice::WriteOnly );
  if (picposition==TagLib::ID3v2::AttachedPictureFrame::FileIcon)
    {
      //We need to rescale the picture to 32x32, and store as png
      QImage newimage=picture.scaled(32,32);
      newimage.save ( &buffer,"PNG" );
      buffer.close();
      pictureframe->setMimeType ( "image/png" );
    }
  else
    {
      picture.save ( &buffer,"JPEG" );
      buffer.close();
      pictureframe->setMimeType ( "image/jpeg" );
    }
  pictureframe->setType ( picposition );
  pictureframe->setPicture ( TagLib::ByteVector ( picturedata.constData(),picturedata.size() ) );
  if ( newframe )
    filetosave.ID3v2Tag()->addFrame ( pictureframe );
}

wchar_t * taglibinterface::converttowchar(const QString& text)
{
  wchar_t *converted;
  if (text.isNull())
    {
      converted=NULL;
    }
  else
    {
      QString text2=text.toUtf8();
      converted=new wchar_t [text2.length()+1];
      text2.toWCharArray(converted);
      converted[text2.length()]='\0';
    }
  return converted;
}
