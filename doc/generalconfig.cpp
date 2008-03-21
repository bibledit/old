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
Generates lists of keys that can be used to access Bibledit's configuration.
*/


#define GEN_S "<!-- General Configuration Start -->"
#define GEN_E "<!-- General Configuration End -->"


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
  // Filenames.
  string genconfigfilename = "../../src/generalconfig.cpp";
  string htmlfilename = "scripting_configuration.html";

  // Load the source code of the general configuration.
  vector<string> genlines;
  {
    ifstream in (genconfigfilename.c_str());
    string line;
    while (getline (in, line)) {
      genlines.push_back (line);
    }
  }

  // Extract all keys and comments.
  vector<string> general_keys;
  vector<string> general_comments;
  vector<string> general_types;
  bool readcomment = false;
  bool readkey = false;
  string comment;
  string functiontype;
  for (unsigned int i = 0; i < genlines.size(); i++) {
    if (readcomment) {
      if (!comment.empty()) 
        comment.append (" ");
      string line = trim (genlines[i]);
      if (line != "{")
        comment.append (line);
    }
    if (readkey) {
      if (genlines[i].find ("return") != string::npos) {
        string line = trim (genlines[i]);
        size_t pos = line.find ("\"");
        line.erase (0, ++pos);
        pos = line.find ("\"");
        line.erase (pos, 1000);
        string key (line);
        replace_text (comment, "/*", "");
        replace_text (comment, "*/", "");
        replace_text (comment, "//", "");
        comment = trim (comment);
        general_keys.push_back (key);
        general_comments.push_back (comment);
        general_types.push_back (functiontype);
        comment.clear();
        readkey = false;
      }
    }
    size_t colonposition = genlines[i].find ("::");
    if (colonposition != string::npos) {
      size_t _keyposition = genlines[i].find ("_key");
      if (_keyposition != string::npos) {
        readcomment = true;
        string functionname;         
        functionname = genlines[i].substr (colonposition + 2, _keyposition - colonposition - 2);
        functiontype.clear();
        for (unsigned int i2 = i + 6; i2 < genlines.size(); i2++) {
          if (genlines[i2].find (functionname) != string::npos) {
            string line (genlines[i2]);
            replace_text (line, " ", "");
            if (line.substr (0, 4) == "bool")
              functiontype = "boolean";
            if (line.substr (0, 3) == "int")
              functiontype = "integer";
            if (line.substr (0, 7) == "ustring")
              functiontype = "string";
            if (line.substr (0, 6) == "double")
              functiontype = "floating point";
            if (line.substr (0, 12) == "vector<bool>")
              functiontype = "boolean list";
            if (line.substr (0, 11) == "vector<int>")
              functiontype = "integer list";
            if (line.substr (0, 15) == "vector<ustring>")
              functiontype = "string list";
            if (line.substr (0, 14) == "vector<double>")
              functiontype = "floating point list";
            break;
          }
        }
      }
    }
    if (genlines[i].find ("{") != string::npos) {
      if (readcomment) readkey = true;
      readcomment = false;
    }
  }

  // Load the html text.
  // Remove any previous table of keys.
  vector<string> htmllines;
  {
    bool keepline = true;
    ifstream in (htmlfilename.c_str());
    string line;
    while (getline (in, line)) {
      if (line == GEN_E)
        keepline = true;
      if (keepline)
        htmllines.push_back (line);
      if (line == GEN_S)
        keepline = false;
    }
  }


  // Insert the keys and comments into the html file.
  vector <string> htmlout;
  {
    for (unsigned int i = 0; i < htmllines.size(); i++) {
      htmlout.push_back (htmllines[i]);
      if (htmllines[i] == GEN_S) {
        /*
        <table border="0">
          <caption></caption>
            <tbody>
             <tr>
               <td>sync_all</td>
               <td>Syncs all</td>
               <td>boolean</td>
             </tr>
           </tbody>
         </table>
        */     
        htmlout.push_back ("<table border=\"0\">");
        htmlout.push_back ("  <caption></caption>");
        htmlout.push_back ("  <tbody>");
        for (unsigned int i2 = 0; i2 < general_keys.size(); i2++) {
          htmlout.push_back ("     <tr>");
          htmlout.push_back ("       <td>" + general_keys[i2] + "</td>");
          htmlout.push_back ("       <td>" + general_comments[i2] + "</td>");
          htmlout.push_back ("       <td>" + general_types[i2] + "</td>");
          htmlout.push_back ("     </tr>");
        }
        htmlout.push_back ("  </tbody>");
        htmlout.push_back ("</table>");
      }
    }
  }
  
  // Save result.
  {  
    int fd;
    fd = open (htmlfilename.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0666);
    for (unsigned int i2 = 0; i2 < htmlout.size(); i2++) {
      write (fd, htmlout[i2].c_str(), strlen (htmlout[i2].c_str()));
      write (fd, "\n", 1);
    }
    close (fd);
  }

  // Check on double keys.
  {
    set <string> double_keys;
    for (unsigned int i = 0; i < general_keys.size(); i++) {
      if (double_keys.find (general_keys[i]) != double_keys.end()) {
        cerr << "Key already in use: " << general_keys[i] << endl;
      }
      double_keys.insert (general_keys[i]);
    }
  }
  
  // Finished.
  return 0;
}
