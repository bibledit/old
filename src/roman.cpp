/*
** Copyright (©) 2003-2009 Teus Benschop.
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

#include "roman.h"
#include "tiny_utilities.h"

ustring integer2roman(int arabic)
// Converts an integer to a Roman numeral.
{
  // Type definition.
  struct roman_digit_type {
    const char *roman;
    int integer;
  };

  // Conversion data.
  const roman_digit_type roman_digits[] = {
    {"M", 1000},
    {"CM", 900},
    {"D", 500},
    {"CD", 400},
    {"C", 100},
    {"XC", 90},
    {"L", 50},
    {"XL", 40},
    {"X", 10},
    {"IX", 9},
    {"V", 5},
    {"IV", 4},
    {"I", 1},
  };

  // Roman digits can't display something like 4999 
  // without using overlaid bars and so forth.
  // Disregarding the above the code below just keeps using the M's.

  // Conversion routine.
  ustring roman;
  for (unsigned int i = 0; arabic && i < sizeof(roman_digits) / sizeof(roman_digits[0]); i++) {
    while (roman_digits[i].integer <= arabic) {
      roman.append(roman_digits[i].roman);
      arabic -= roman_digits[i].integer;
    }
  }

  // Return result.
  return roman;
}

vector < ustring > roman_digits()
// Gives the available roman digits.
{
  vector < ustring > digits;
  digits.push_back("I");
  digits.push_back("V");
  digits.push_back("X");
  digits.push_back("L");
  digits.push_back("C");
  digits.push_back("D");
  digits.push_back("M");
  return digits;
}

ustring integer_or_roman_sample(unsigned int index)
{
  ustring sample;
  for (unsigned int i = 1; i <= 3; i++) {
    if (i > 1) {
      sample.append(", ");
    }
    switch (index) {
    case 1:{
        sample.append(integer2roman(i));
        break;
      }
    default:{
        sample.append(convert_to_string(i));
        break;
      }
    }
  }
  return sample;
}

ustring integer_or_roman_numeral(unsigned int integer, unsigned int index)
{
  ustring numeral;
  switch (index) {
  case 1:{
      numeral = integer2roman(integer);
      break;
    }
  default:{
      numeral = convert_to_string(integer);
      break;
    }
  }
  return numeral;
}
