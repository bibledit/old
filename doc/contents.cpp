/*
** Copyright (C) 2003-2006 Teus Benschop.
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
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


#include <exception>            // Standard exception class
#include <stdexcept>            // logic_error and runtime_error
#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <vector>
#include <set>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <map>
#include <glib.h>


using namespace std;


/*
*/


#define TOC_S "<!-- Table of Contents Start -->"
#define TOC_E "<!-- Table of Contents End -->"


void replace_text (string& line, const string& look_for, const string& replace_with)
{
  size_t offposition = line.find (look_for);
  while (offposition != string::npos) {
    line.replace (offposition, look_for.length (), replace_with);
    offposition = line.find (look_for, offposition + replace_with.length ());
  }
}


string trim (const string & s)
{
  if (s.length () == 0)
    return s;
  size_t beg = s.find_first_not_of (" \t\n\r");
  size_t end = s.find_last_not_of (" \t\n\r");
  if (beg == string::npos)
    return "";
  return string (s, beg, end - beg + 1);
}


int main (int argc, char *argv[])
{
  // Read all .html files in the current directory.
  vector<string> files;
  {  
    GDir *dir = g_dir_open (".", 0, NULL);
    const gchar *s;
    vector <string> entries;
    while ((s = g_dir_read_name (dir)) != NULL)
      entries.push_back (s);
    g_dir_close (dir);    
    for (unsigned int i = 0; i < entries.size (); i++) {
      if (g_str_has_suffix (entries[i].c_str(), ".html"))
        if (!g_file_test (entries[i].c_str(), G_FILE_TEST_IS_DIR)) {
          bool insert = true;
          // Some files should not get a table of contents.
          if (entries[i] == "toolsmenu.html") insert = false;
          if (entries[i] == "tasksmenu.html") insert = false;
          if (entries[i] == "othermenu.html") insert = false;
          if (insert) files.push_back (entries[i]);
        }
    }
  }

  // Go through each file and process it.
  for (unsigned int i = 0; i < files.size(); i++) {

    // Load the text, but remove any previous table of contents.
    vector<string> lines;
    {
      bool keepline = true;
      ifstream in (files[i].c_str());
      string line;
      while (getline (in, line)) {
        if (line == TOC_S)
          keepline = false;
        if (keepline) {
          line = trim (line);
          if (!line.empty()) lines.push_back (line);
        }
        if (line == TOC_E)
          keepline = true;
      }
    }

    // Clean the lines.
    {
      vector<string> cleanlines;
      for (unsigned int i2 = 0; i2 < lines.size(); i2++) {
        bool join = false;
        if (!cleanlines.empty())
          if (cleanlines[cleanlines.size() - 1].substr (0, 2) == "<h")
            if (lines[i2].substr (0, 1) != "<")
              join = true;
        if (i2 == 0) join = false;
        if (join) {
          cleanlines[cleanlines.size() - 1].append (" " + lines[i2]);
        } else 
          cleanlines.push_back (lines[i2]);         
      }
      lines.clear();
      lines.assign (cleanlines.begin(), cleanlines.end());
    }
    
    // Get container with characters allowed in an anchor.
    set <string> anchorchars;
    anchorchars.insert ("a");    
    anchorchars.insert ("b");
    anchorchars.insert ("c");
    anchorchars.insert ("d");
    anchorchars.insert ("e");
    anchorchars.insert ("f");
    anchorchars.insert ("g");
    anchorchars.insert ("h");
    anchorchars.insert ("i");
    anchorchars.insert ("j");
    anchorchars.insert ("k");
    anchorchars.insert ("l");
    anchorchars.insert ("m");
    anchorchars.insert ("n");
    anchorchars.insert ("o");
    anchorchars.insert ("p");
    anchorchars.insert ("q");
    anchorchars.insert ("r");
    anchorchars.insert ("s");
    anchorchars.insert ("t");
    anchorchars.insert ("u");
    anchorchars.insert ("v");
    anchorchars.insert ("w");
    anchorchars.insert ("x");
    anchorchars.insert ("y");
    anchorchars.insert ("z");
    anchorchars.insert ("0");
    anchorchars.insert ("1");
    anchorchars.insert ("2");
    anchorchars.insert ("3");
    anchorchars.insert ("4");
    anchorchars.insert ("5");
    anchorchars.insert ("6");
    anchorchars.insert ("7");
    anchorchars.insert ("8");
    anchorchars.insert ("9");

    // Get the headings in the page and store their levels.
    // Remove any anchors, and add new ones. Store them too.
    vector<string> headings;
    vector<int> levels;
    vector<string> anchors;
    set<string> unique_anchors;
    for (unsigned int i2 = 0; i2 < lines.size(); i2++) {
      string line = lines[i2];
      size_t pos;
      int level = 1;
      pos = line.find ("<h1>");
      if (pos == string::npos) {
        pos = line.find ("<h2>");
        level = 2;
      }
      if (pos == string::npos) {
        pos = line.find ("<h3>");
        level = 3;
      }
      if (pos == string::npos) {
        pos = line.find ("<h4>");
        level = 4;
      }
      if (pos == string::npos) {
        pos = line.find ("<h5>");
        level = 5;
      }
      if (pos == string::npos) {
        pos = line.find ("<h6>");
        level = 6;
      }
      if (pos != string::npos) {
        size_t pos2 = line.find ("<a name");
        if (pos2 != string::npos) {
          size_t pos3 = line.find ("</a>", pos2);
          line.erase (pos2, pos3 - pos2 + 4);
        }
        pos = line.find (">");
        string heading = line.substr (pos + 1, line.length() - pos - 6);
        string anchor (heading);
        for (size_t i3 = 0; i3 < anchor.length (); i3++) {
          anchor[i3] = tolower (anchor[i3]);
        }
        string anchor2;
        for (size_t i3 = 0; i3 < anchor.length (); i3++) {
          if (anchorchars.find (anchor.substr (i3, 1)) != anchorchars.end ())
            anchor2.append (anchor.substr (i3, 1));
        }
        anchor = anchor2;
        if (anchor.empty())
          anchor = "anchor";
        // Check no duplicate anchors.
        while (unique_anchors.find (anchor) != unique_anchors.end ())
          anchor.append ("1");
        unique_anchors.insert (anchor);
        line.insert (++pos, "<a name=\"" + anchor + "\"></a>");
        lines[i2] = line;
        // Store everything.
        headings.push_back (heading);
        levels.push_back (level);
        anchors.push_back (anchor);
      }
    }

    // Insert the table of contents.
    vector<string> lines2;
    bool table_inserted = false;
    for (unsigned int i2 = 0; i2 < lines.size(); i2++) {
      lines2.push_back (lines[i2]);
      if (!table_inserted) {
        if (lines[i2].find ("<h2>") != string::npos) {
          if (headings.size() > 3) {
            // Insert starting comment line and code.
            lines2.push_back (TOC_S);
            lines2.push_back ("<div id=\"toc\">");
            lines2.push_back ("<ul>");            
            // Determine level offset.
            int lowest_level = 1000;
            for (unsigned int i3 = 0; i3 < headings.size(); i3++) {
              if (levels[i3] < lowest_level)
                lowest_level = levels[i3];
            }
            for (unsigned int i3 = 1; i3 < headings.size(); i3++) {
              lines2.push_back ("<li>");
              string link;
              link = "<a href=\"#";
              link.append (anchors[i3]);
              link.append ("\"");
              if (levels[i3] > lowest_level) {
                link.append (" class=\"level");
                ostringstream r;
                r << (levels[i3] - lowest_level);
                link.append (r.str());
                link.append ("\"");
              }
              link.append (">");
              link.append (headings[i3]);
              link.append ("</a></li>");
              lines2.push_back (link);
            }
            // Insert final code and comment line.
            lines2.push_back ("</ul>");
            lines2.push_back ("</div>");          
            lines2.push_back (TOC_E);            
          }  
          table_inserted = true;
        }
      }
    }

    // Save result, but only if there were actual changes.
    {
      // To find change, store the current content of the file.
      vector<string> oldlines;
      ifstream in (files[i].c_str());
      string line;
      while (getline (in, line)) {
        line = trim (line);
        if (!line.empty()) oldlines.push_back (line);
      }
      // Compare the current content with the new, proposed, content.
      bool save = false;
      if (oldlines.size() != lines2.size()) save = true;
      if (!save) {
        for (unsigned int s = 0; s < oldlines.size(); s++) {
          if (oldlines[s] != lines2[s]) save = true;
        }
      }
      // Save data, if needed.
      if (save) {
        int fd;
        fd = open (files[i].c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0666);
        for (unsigned int i2 = 0; i2 < lines2.size(); i2++) {
          write (fd, lines2[i2].c_str(), strlen (lines2[i2].c_str()));
          write (fd, "\n", 1);
        }
        close (fd);
      }
    }
    
  }

  // Finished.
  return 0;
}
