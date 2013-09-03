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
#include "directories.h"
#include "gwrappers.h"
#include "projectutils.h"


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


void merge_editor_and_file (vector <ustring> merge_base,
                            vector <ustring> editor_lines,
                            const ustring& project, unsigned int book, unsigned int chapter)
// Merges the changes from the editor with the changes in the filesystem,
// and stores the result to file.
{
  gw_message ("Merge changes from user and server.");
  // If PHP is not available, skip running the filter to avoid complete data loss.
  if (!gw_find_program_in_path ("php")) {
    gw_message ("Cannot find PHP.");
    gw_message ("Bibledit-Gtk needs PHP for improved collaboration and merging.");
    gw_message ("Some changes were lost.");
    gw_message ("Install package php5-cli.");
    return;
  }
  ustring basefile = gw_build_filename (directories_get_temp(), "mergebase.txt");
  write_lines (basefile, merge_base);
  ustring userfile = gw_build_filename (directories_get_temp(), "mergeuser.txt");
  write_lines (userfile, editor_lines);
  vector <ustring> server_lines = project_retrieve_chapter (project, book, chapter);
  ustring serverfile = gw_build_filename (directories_get_temp(), "mergeserver.txt");
  write_lines (serverfile, server_lines);
  ustring outputfile = gw_build_filename (directories_get_temp(), "mergeoutput.txt");
  GwSpawn spawn ("php");
  spawn.arg (gw_build_filename (directories_get_package_data (), "mergecli.php"));
  spawn.arg (basefile);
  spawn.arg (userfile);
  spawn.arg (serverfile);
  spawn.arg (outputfile);
  spawn.read ();
  spawn.run ();
  for (unsigned int i = 0; i < spawn.standardout.size(); i++) {
    gw_message (spawn.standardout[i]);
  }
  for (unsigned int i = 0; i < spawn.standarderr.size(); i++) {
    gw_message (spawn.standarderr[i]);
  }
  ReadText rt (outputfile, true);
  CategorizeChapterVerse ccv (rt.lines);
  project_store_chapter (project, book, ccv);
}
