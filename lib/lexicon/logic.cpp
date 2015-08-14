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


#include <lexicon/logic.h>
#include <filter/url.h>
#include <filter/string.h>
#include <database/strong.h>


// Clean up the Strong's number.
string lexicon_logic_strong_number_cleanup (string strong)
{
  // When a Strong's number is given as e.g. "H01", change it to "0".
  strong = filter_string_str_replace ("H0", "H", strong);
  
  return strong;
}


// Gets the text to show when the mouse cursor hovers above a Strong's definition.
string lexicon_logic_strong_hover_text (string strong)
{
  Database_Strong database_strong;
  string text = database_strong.meaning (strong);
  if (text.empty ()) text = database_strong.usage (strong);
  filter_string_replace_between (text, "<", ">", "");
  return text;
}
