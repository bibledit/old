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

#include "libraries.h"
#include "notecaller.h"
#include "utilities.h"
#include "constants.h"
#include "tiny_utilities.h"

NoteCaller::NoteCaller(NoteNumberingType numbering_in, ustring user_sequence)
// Deal with the note callers.
{
  numbering = numbering_in;
  sequence = user_sequence;
  if (numbering == nntAlphabetical)
    sequence = ALPHABET;
  reset();
}

NoteCaller::~NoteCaller()
{
}

void NoteCaller::reset()
{
  caller_pointer = 0;
}

ustring NoteCaller::get_caller()
{
  ustring caller;
  switch (numbering) {
  case nntNumerical:
    {
      caller = convert_to_string(caller_pointer + 1);
      break;
    }
  case nntAlphabetical:
  case nntUserDefined:
    {
      if (caller_pointer >= sequence.length())
        caller_pointer = 0;
      caller = sequence[caller_pointer];
      break;
    }
  }
  caller_pointer++;
  return caller;
}

ustring note_caller_numbering_per_page_pool()
// This provides a pool of characters, to be used as temporary placeholders 
// for footnote numbers to be renumbered per page later on.
// The "Linear B" script is used as it is not expected that these 
// characters ever be used in a Bible.
{
  return "𐀀𐀁𐀂𐀃𐀄𐀅𐀆𐀇𐀈𐀉𐀊𐀋𐀍𐀎𐀏𐀐𐀑𐀒𐀓𐀔𐀕𐀖𐀗𐀘𐀙𐀚𐀛𐀜𐀝𐀞𐀟𐀠𐀡𐀢𐀣𐀤𐀥𐀦𐀨𐀩𐀪𐀫𐀬𐀭𐀮𐀯𐀰𐀱𐀲𐀳𐀴𐀵𐀶𐀷𐀸𐀹𐀺𐀼𐀽𐀿";
}
