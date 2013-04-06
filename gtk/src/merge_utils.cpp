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

#include "merge_utils.h"
#include "utilities.h"
#include "tiny_utilities.h"
#include "usfmtools.h"

ustring merge_conflict_markup(unsigned int number)
// Gives the text of the conflict markup.
// Number: 1..3.
{
  ustring markup;
  switch (number) {
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

MergeButton::MergeButton(int dummy)
{
}

MergeButton::MergeButton()
{
}

MergeButtonPair::MergeButtonPair(int dummy)
{
}

MergeButtonPair::MergeButtonPair()
{
}

ApproveButton::ApproveButton(int dummy)
{
}

ApproveButton::ApproveButton()
{
}

Patch::Patch(int dummy)
{
}

Patch::Patch()
{
}

vector < Patch > merge_read_patch(const ustring & filename)
// Reads patches from file.
{
  vector < Patch > patches;
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

const gchar *merge_new_line_indicator()
{
  return "__new__line__indicator__\n";
}

const gchar *merge_verse_indicator()
{
  return "___verse___indicator___";
}

ustring merge_split_data(const ustring & data)
{
  ustring splitdata;
  ParseLine parseline(data);
  for (unsigned int l = 0; l < parseline.lines.size(); l++) {
    ustring s(parseline.lines[l]);
    ustring v = usfm_extract_marker(s);
    if (v == "v") {
      size_t position = s.find(" ");
      position = CLAMP(position, 0, s.length());
      s = s.substr(0, position);
      s.insert(0, " ");
      v = merge_verse_indicator();
      for (unsigned int i = 0; i < 10; i++) {
        v.append(s);
      }
      v.append("\n");
      for (unsigned int i = 0; i < 5; i++) {
        splitdata.append(v);
      }
    }
    Parse parse(parseline.lines[l], false);
    for (unsigned int w = 0; w < parse.words.size(); w++) {
      splitdata.append(parse.words[w]);
      splitdata.append("\n");
    }
    if (l != (parseline.lines.size() - 1))
      splitdata.append(merge_new_line_indicator());
  }
  return splitdata;
}

ustring merge_join_data(const ustring & data)
{
  ustring joineddata;
  Parse parse(data, false, merge_new_line_indicator());
  for (unsigned int p = 0; p < parse.words.size(); p++) {
    ParseLine parseline(parse.words[p]);
    for (unsigned int pl = 0; pl < parseline.lines.size(); pl++) {
      if (parseline.lines[pl].find(merge_verse_indicator()) == 0)
        continue;
      joineddata.append(parseline.lines[pl]);
      if (pl != (parseline.lines.size() - 1))
        joineddata.append(" ");
    }
    joineddata.append("\n");
  }
  return joineddata;
}

ustring merge_split_data_character(const ustring & data)
// Splits the data on the character level, so preparing it for running a diff.
{
  ustring splitdata;
  ParseLine parseline(data);
  for (unsigned int l = 0; l < parseline.lines.size(); l++) {
    ustring s(parseline.lines[l]);
    ustring v = usfm_extract_marker(s);
    if (v == "v") {
      size_t position = s.find(" ");
      position = CLAMP(position, 0, s.length());
      s = s.substr(0, position);
      s.insert(0, " ");
      v = merge_verse_indicator();
      for (unsigned int i = 0; i < 10; i++) {
        v.append(s);
      }
      v.append("\n");
      for (unsigned int i = 0; i < 5; i++) {
        splitdata.append(v);
      }
    }
    for (unsigned int c = 0; c < parseline.lines[l].size(); c++) {
      splitdata.append(parseline.lines[l].substr(c, 1));
      splitdata.append("\n");
    }
    if (l != (parseline.lines.size() - 1))
      splitdata.append(merge_new_line_indicator());
  }
  return splitdata;
}

ustring merge_join_data_character(const ustring & data)
// Joins data previously split.
{
  ustring joineddata;
  Parse parse(data, false, merge_new_line_indicator());
  for (unsigned int p = 0; p < parse.words.size(); p++) {
    ParseLine parseline(parse.words[p]);
    for (unsigned int pl = 0; pl < parseline.lines.size(); pl++) {
      if (parseline.lines[pl].find(merge_verse_indicator()) == 0)
        continue;
      joineddata.append(parseline.lines[pl]);
    }
    joineddata.append("\n");
  }
  return joineddata;
}
