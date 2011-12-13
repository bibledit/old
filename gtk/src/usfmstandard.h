/*
 ** Copyright (©) 2003-2012 Teus Benschop.
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

#ifndef INCLUDED_USFMDATA_H
#define INCLUDED_USFMDATA_H


#include "libraries.h"


enum UsfmHasEndMarkerType {uhemtNo, uhemtYes, uhemtOptional};
enum UsfmFunctionType
{
  uftUnknown,
  uftFile,
  uftEncoding,
  uftStatus,
  uftComment,
  uftRunningHeader,
  uftLongToc,
  uftShortToc,
  uftBookAbbr,
  uftTitle,
  uftSection,
  uftParagraph,
  uftList,
  uftText,
  uftChapter,
  uftChapterLabel,
  uftChapterMarker,
  uftVerse,
  uftVerseMarker,
  uftNoBreak,
  uftRow,
  uftCell,
  uftFootnote,
  uftEndnote,
  uftNoteMark,
  uftCrossreference,
  uftKeyword,
  uftSpace,
  uftBreak,
  uftPage,
  uftFigure,
  uftIndex,
  uftPronunciation,
  uftWordlist,
  uftGreek,
  uftHebrew,
  uftPeripheral
};

class USFMStandard
{
public:
  USFMStandard (int dummy);
  ~USFMStandard ();
  bool marker_exists (const ustring& marker);
  UsfmFunctionType marker_function (const ustring& marker);
private:
  void load ();
  void store ();
  ustring marker;
  map <ustring, bool> exists;
  bool startswithbackslash_v;
  map <ustring, bool> startswithbackslash;
  UsfmHasEndMarkerType hasendmarker_v;
  map <ustring, UsfmHasEndMarkerType> hasendmarker;
  ustring variants;
  bool startsline_v;
  map <ustring, bool> startsline;
  UsfmFunctionType function_v;
  map <ustring, UsfmFunctionType> function;
};


#endif
