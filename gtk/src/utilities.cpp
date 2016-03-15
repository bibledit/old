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
#include "utilities.h"
#include <glib.h>
#include "constants.h"
#include "gwrappers.h"
#include "directories.h"
#include <errno.h>
#include "shell.h"
#include "tiny_utilities.h"
#include <glib/gi18n.h>

#define MY_NUMBERS "0123456789"


ustring number_in_string(const ustring & str)
{
  // Looks for and returns a positive number in a string.
  ustring output = str;
  output.erase(0, output.find_first_of(MY_NUMBERS));
  size_t end_position = output.find_first_not_of(MY_NUMBERS);
  if (end_position != string::npos) {
    output.erase(end_position, output.length());
  }
  return output;
}


#undef MY_NUMBERS


unsigned int digit_count_in_string(const ustring & str)
{
  unsigned int digitcount = 0;
  string s(str);
  for (unsigned int i = 0; i < s.length(); i++) {
    if (g_ascii_isdigit(guchar(s[i])))
      digitcount++;
  }
  return digitcount;
}


ustring upperCase(const ustring & s)
{
// Make an uppercase copy of s
  string upper(s);
  for (size_t i = 0; i < s.length(); ++i)
    upper[i] = toupper(upper[i]);
  return upper;
}


ustring lowerCase(const ustring & s)
{
// Make a lowercase copy of s
  string lower(s);
  for (size_t i = 0; i < s.length(); ++i)
    lower[i] = tolower(lower[i]);
  return lower;
}


ustring remove_spaces(const ustring & s)
{
  ustring s2 = s;
  size_t spacepos = s2.find(" ");
  while (spacepos != string::npos) {
    s2.erase(spacepos, 1);
    spacepos = s2.find(" ");
  }
  return s2;
}


void write_lines(const ustring & file, vector < ustring > &lines)
{
  WriteText wt(file);
  for (unsigned int i = 0; i < lines.size(); i++) {
    wt.text(lines[i]);
    wt.text("\n");
  }
}


ustring temporary_file(const ustring & filename)
{
  return gw_build_filename(Directories->get_temp(), filename);
}


ustring string_reverse(const ustring & s)
{
  ustring returnvalue;
  for (int i = s.length() - 1; i >= 0; i--)
    returnvalue.append(s.substr(i, 1));
  return returnvalue;
}


ustring double_apostrophy(const ustring & line)
{
  /*
     SQLite needs any apostrophy in the data to be prefixed by another one.
     This function does that.
   */
  ustring returnvalue;
  returnvalue = line;
  size_t offset = returnvalue.find("'");
  while (offset != string::npos) {
    returnvalue.insert(offset, "'");
    offset++;
    offset++;
    offset = returnvalue.find("'", offset);
  }
  return returnvalue;
}


unsigned int file_get_modification_time(const ustring & filename)
{
  struct stat statbuf;
  stat(filename.c_str(), &statbuf);
  return statbuf.st_mtime;
}


unsigned int file_get_size(const ustring & filename)
{
  struct stat statbuf;
  stat(filename.c_str(), &statbuf);
  return statbuf.st_size;
}


void textbuffer_get_lines(GtkTextBuffer * buffer, vector < ustring > &lines, bool trimline)
// Reads all the lines in the textbuffer.
{
  // We need to know the number of lines.
  int number_of_lines = gtk_text_buffer_get_line_count(buffer);
  GtkTextIter iterator;
  GtkTextIter endofline;
  // Get all lines.
  for (int i = 0; i < number_of_lines; i++) {
    gtk_text_buffer_get_iter_at_line(buffer, &iterator, i);
    // Ensure that also the last line, without a newline character, gets taken.
    if (i + 1 == number_of_lines) {
      gtk_text_buffer_get_end_iter(buffer, &endofline);
    } else {
      gtk_text_buffer_get_iter_at_line(buffer, &endofline, i + 1);
      gtk_text_iter_backward_char(&endofline);
    }
    // Get the line.
    ustring line = gtk_text_buffer_get_text(buffer, &iterator, &endofline, false);
    // Trim it.
    if (trimline)
      line = trim(line);
    // Store it.
    lines.push_back(line);
  }
}


bool replace_text(ustring & line, const ustring & look_for, const ustring & replace_with)
// Replaces some text. Returns true if any replacement was done.
{
  bool replacements_done = false;
  size_t offposition = line.find (look_for);
  while (offposition != string::npos) {
    line.replace (offposition, look_for.length (), replace_with);
    offposition = line.find (look_for, offposition + replace_with.length ());
    replacements_done = true;
  }
  return replacements_done;
}


bool replace_text_between(ustring & line, const ustring & start, const ustring & end, const ustring & replacement)
// Replaces text that starts with "start" and ends with "end" with "replacement".
// Returns true if replacement was done.
{
  bool replacements_done = false;
  size_t beginpos = line.find(start);
  size_t endpos = line.find(end);
  while ((beginpos != string::npos) && (endpos != string::npos) && (endpos > beginpos)) {
    line.replace(beginpos, endpos - beginpos + end.length(), replacement);
    beginpos = line.find(start, beginpos + replacement.length());
    endpos = line.find(end, beginpos + replacement.length());
    replacements_done = true;
  }
  return replacements_done;
}

// Function template for swapping elements in the container
template<typename T> void quick_swap(T &a, T &b)
{
  T temp = a; a = b; b = temp;
}

// Instantiations of several quick_swap variants...cannot inline these
template void quick_swap<ustring>(ustring &a, ustring &b);
template void quick_swap<long long unsigned> (long long unsigned  &a, long long unsigned &b);
template void quick_swap<int> (int     &a, int     &b);
template void quick_swap<bool>(bool    &a, bool    &b);
template void quick_swap<long unsigned int>(long unsigned int &a, long unsigned int &b);

// Watch for inefficiencies in above, and turn into a macro if so

/*
void quick_swap(ustring & a, ustring & b)
{
  ustring t = a;
  a = b;
  b = t;
}


void quick_swap(unsigned int &a, unsigned int &b)
{
  unsigned int t = a;
  a = b;
  b = t;
}

void quick_swap(size_t &a, size_t &b)
{
  size_t t = a;
  a = b;
  b = t;
}


void quick_swap(long unsigned int &a, long unsigned int &b)
{
  long unsigned int t = a;
  a = b;
  b = t;
}


void quick_swap(int &a, int &b)
{
  int t = a;
  a = b;
  b = t;
}


void quick_swap(bool & a, bool & b)
{
  bool t = a;
  a = b;
  b = t;
}
*/

/*
This function is unusual in the sense that it does not sort one container, as
the big majority of sort functions do, but it accepts two containers.
It sorts on the first, and reorders the second container at the same time, 
following the reordering done in the first container.
*/
// Function template for sorting the containers
template<typename T1, typename T2> void quick_sort(vector<T1> &one, vector <T2> &two, unsigned int beg, unsigned int end)
{
  if (end > beg + 1) {
    T1 piv = one[beg];
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
// Instantiations of several of these special quick_sort variants...cannot inline these
template void quick_sort<unsigned int, ustring>     (vector <unsigned int> &one, vector <ustring>      &two, unsigned int beg, unsigned int end);
template void quick_sort<ustring, unsigned int>     (vector <ustring>      &one, vector <unsigned int> &two, unsigned int beg, unsigned int end);
template void quick_sort<unsigned int, unsigned int>(vector <unsigned int> &one, vector <unsigned int> &two, unsigned int beg, unsigned int end);
template void quick_sort<long long unsigned int, long long unsigned int>(vector<long long unsigned int>&one, vector<long long unsigned int>& two, unsigned int beg, unsigned int end);
template void quick_sort<int, unsigned int>         (vector <int>          &one, vector<unsigned int>  &two, unsigned int beg, unsigned int end);
template void quick_sort<ustring, ustring>          (vector <ustring>      &one, vector <ustring>      &two, unsigned int beg, unsigned int end);
template void quick_sort<long unsigned int, long unsigned int>(vector <long unsigned int> &one, vector <long unsigned int> &two, unsigned int beg, unsigned int end);

// Similar to above, but "bool" type needs special handling
template<typename T1> void quick_sort(vector<T1> &one, vector <bool> &two, unsigned int beg, unsigned int end)
 {
  if (end > beg + 1) {
    T1 piv = one[beg];
    unsigned int l = beg + 1;
    unsigned int r = end;
    while (l < r) {
      if (one[l] <= piv) {
        l++;
      } else {
        --r;
        quick_swap(one[l], one[r]);
        bool two_l = two[l];
        bool two_r = two[r];
        quick_swap(two_l, two_r);
        two[l] = two_l;
        two[r] = two_r;
      }
    }
    --l;
    quick_swap(one[l], one[beg]);
    bool two_l = two[l];
    bool two_beg = two[beg];
    quick_swap(two_l, two_beg);
    two[l] = two_l;
    two[beg] = two_beg;
    quick_sort(one, two, beg, l);
    quick_sort(one, two, r, end);
  }
}
template void quick_sort<unsigned int> (vector <unsigned int> &one, vector<bool> &two, unsigned int beg, unsigned int end);
template void quick_sort<ustring>      (vector <ustring>      &one, vector<bool> &two, unsigned int beg, unsigned int end);


/*
void quick_sort(vector < unsigned int >&one, vector < ustring > &two, unsigned int beg, unsigned int end)
{
  if (end > beg + 1) {
    unsigned int piv = one[beg];
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

void quick_sort(vector < ustring > &one, vector < unsigned int >&two, unsigned int beg, unsigned int end)
{
  if (end > beg + 1) {
    ustring piv = one[beg];
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

void quick_sort(vector < unsigned int >&one, vector < unsigned int >&two, unsigned int beg, unsigned int end)
{
  if (end > beg + 1) {
    unsigned int piv = one[beg];
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

void quick_sort (vector<long long unsigned int>& one,       vector<long long unsigned int>& two,       unsigned int beg, unsigned int end)
{
  if (end > beg + 1) {
    unsigned int piv = one[beg];
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


void quick_sort (vector<unsigned int>& one, vector<bool>& two, unsigned int beg, unsigned int end)
{
  if (end > beg + 1) {
    unsigned int piv = one[beg];
    unsigned int l = beg + 1;
    unsigned int r = end;
    while (l < r) {
      if (one[l] <= piv) {
        l++;
      } else {
        --r;
        quick_swap(one[l], one[r]);
        bool two_l = two[l];
        bool two_r = two[r];
        quick_swap(two_l, two_r);
        two[l] = two_l;
        two[r] = two_r;
      }
    }
    --l;
    quick_swap(one[l], one[beg]);
    bool two_l = two[l];
    bool two_beg = two[beg];
    quick_swap(two_l, two_beg);
    two[l] = two_l;
    two[beg] = two_beg;
    quick_sort(one, two, beg, l);
    quick_sort(one, two, r, end);
  }
}


void quick_sort(vector < int >&one, vector < unsigned int >&two, unsigned int beg, unsigned int end)
{
  if (end > beg + 1) {
    int piv = one[beg];
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

void quick_sort(vector < ustring > &one, vector < ustring > &two, unsigned int beg, unsigned int end)
{
  if (end > beg + 1) {
    ustring piv = one[beg];
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


void quick_sort(vector < ustring > &one, vector < bool > &two, unsigned int beg, unsigned int end)
{
  if (end > beg + 1) {
    ustring piv = one[beg];
    unsigned int l = beg + 1;
    unsigned int r = end;
    while (l < r) {
      if (one[l] <= piv) {
        l++;
      } else {
        --r;
        quick_swap(one[l], one[r]);
        bool two_l = two[l];
        bool two_r = two[r];
        quick_swap(two_l, two_r);
        two[l] = two_l;
        two[r] = two_r;
      }
    }
    --l;
    quick_swap(one[l], one[beg]);
    bool two_l = two[l];
    bool two_beg = two[beg];
    quick_swap(two_l, two_beg);
    two[l] = two_l;
    two[beg] = two_beg;
    quick_sort(one, two, beg, l);
    quick_sort(one, two, r, end);
  }
}
*/

/* A regular single vector sorting routine */
void quick_sort(vector < ustring > &one, unsigned int beg, unsigned int end)
{
  if (end > beg + 1) {
    ustring piv = one[beg];
    unsigned int l = beg + 1;
    unsigned int r = end;
    while (l < r) {
      if (one[l] <= piv) {
        l++;
      } else {
        --r;
        quick_swap(one[l], one[r]);
      }
    }
    --l;
    quick_swap(one[l], one[beg]);
    quick_sort(one, beg, l);
    quick_sort(one, r, end);
  }
}

/*
void quick_sort(vector <long unsigned int>& one, vector <long unsigned int>& two, unsigned int beg, unsigned int end)
{
  if (end > beg + 1) {
    long unsigned int piv = one[beg];
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
*/

gchar *de_windows_notepad(gchar * contents)
// Some Windows textfiles, probably the ones created with Notepad, have 
// the "feature" to put \xEF\xBB\xBF at the start of the file.
// This function removes those characters.
{
  gchar *returnvalue = contents;
  char *ef_bb_bf = g_strstr_len(contents, 3, "\xef\xbb\xbf");
  if (ef_bb_bf) {
    ef_bb_bf += 3;
    returnvalue = g_strdup(ef_bb_bf);
    g_free(contents);
  }
  return returnvalue;
}


void de_byte_order_mark (ustring& line)
// Some textfiles start with a byte order mark.
// This function remove it.
{
  if (line.find ("﻿") == 0) { // Note that there's text between the quotation marks.
    line.erase (0, 1);
  }
}


ustring spaces(unsigned int count)
// Returns "count" spaces.
{
  ustring space;
  for (unsigned int i = 0; i < count; i++)
    space.append(" ");
  return space;
}


void bitpattern_add(ustring & pattern, bool setting)
// Adds one bit for "setting" to "pattern".
{
  pattern.append(convert_to_string(setting));
}


bool bitpattern_take(ustring & pattern)
// Return the next bit from "pattern" and removes it from that string.
// This implies that settngs from the pattern must be taken in the same order
// that they were added.
{
  bool setting = false;
  if (!pattern.empty()) {
    setting = convert_to_bool(pattern.substr(0, 1));
    pattern.erase(0, 1);
  }
  return setting;
}


ustring character_to_decimal_entity(const ustring & character)
{
  gunichar unichar;
  gunichar *uc;
  uc = g_utf8_to_ucs4_fast(character.c_str(), -1, NULL);
  unichar = *uc;
  g_free(uc);
  gchar *decimal = g_strdup_printf("&#%d;", unichar);
  ustring udec(decimal);
  g_free(decimal);
  return udec;
}


ustring character_to_hexadecimal_entity(const ustring & character)
{
  gunichar unichar;
  gunichar *uc;
  uc = g_utf8_to_ucs4_fast(character.c_str(), -1, NULL);
  unichar = *uc;
  g_free(uc);
  gchar *decimal = g_strdup_printf("U+%04X", unichar);
  ustring udec(decimal);
  g_free(decimal);
  return udec;
}


void string_append_line(ustring & container, const ustring & line)
{
  if (!container.empty()) {
    container.append("\n");
  }
  container.append(line);
}


ustring present_working_directory()
// Gives the present working directory.
{
  char pwd[10000];
  if (getcwd(pwd, 10000)) ;
  ustring s(pwd);
  return s;
}


bool vector_strings_equal (const vector <ustring>& vector1, const vector <ustring>& vector2)
{
  if (vector1.size() != vector2.size())
    return false;
  
  for (unsigned int i = 0; i < vector1.size(); i++)
    if (vector1[i] != vector2[i])
      return false;
  
  return true;
}


int clamp (int in, int min, int max)
{
  if (in < min)
    in = min;
  if (in > max)
    in = max;
  return in;
}


ReadDirectories::ReadDirectories(const ustring & path, const ustring & prefix, const ustring & suffix)
{
  // Reads the directories in directory "path" that end on "suffix".
  // It does not return regular files.
  try {
    GDir *dir = g_dir_open(path.c_str(), 0, NULL);
    const gchar *s;
    vector < ustring > entries;
    while ((s = g_dir_read_name(dir)) != NULL)
      entries.push_back(s);
    g_dir_close(dir);
    for (unsigned int i = 0; i < entries.size(); i++) {
      if (g_str_has_suffix(entries[i].c_str(), suffix.c_str()))
        if (g_str_has_prefix(entries[i].c_str(), prefix.c_str()))
          if (g_file_test(gw_build_filename(path, entries[i]).c_str(), G_FILE_TEST_IS_DIR))
            directories.push_back(entries[i]);
    }
  }
  catch(...) {
  }
}


ReadDirectories::~ReadDirectories()
{
}


ReadFiles::ReadFiles(const ustring & path, const ustring & prefix, const ustring & suffix)
{
  // Reads the regular files in directory "path" that end on "suffix".
  // It does not return directories.
  try {
    GDir *dir = g_dir_open(path.c_str(), 0, NULL);
    const gchar *s;
    vector < ustring > entries;
    while ((s = g_dir_read_name(dir)) != NULL)
      entries.push_back(s);
    g_dir_close(dir);
    for (unsigned int i = 0; i < entries.size(); i++) {
      if (g_str_has_suffix(entries[i].c_str(), suffix.c_str()))
        if (g_str_has_prefix(entries[i].c_str(), prefix.c_str()))
          if (!g_file_test(gw_build_filename(path, entries[i]).c_str(), G_FILE_TEST_IS_DIR))
            files.push_back(entries[i]);
    }
  }
  catch(...) {
  }
}


ReadFiles::~ReadFiles()
{
}


ReadText::ReadText(const ustring & file, bool silent, bool trimming)
{
  // Reads the text and stores it line by line, trimmed, into "lines".
  // If "silent" is true, then no exception will be thrown in case of an error.
  // The lines will be trimmed if "trimming" is true.
  ifstream in(file.c_str());
  if (!in) {
    if (!silent) {
      cerr << _("Error opening file ") << file << endl;
      throw;
    }
    return;
  }
  string s;
  while (getline(in, s)) {
    if (trimming)
      s = trim(s);
    lines.push_back(s);
  }
}


ReadText::~ReadText()
{
}


WriteText::WriteText(const ustring & file)
{
/*
  This opens a textfile for writing.
  At first it uses the streaming system (e.g. out << text << endl), but because
  this causes crashes on Macintosh when writing Unicode, this has been changed
  to regular Linux calls: open, write.
*/
  fd = open(file.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0666);
  if (fd < 0) {
    gw_critical(_("Error creating file ") + file);
    perror(NULL);
  }
}


WriteText::~WriteText()
{
  close(fd);
}


void WriteText::text(const ustring & text)
// Write the text. For calculating the lenght, do not use text.length(),
// because this gives the number of unicode characters, not the length in bytes. 
// Use strlen () instead.
{
  if (write(fd, text.c_str(), strlen(text.c_str()))) ;
}


Parse::Parse(const ustring & line, bool remove_punctuation, const ustring & separator)
// Parses a line of text in its separate words.
// remove_punctuation: remove some standard punctuation.
// separator: standard this is a space, but can be modified.
{
  ustring processed_line;
  processed_line = trim(line);
  processed_line.append(separator);
  size_t spaceposition;
  spaceposition = processed_line.find(separator);
  while (spaceposition != string::npos) {
    ustring word = processed_line.substr(0, spaceposition);
    if (remove_punctuation) {
      string::size_type location = word.find_last_of(".,;:");
      if (location != string::npos)
        word = word.substr(0, location);
    }
    words.push_back(word);
    processed_line.erase(0, spaceposition + separator.length());
    spaceposition = processed_line.find(separator);
  }
}


Parse::~Parse()
{
}


ParseWords::ParseWords(const ustring & text)
// Parses a line of text in its separate words.
// Note: This is comparable to object Parse, but does a better job.
{
  // Load text into buffer.
  ustring text2(text);
  text2.append(" ");
  GtkTextBuffer *textbuffer;
  textbuffer = gtk_text_buffer_new(NULL);
  gtk_text_buffer_set_text(textbuffer, text2.c_str(), -1);
  // Iterators.  
  GtkTextIter startiter, enditer;
  // Parse into separate words.
  gtk_text_buffer_get_start_iter(textbuffer, &enditer);
  while (gtk_text_iter_forward_word_end(&enditer)) {
    startiter = enditer;
    gtk_text_iter_backward_word_start(&startiter);
    ustring word = gtk_text_iter_get_text(&startiter, &enditer);
    words.push_back(word);
  }
  // Free memory
  g_object_unref(textbuffer);
}
