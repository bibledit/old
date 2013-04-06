/*
 ** Copyright (©) 2003-2013 Teus Benschop.
 **  
 ** This program is free software; you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation; either version 3 of the License, or
 ** (at your option) any later version.
 **  
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **  
 ** You should have received a copy of the GNU General Public License
 ** along with this program; if not, write to the Free Software
 ** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **  
 */


#ifndef INCLUDED_BOOKDATA_H
#define INCLUDED_BOOKDATA_H


enum BookType {btOldTestament, btNewTestament, btFrontBackMatter, btOtherMaterial, btUnknown, btApocrypha};


typedef struct
{
  const char *name; // English name.
  const char *osis; // Osis name.
  const char *paratext; // Paratext ID.
  const char *bibleworks; // BibleWorks name.
  const char *onlinebible; // Online Bible name.
  unsigned int id; // Bibledit's internal id.
  BookType type; // The type of the book.
  bool onechapter; // The book has one chapter.
} book_record;


unsigned int bookdata_books_count();


#endif
