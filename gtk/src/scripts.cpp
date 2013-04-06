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

#include "scripts.h"
#include "unixwrappers.h"
#include "directories.h"
#include "gwrappers.h"
#include "utilities.h"
#include "shell.h"
#include "tiny_utilities.h"

ustring scripts_straight_through()
{
  return "straight through";
}

ustring script_prefix(ScriptType scripttype)
{
  ustring prefix = "script_";
  switch (scripttype) {
  case stSed:
    prefix.append("sed_");
    break;
  case stTECkit:
    prefix.append("teckit_");
    break;
  case stFree:
    prefix.append("free_");
    break;
  case stEnd:
    break;
  }
  return prefix;
}

ustring script_suffix(ScriptType scripttype, bool binary)
{
  ustring suffix;
  switch (scripttype) {
  case stSed:
    break;
  case stTECkit:
    if (binary)
      suffix.append(".tec");
    else
      suffix.append(".map");
    break;
  case stFree:
  case stEnd:
    break;
  }
  return suffix;
}

vector < ustring > scripts_get_all()
// Gets a list of available scripts.
{
  // Read available scripts and clean them up.
  ReadFiles rf(directories_get_scripts(), script_prefix(stEnd), "");
  for (unsigned int i = 0; i < rf.files.size(); i++) {
    for (int scripttype = stSed; scripttype < stEnd; scripttype++) {
      ustring prefix = script_prefix((ScriptType) scripttype);
      ustring source_suffix = script_suffix((ScriptType) scripttype, false);
      ustring binary_suffix = script_suffix((ScriptType) scripttype, true);
      if (g_str_has_prefix(rf.files[i].c_str(), prefix.c_str())) {
        rf.files[i].erase(0, prefix.length());
      }
      if (!source_suffix.empty()) {
        if (g_str_has_suffix(rf.files[i].c_str(), source_suffix.c_str())) {
          rf.files[i].erase(rf.files[i].length() - source_suffix.length(), source_suffix.length());
        }
      }
      if (!binary_suffix.empty()) {
        if (g_str_has_suffix(rf.files[i].c_str(), binary_suffix.c_str())) {
          rf.files[i].erase(rf.files[i].length() - binary_suffix.length(), binary_suffix.length());
        }
      }
    }
  }

  // In cases of TECkit, there is the .map script and the compiled .tec script.
  // These both have the same name, so we have two scripts of the same name.
  // Remove any double names.
  set < ustring > scriptset(rf.files.begin(), rf.files.end());
  vector < ustring > scripts(scriptset.begin(), scriptset.end());

  // Sort them.
  sort(scripts.begin(), scripts.end());

  // Add the "straight through" script at the start.
  scripts.insert(scripts.begin(), scripts_straight_through());

  // Return them.
  return scripts;
}

bool script_available(const ustring & script)
// Returns whether "script" is available.
{
  vector < ustring > scripts = scripts_get_all();
  set < ustring > scripts_set(scripts.begin(), scripts.end());
  return scripts_set.find(script) != scripts_set.end();
}

ustring script_filter(const ustring & scriptname, bool straightthrough, const ustring & inputfile, const ustring & outputfile)
/*
 Runs the filter "scriptname".
 Input text in "inputfile", and the output text goes in "outputfile".
 If everything is okay, it returns nothing.
 If there were errors, it returns these.
 */
{
  // Remove any previous output.
  unlink(outputfile.c_str());
  unlink(script_temporal_error_file().c_str());

  // Handle straight through.
  if (straightthrough) {
    unix_cp(inputfile, outputfile);
    return "";
  }
  // Get the filename and the type of the script.
  ScriptType scripttype;
  ustring scriptfile = script_get_path(scriptname, &scripttype, true);

  // If the rules file does not exist, or the script is of an unknown type, pass it straight through.
  if (!g_file_test(scriptfile.c_str(), G_FILE_TEST_IS_REGULAR) || (scripttype == stEnd)) {
    unix_cp(inputfile, outputfile);
    gw_warning("Error in script " + scriptname);
    return "";
  }
  // Encode the input usfm file.
  ustring encodedinputfile = script_temporal_input_file();
  if (inputfile != encodedinputfile) {
    unix_cp(inputfile, encodedinputfile);
  }
  script_encode_usfm_file(encodedinputfile);

  // Run filter.
  ustring command;
  ustring error;
  switch (scripttype) {
  case stSed:
    {
      command.append(script_sed_binary());
      command.append(" -f");
      command.append(shell_quote_space(scriptfile));
      command.append("<");
      command.append(shell_quote_space(encodedinputfile));
      command.append(">");
      command.append(shell_quote_space(outputfile));
      break;
    }
  case stTECkit:
    {
      command.append(script_teckit_txtconverter());
      command.append(" -i");
      command.append(shell_quote_space(encodedinputfile));
      command.append(" -o");
      command.append(shell_quote_space(outputfile));
      command.append(" -t");
      command.append(shell_quote_space(scriptfile));
      command.append(" -nobom");
      break;
    }
  case stFree:
    {
      // Text of the script.
      ustring scriptdata;
      {
        // Read script.
        gchar *contents;
        g_file_get_contents(scriptfile.c_str(), &contents, NULL, NULL);
        if (contents) {
          scriptdata = contents;
          g_free(contents);
        } else {
          error = "Can't read script file";
          gw_warning(error);
          return error;
        }
      }
      // Check for and insert the input filename.
      if (scriptdata.find(script_free_input_identifier()) == string::npos) {
        error = "Can't find where to put input file";
        gw_warning(error);
        return error;
      }
      replace_text(scriptdata, script_free_input_identifier(), shell_quote_space(encodedinputfile));
      // Check for and insert the output filename.
      if (scriptdata.find(script_free_output_identifier()) == string::npos) {
        error = "Can't find where to put output file";
        gw_warning(error);
        return error;
      }
      replace_text(scriptdata, script_free_output_identifier(), shell_quote_space(outputfile));
      // Write temporal script.
      g_file_set_contents(script_temporal_script_file().c_str(), scriptdata.c_str(), -1, NULL);
      // Assemble command to run.
      command.append("sh");
      command.append(shell_quote_space(script_temporal_script_file()));
      break;
    }
  case stEnd:
    {
      break;
    }
  }

  // Add the error file to the command, and run it.
  command.append(" 2> ");
  command.append(script_temporal_error_file());
  int result = system(command.c_str()); // This one is too unpredictable to be used with GwSpawn.

  // The filters are so much beyond any control that we never can be sure that 
  // their output is in the UTF-8 encoding.
  // Sed would give UTF-8, but as TECkit can also give legacy encodings.
  // We can't know what free scripts will do, it could be anything.
  // So here check the UTF-8 encoding. 
  // If UTF-8 validation fails, we copy the input straight to the output.
  {
    gchar *contents;
    g_file_get_contents(outputfile.c_str(), &contents, NULL, NULL);
    if (contents) {
      if (!g_utf8_validate(contents, -1, NULL)) {
        unix_cp(inputfile, outputfile);
        error = "UTF-8 validation failure";
        gw_warning(error);
      }
      g_free(contents);
      if (!error.empty())
        return error;
    }
  }

  // Decode the output file.
  script_decode_usfm_file(outputfile);

  // Handle OK.
  if (result == 0)
    return "";

  // Handle error.
  gchar *contents;
  g_file_get_contents(script_temporal_error_file().c_str(), &contents, NULL, NULL);
  if (contents) {
    error = contents;
    g_free(contents);
    gw_warning(error);
  }
  return error;
}

ustring script_get_path(const ustring & script, ScriptType * scripttype, bool binary)
// Gets the full path to an existing "script".
// If "binary" is true, then, if possible, it gives the path, not to the original
// script, but to the binary form of it. Right now this is used for TECkit support
// which has the mapping file, which is the script, and the compiled file, which
// is the binary.
// The "scripttype" is filled with the right type of script.
{
  // Try the various types of scripts.
  for (unsigned int i = 0; i < stEnd; i++) {
    ScriptType script_type = ScriptType(i);
    ustring path = gw_build_filename(directories_get_scripts(), script_prefix(script_type) + script + script_suffix(script_type, binary));
    if (g_file_test(path.c_str(), G_FILE_TEST_IS_REGULAR)) {
      if (scripttype)
        *scripttype = script_type;
      return path;
    }
  }
  // Failure.
  if (scripttype)
    *scripttype = stEnd;
  return "";
}

ustring script_get_path(const ustring & script, ScriptType scripttype, bool binary)
// Gets the full path to a new or existing "script" of "scripttype".
{
  return gw_build_filename(directories_get_scripts(), script_prefix(scripttype) + script + script_suffix(scripttype, binary));
}

ustring script_temporal_script_file()
{
  return gw_build_filename(directories_get_temp(), "script_filter_script");
}

ustring script_temporal_input_file()
{
  return gw_build_filename(directories_get_temp(), "script_filter_input");
}

ustring script_temporal_output_file()
{
  return gw_build_filename(directories_get_temp(), "script_filter_output");
}

ustring script_temporal_error_file()
{
  return gw_build_filename(directories_get_temp(), "script_filter_error");
}

ustring script_get_named_type(ScriptType scripttype)
{
  ustring name;
  switch (scripttype) {
  case stSed:
    name = "Sed rules";
    break;
  case stTECkit:
    name = "TECkit mapping";
    break;
  case stFree:
    name = "Free script";
    break;
  case stEnd:
    break;
  }
  return name;
}

bool script_sed_installed()
{
  return gw_find_program_in_path(script_sed_binary());
}

bool script_teckit_installed()
{
  return gw_find_program_in_path(script_teckit_compiler());
}

const gchar *script_sed_binary()
{
  return "sed";
}

const gchar *script_teckit_compiler()
{
  return "teckit_compile";
}

const gchar *script_teckit_txtconverter()
{
  return "txtconv";
}

const gchar *script_free_input_identifier()
{
  return "inputfile";
}

const gchar *script_free_output_identifier()
{
  return "outputfile";
}

void script_encode_usfm_file(const ustring & filename)
// Encodes a USFM file. The purpose is that the USFM marked are not changed by the script.
// The assumption is that numbers are not affected.
{
  // Read the file. Bail out if there's no text.
  ustring input;
  {
    gchar *contents;
    g_file_get_contents(filename.c_str(), &contents, NULL, NULL);
    if (!contents)
      return;
    input = contents;
    g_free(contents);
  }
  if (input.empty())
    return;

  // Go through the input, changing usfm codes to their numerical equivalent,
  // and copying data to the output.
  // E.g. "\id" would become "\_105_100".
  ustring output;
  bool within_usfm = false;
  for (unsigned int i = 0; i < input.length(); i++) {
    ustring character = input.substr(i, 1);
    if (within_usfm)
      if (character == " ")
        within_usfm = false;
    if (within_usfm) {
      gunichar unichar;
      gunichar *uc;
      uc = g_utf8_to_ucs4_fast(character.c_str(), -1, NULL);
      unichar = *uc;
      g_free(uc);
      character = "_" + convert_to_string(unichar);
    }
    output.append(character);
    if (character == "\\")
      within_usfm = true;
  }

  // Write the data back to the file.
  g_file_set_contents(filename.c_str(), output.c_str(), -1, NULL);
}

void script_decode_usfm_file(const ustring & filename)
// Decodes a USFM file, that means, puts the original USFM code back.
{
  // Read the file. Bail out if there's no text.
  ustring input;
  {
    gchar *contents;
    g_file_get_contents(filename.c_str(), &contents, NULL, NULL);
    if (!contents)
      return;
    input = contents;
    g_free(contents);
  }
  if (input.empty())
    return;

  // Go through the input, changing usfm codes to their numerical equivalent,
  // and copying data to the output.
  // E.g. "\_105_100" would become "\id".
  ustring output;
  while (!input.empty()) {
    ustring character = input.substr(0, 1);
    input.erase(0, 1);
    output.append(character);
    if (character == "\\") {
      size_t pos = input.find(" ");
      if (pos == string::npos)
        pos = input.length();
      ustring encoded_text = input.substr(0, pos);
      input.erase(0, pos);
      Parse parse(encoded_text, false, "_");
      for (unsigned int i = 0; i < parse.words.size(); i++) {
        gunichar unichar = convert_to_int(parse.words[i]);
        gchar buf[7];
        gint length = g_unichar_to_utf8(unichar, (gchar *) & buf);
        buf[length] = '\0';
        character = buf;
        output.append(character);
      }
    }
  }
  // Write the data back to the file.
  g_file_set_contents(filename.c_str(), output.c_str(), -1, NULL);
}
