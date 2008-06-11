/*
 ** Copyright (©) 2003-2008 Teus Benschop.
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
 ** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 **  
 */

#include "merge_utils.h"
#include "utilities.h"
#include "tiny_utilities.h"

ustring merge_conflict_markup(unsigned int number)
// Gives the text of the conflict markup.
// Number: 1..3.
{
  ustring markup;
  switch (number)
  {
    case 1:
    {
      markup = "before";
      break;
    }
    case 2:
    {
      markup = "middle";
      break;
    }
    case 3:
    {
      markup = "after";
      break;
    }
  }
  if (!markup.empty()) {
    markup.insert(0, "__conflict__marker__");
    markup.append("__");
  }
  return markup;
}

MergeButton::MergeButton(int dummy) {
}

MergeButton::MergeButton() {
}

MergeButtonPair::MergeButtonPair(int dummy) {
}

MergeButtonPair::MergeButtonPair() {
}

ApproveButton::ApproveButton(int dummy) {
}

ApproveButton::ApproveButton() {
}

Patch::Patch(int dummy) {
}

Patch::Patch() {
}

vector <Patch> merge_read_patch(const ustring& filename)
// Reads patches from file.
{
  vector <Patch> patches;
  ReadText rt(filename, true, false);
  for (unsigned int i = 0; i < rt.lines.size(); i++) {
    unsigned int linenumber = convert_to_int(number_in_string(rt.lines[i]));
    bool addition = rt.lines[i].find("a") != string::npos;
    while (++i < rt.lines.size()) {
      if (rt.lines[i].find_first_of("<>") == 0) {
        rt.lines[i].erase(0, 2);
        Patch patch(0);
        patch.linenumber = linenumber;
        patch.addition = addition;
        patch.change = (rt.lines[i]);
        patches.push_back(patch);
        if (!addition)
          linenumber++;
      } else {
        i--;
        break;
      }
    }
  }
  return patches;
}
