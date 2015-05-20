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


#include <locale/logic.h>
#include <locale/translate.h>
#include <filter/string.h>


// Filters out the default language.
// Returns the $localization, or "" in case of the default language.
string locale_logic_filter_default_language (string localization)
{
  if (localization == "default") localization = "";
  return localization;
}


// Takes a month from 1 to 12, and returns its localized name.
string locale_logic_month (int month)
{
  switch (month) {
    case 1:  return translate ("January");
    case 2:  return translate ("February");
    case 3:  return translate ("March");
    case 4:  return translate ("April");
    case 5:  return translate ("May");
    case 6:  return translate ("June");
    case 7:  return translate ("July");
    case 8:  return translate ("August");
    case 9:  return translate ("September");
    case 10: return translate ("October");
    case 11: return translate ("November");
    case 12: return translate ("December");
  }
  return translate ("Month") + " " + convert_to_string (month);
}

