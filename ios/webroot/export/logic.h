/*
 Copyright (©) 2003-2015 Teus Benschop.
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


#ifndef INCLUDED_EXPORT_LOGIC_H
#define INCLUDED_EXPORT_LOGIC_H


#include <config/libraries.h>


class Export_Logic
{
public:
  static void scheduleAll ();
  static void scheduleTextAndBasicUsfm (string bible);
  static void scheduleUsfm (string bible);
  static void scheduleOpenDocument (string bible);
  static void scheduleInfo (string bible);
  static void scheduleHtml (string bible);
  static void scheduleWeb (string bible);
  static void scheduleWebIndex (string bible);
  static void scheduleOnlineBible (string bible);
  static void scheduleESword (string bible);
  static string mainDirectory ();
  static string bibleDirectory (string bible);
  static string USFMdirectory (string bible, int type);
  static string webDirectory (string bible);
  static string webBackLinkDirectory (string bible);
  static string baseBookFileName (int book);
  static string osisDirectory (string bible);
  static string osisSwordVariantDirectory (string interpreter, int complexity);
private:
};


#endif
