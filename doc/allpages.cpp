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


#include <exception>            // Standard exception class
#include <stdexcept>            // logic_error and runtime_error
#include <iostream>
#include <cstdlib>
#include <string.h>
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


void quick_swap(string& a, string& b)
{
  string t = a;
  a = b;
  b = t; 
}


void quick_sort(vector<string>& one, vector<string>& two, unsigned int beg, unsigned int end)
{
  if (end > beg + 1) {
    string piv = one[beg];
    unsigned int l = beg + 1;
    unsigned int r = end;
    while (l < r) {
      if (one[l] <= piv) {
        l++;
      } else {
        --r;
        quick_swap(one[l], one[r]);
        quick_swap(two[l], two[r]);
      }
    }
    --l;
    quick_swap(one[l], one[beg]);
    quick_swap(two[l], two[beg]);
    quick_sort(one, two, beg, l);
    quick_sort(one, two, r, end);
  }
}


int main (int argc, char *argv[])
{
  // Read all .html files in the current directory.
  // Leave out some which we don't want.
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
          bool wanted = true;
          if (entries[i] == "index.html") wanted = false;
          if (entries[i] == "allpages.html") wanted = false;
          if (entries[i] == "topbar.html") wanted = false;
          if (entries[i] == "empty.html") wanted = false;
          if (wanted)
            files.push_back (entries[i]);
        }
    }
  }
  // Determine the titles of the pages. 
  // Look for the <h2> tag, and take what's there, else take a default value. 
  // Store result in another container.
  vector<string> titles;
  for (unsigned int i = 0; i < files.size(); i++) {
    string title = "Untitled";
    ifstream in (files[i].c_str());
    string s;
    while (getline (in, s)) {
      if (s.find ("<h2") != string::npos) {
        size_t pos1 = s.find ("</a>");
        if (pos1 != string::npos) {
          pos1 += 4;
          size_t pos2 = s.find ("<", pos1);
          title = s.substr (pos1, pos2 - pos1);
          break;
        }
      }
    }
    titles.push_back (title);
  }
  // Sort containers.
  quick_sort (titles, files, 0, titles.size());  
  // Take existing allpages.html and remove everything between certain tags.
  // Then insert our new data.
  vector<string> allpages;
  vector<string> allpages2;
  {
    bool keepline1 = true;
    bool keepline2 = false;
    ifstream in ("allpages.html");
    string s;
    while (getline (in, s)) {
      if (keepline1)
        allpages.push_back (s);
      if (s.find ("All Pages</h2>") != string::npos)
        keepline1 = false;
      if (s.find ("</body>") != string::npos) {
        keepline2 = true;
      }
      if (keepline2) {
        allpages2.push_back (s);
      }
    }
  }
  for (unsigned int i = 0; i < files.size(); i++) {
    string s = "<p><a href=\"" + files[i] + "\">" + titles[i] + "</a></p>";
    allpages.push_back (s);    
  }
  for (unsigned int i = 0; i < allpages2.size(); i++) {
    allpages.push_back (allpages2[i]);
  }
  // Save result.
  {  
    int fd;
    fd = open ("allpages.html", O_CREAT | O_WRONLY | O_TRUNC, 0666);
    for (unsigned int i = 0; i < allpages.size(); i++) {
      if (write (fd, allpages[i].c_str(), strlen (allpages[i].c_str())));
      if (write (fd, "\n", 1));
    }
    close (fd);
  }
  // Finished.
  return 0;
}
