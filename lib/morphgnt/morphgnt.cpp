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


#include "morphgnt.h"
#include <sqlite3.h>


vector <string> explode (string value, char delimiter)
{
  vector <string> result;
  istringstream iss (value);
  for (string token; getline (iss, token, delimiter); )
  {
    result.push_back (move (token));
  }
  return result;
}


string file_get_contents (string filename)
{
  ifstream ifs (filename.c_str(), ios::in | ios::binary | ios::ate);
  streamoff filesize = ifs.tellg();
  ifs.seekg (0, ios::beg);
  vector <char> bytes ((int)filesize);
  ifs.read (&bytes[0], (int)filesize);
  return string (&bytes[0], (int)filesize);
}


int convert_to_int (string s)
{
  int i = atoi (s.c_str());
  return i;
}


string convert_to_string (string s)
{
  return s;
}


string convert_to_string (int i)
{
  ostringstream r;
  r << i;
  return r.str();
}


int main (int argc, char **argv)
{
  unlink ("morphgnt.sqlite");
  sqlite3 *db;
  sqlite3_open ("morphgnt.sqlite", &db);
  sqlite3_exec (db, "PRAGMA synchronous = OFF;", NULL, NULL, NULL);
  sqlite3_exec (db, "CREATE TABLE IF NOT EXISTS morphgnt (book integer, chapter integer, verse integer, pos text, parsing text, word text, lemma text);", NULL, NULL, NULL);

  vector <string> files;
  DIR * dir = opendir (".");
  struct dirent * direntry;
  while ((direntry = readdir (dir)) != NULL) {
    string name = direntry->d_name;
    if (name.find ("morphgnt.txt") == string::npos) continue;
    files.push_back (name);
  }
  closedir (dir);

  for (auto file : files) {
    cout << file << endl; // Todo
    string contents = file_get_contents (file);
    vector <string> lines = explode (contents, '\n');
    for (auto line : lines) {
      vector <string> bits = explode (line, ' ');
      if (bits.size () != 7) {
        cout << line << endl;
        cout << "Should be seven bits" << endl;
        return 0;
      }

      string passage = bits [0];
      int book = convert_to_int (passage.substr (0, 2)) + 39;
      int chapter = convert_to_int (passage.substr (2, 2));
      int verse = convert_to_int (passage.substr (4, 2));
      string pos = bits[1];
      string parsing = bits[2];
      string word = bits[3];
      string lemma = bits[6];
      
      string sql =
      "INSERT INTO morphgnt VALUES ("
      + convert_to_string (book)
      + ", "
      + convert_to_string (chapter)
      + ", "
      + convert_to_string (verse)
      + ", "
      + convert_to_string ("'" + pos + "'")
      + ", "
      + convert_to_string ("'" + parsing + "'")
      + ", "
      + convert_to_string ("'" + word + "'")
      + ", "
      + convert_to_string ("'" + lemma + "'")
      + ");";
      char *error = NULL;
      int rc = sqlite3_exec (db, sql.c_str(), NULL, NULL, &error);
      if (rc != SQLITE_OK) {
        cout << sql << endl;
        cout << error << endl;
        return 0;
      }
    }
  }

  sqlite3_close (db);

  return 0;
}
