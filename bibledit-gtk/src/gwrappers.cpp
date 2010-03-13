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

#include "libraries.h"
#include "gwrappers.h"
#include <glib.h>
#include "utilities.h"
#include "progresswindow.h"
#include "unixwrappers.h"
#ifdef WIN32
#include <windows.h>
#else
#include <sys/wait.h>
#endif
#include "directories.h"
#include "shell.h"
#include "tiny_utilities.h"


ustring gw_build_filename(const ustring & part1, const ustring & part2)
// Wrapper for g_build_filename, to make programming easier.
{
  return tiny_gw_build_filename(part1, part2);
}

ustring gw_build_filename(const ustring & part1, const ustring & part2, const ustring & part3)
{
  return tiny_gw_build_filename(part1, part2, part3);
}

ustring gw_build_filename(const ustring & part1, const ustring & part2, const ustring & part3, const ustring & part4)
{
  ustring filename;
  gchar *name;
  name = g_build_filename(part1.c_str(), part2.c_str(), part3.c_str(), part4.c_str(), NULL);
  filename = name;
  g_free(name);
  return filename;
}

ustring gw_build_filename(const ustring & part1, const ustring & part2, const ustring & part3, const ustring & part4, const ustring & part5)
{
  ustring filename;
  gchar *name;
  name = g_build_filename(part1.c_str(), part2.c_str(), part3.c_str(), part4.c_str(), part5.c_str(), NULL);
  filename = name;
  g_free(name);
  return filename;
}

void gw_message(const ustring & message)
{
  if (write(1, message.c_str(), strlen(message.c_str()))) ;
  if (write(1, "\n", 1)) ;
}

void gw_warning(const ustring & warning)
{
  g_warning("%s", warning.c_str());
}

void gw_critical(const ustring & critical)
{
  g_critical("%s", critical.c_str());
}

void gw_error(const ustring & error)
{
  g_error("%s", error.c_str());
}

ustring gw_path_get_basename(const ustring & filename)
{
  ustring returnvalue;
  gchar *basename;
  basename = g_path_get_basename(filename.c_str());
  returnvalue = basename;
  g_free(basename);
  return returnvalue;
}

ustring gw_path_get_dirname(const ustring & filename)
{
  ustring returnvalue;
  gchar *dirname;
  dirname = g_path_get_dirname(filename.c_str());
  returnvalue = dirname;
  g_free(dirname);
  return returnvalue;
}


bool gw_find_program_in_path(const ustring & program)
/*
Before we used the exit code of the unix which command, but as this does 
not properly work on BSD systems, like Mac OS X, and the OLPC doesn't
include the which command, we make our own wrapper for glib's version.
It returns true if "program" is an executable program.
*/
{
  gchar *result;
  result = g_find_program_in_path(program.c_str());
  if (result) {
    g_free(result);
    return true;
  }
  return false;
}


void gw_destroy_source(guint & event_id)
// This is a combined wrapper for convenience.
{
  if (event_id) {
    GSource *source = g_main_context_find_source_by_id(NULL, event_id);
    if (source)
      g_source_destroy(source);
  }
  event_id = 0;
}


void gw_mkdir_with_parents(const ustring & directory)
// Creates directory, with the parents, if need be.
// Function mkdir could be used (see man 2 mkdir), but this does not allow for 
// the creation of the parent directories. The core utility mkdir provides
// this functionality, so is preferred, and used here.
// Later one g_mkdir_with_parents () was used, but this did not create 
// directories properly. Hence we are stuck with mkdir.
{
  ustring s;
  GwSpawn spawn("mkdir");
#ifndef WIN32
  spawn.arg("-p");
#endif
  spawn.arg(directory);
#ifdef WIN32
  spawn.devnull();
#endif
  spawn.run();
}

GwSpawn::GwSpawn(const char *program)
{
  myprogram = program;
  myasync = false;
  mydevnull = false;
  myread = false;
  myprogress = false;
  myallowcancel = false;
  myhide = false;
  cancelled = false;
  exitstatus = 0;
  pid = 0;
}

GwSpawn::~GwSpawn()
{
}

void GwSpawn::workingdirectory(ustring directory)
// The process' working directory.
{
  myworkingdirectory = directory;
}

void GwSpawn::arg(ustring value)
// Add one arguments to the arguments for running the program.
// This function can be repeated as many times as desired.
{
#ifdef WIN32
  // Quote the argument.
  value = shell_quote_space(value);
#else
  // Escape the '.
  replace_text(value, "'", "\\'");
#endif
  // Save argument.
  myarguments.push_back(value);
}

void GwSpawn::async()
// Run the program asynchronously, that is, don't wait for the program to exit.
{
  myasync = true;
}

void GwSpawn::devnull()
// Pipe cout and cerr to /dev/null.
{
  mydevnull = true;
}

void GwSpawn::write(ustring text)
// Write to cin of the program.
{
  mywrite = text;
  // Writing only works in async mode, so set that flag too.
  myasync = true;
}

void GwSpawn::read()
// Make cout and cerr of the program available for later reading.
{
  myread = true;
}

void GwSpawn::progress(ustring text, bool allow_cancel)
// Show progress of the program that runs. A pulsing bar remains visible as 
// long as the program runs. The user can cancel the program.
{
  myprogress = true;
  mytext = text;
  myallowcancel = allow_cancel;
  // Progress display requires async mode, so set that flag too.
  myasync = true;
}


void GwSpawn::describe ()
// Describes the command if it would run from a shell.
{
  ustring description;
  if (!myworkingdirectory.empty()) {
    description.append ("cd ");
    description.append (myworkingdirectory);
    description.append ("; ");
  }
  description.append (myprogram);
  for (unsigned int i = 0; i < myarguments.size(); i++) {
    description.append (" ");
    description.append (myarguments[i]);
  }
  gw_message (description);
}


#ifndef WIN32
void GwSpawn::run()
{
  // Working directory.
  const gchar *workingdirectory = NULL;
  if (!myworkingdirectory.empty())
    workingdirectory = myworkingdirectory.c_str();
  // Store arguments in argv.
  char *argv[myarguments.size() + 2];
  argv[0] = (char *)myprogram;
  for (unsigned int i = 0; i < myarguments.size(); i++) {
    argv[i + 1] = (char *)myarguments[i].c_str();
  }
  // Terminate argv.
  argv[myarguments.size() + 1] = NULL;
  // Spawn flags.
  int flags = G_SPAWN_SEARCH_PATH;
  if (mydevnull)
    flags |= (G_SPAWN_STDOUT_TO_DEV_NULL | G_SPAWN_STDERR_TO_DEV_NULL);
  // Possible pipes.
  gint standard_input_filedescriptor = 0;
  gint standard_output_filedescriptor;
  gint standard_error_filedescriptor;
  gint *standard_input_filedescriptor_pointer = NULL;
  gint *standard_output_filedescriptor_pointer = NULL;
  gint *standard_error_filedescriptor_pointer = NULL;
  gchar *standard_output = NULL;
  gchar *standard_error = NULL;
  gchar **standard_output_pointer = NULL;
  gchar **standard_error_pointer = NULL;
  if (myread) {
    standard_output_filedescriptor_pointer = &standard_output_filedescriptor;
    standard_error_filedescriptor_pointer = &standard_error_filedescriptor;
    standard_output_pointer = &standard_output;
    standard_error_pointer = &standard_error;
  }
  if (!mywrite.empty()) {
    standard_input_filedescriptor_pointer = &standard_input_filedescriptor;
  }
  // Spawn process.
  if (myasync) {
    result = g_spawn_async_with_pipes(workingdirectory, argv, NULL, (GSpawnFlags) flags, NULL, NULL, &pid, standard_input_filedescriptor_pointer, standard_output_filedescriptor_pointer, standard_error_filedescriptor_pointer, NULL);
    // Handle writing to stdin.
    if (standard_input_filedescriptor) {
      tiny_spawn_write(standard_input_filedescriptor, mywrite);
      close(standard_input_filedescriptor);
    }
  } else {
    result = g_spawn_sync(workingdirectory, argv, NULL, (GSpawnFlags) flags, NULL, NULL, standard_output_pointer, standard_error_pointer, &exitstatus, NULL);
  }
  // Handle case we didn't spawn the process.
  if (!result) {
    exitstatus = -1;
    ustring message = myprogram;
    message.append(" didn't spawn");
    gw_critical(message);
    return;
  }
  // Handle progress function.
  if (myprogress || standard_input_filedescriptor) {
    ProgressWindow *progresswindow = NULL;
    if (myprogress)
      progresswindow = new ProgressWindow(mytext, myallowcancel);
    ustring filename = gw_build_filename("/proc", convert_to_string(pid));
    while (g_file_test(filename.c_str(), G_FILE_TEST_EXISTS)) {
      if (progresswindow) {
        progresswindow->pulse();
        if (progresswindow->cancel) {
          unix_kill(pid);
          cancelled = true;
        }
      }
      g_usleep(500000);
    }
    // Close pid.
    g_spawn_close_pid(pid);
    if (progresswindow)
      delete progresswindow;
  }
  // Handle reading the output.
  if (myread) {
    // In async mode we've got file descriptors, and in sync mode we have 
    // gchar * output.
    // If async mode, read the output and close the descriptors.
    if (myasync) {
      GIOChannel *channel_out = g_io_channel_unix_new(standard_output_filedescriptor);
      g_io_channel_read_to_end(channel_out, &standard_output, NULL, NULL);
      g_io_channel_shutdown(channel_out, false, NULL);
      GIOChannel *channel_err = g_io_channel_unix_new(standard_error_filedescriptor);
      g_io_channel_read_to_end(channel_err, &standard_error, NULL, NULL);
      g_io_channel_shutdown(channel_err, false, NULL);
    }
    ParseLine parse_out(standard_output);
    standardout = parse_out.lines;
    ParseLine parse_err(standard_error);
    standarderr = parse_err.lines;
    // Free data.
    if (standard_output)
      g_free(standard_output);
    if (standard_error)
      g_free(standard_error);
  }
}
#endif

#ifdef WIN32
void GwSpawn::run()
/*
On Windows the normal routines of glib cannot be used well, because they show
a console window when running certain commands like mkdir, ping, etc.
Therefore this version of run() uses Windows specific system calls.
These calls allow one to hide the console window.
*/
{
  // Working directory.
  const gchar *workingdirectory = NULL;
  if (!myworkingdirectory.empty())
    workingdirectory = myworkingdirectory.c_str();
  /*
     The trick to running a console window silent is in the STARTUPINFO 
     structure that we pass into the CreateProcess function. 
     STARTUPINFO specifies the main window properties. 
     There are many items in the STARTUPINFO structure that we don't care about. 
     The ones that are of interest are:
     * DWORD cb
     * DWORD dwFlags
     * WORD wShowWindow
   */
  // The STARTUPINFO is instantiated.
  STARTUPINFO StartupInfo;
  PROCESS_INFORMATION ProcessInfo;
  // The memory is cleared for the length of the structure.
  memset(&StartupInfo, 0, sizeof(StartupInfo));
  // Fill the structure with the relevant code 
  // that will tell the console window to start up without showing itself.
  StartupInfo.cb = sizeof(STARTUPINFO);
  StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
  StartupInfo.wShowWindow = SW_HIDE;

  // Arguments to the program.
  char Args[4096];
  Args[0] = 0;

  // Environment.
  char *pEnvCMD = NULL;
  char *pDefaultCMD = "CMD.EXE";
  pEnvCMD = getenv("COMSPEC");

  if (pEnvCMD) {
    strcpy(Args, pEnvCMD);
  } else {
    strcpy(Args, pDefaultCMD);
  }

  // The "/c" option - Do the command then terminate the command window.
  strcat(Args, " /c ");
  // The application you would like to run from the command window.
  strcat(Args, myprogram);
  // The parameters passed to the application being run from the command window.
  // The arguments have been quoted and spaced already.
  for (unsigned int i = 0; i < myarguments.size(); i++) {
    strcat(Args, myarguments[i].c_str());
  }

  // Get the suffix for the files to be piped. It has the seconds and 
  // microseconds in them, to allow for parallel usage of these pipes.
  ustring pipe_suffix;
  if ((!mydevnull) || (!mywrite.empty())) {
    GTimeVal gtimeval;
    g_get_current_time(&gtimeval);
    pipe_suffix = convert_to_string((long unsigned int)gtimeval.tv_sec) + convert_to_string((long unsigned int)gtimeval.tv_usec);
  }
  // If there is standard input, create the file to be piped.
  // Write the text into that file. Add the file to the arguments.
  if (!mywrite.empty()) {
    ustring pipe_in = gw_build_filename(directories_get_temp(), "stdin" + pipe_suffix);
    WriteText wt(pipe_in);
    wt.text(mywrite);
    strcat(Args, " <");
    strcat(Args, shell_quote_space(pipe_in).c_str());
  }
  // If the output is not sent to "nul", then create piped files.
  ustring pipe_out;
  ustring pipe_err;
  if (!mydevnull) {
    pipe_out = gw_build_filename(directories_get_temp(), "stdout" + pipe_suffix);
    pipe_err = gw_build_filename(directories_get_temp(), "stderr" + pipe_suffix);
    ustring pout = shell_quote_space(pipe_out);
    ustring perr = shell_quote_space(pipe_err);
    strcat(Args, " >");
    strcat(Args, pout.c_str());
    strcat(Args, " 2>");
    strcat(Args, perr.c_str());
  }
  // Start the process.
  result = CreateProcess(NULL, Args, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, workingdirectory, &StartupInfo, &ProcessInfo);
  if (!result) {
    exitstatus = GetLastError();
    ustring message = myprogram;
    message.append(" didn't spawn");
    gw_critical(message);
    return;
  }
  // Handle progress function.
  if (myprogress) {
    ProgressWindow progresswindow(mytext, myallowcancel);
    // Time passed to WaitForSingleObject is in milliseconds.
    while ((WaitForSingleObject(ProcessInfo.hProcess, 500) >= 500)
           || (WaitForSingleObject(ProcessInfo.hThread, 500) >= 500)) {
      progresswindow.pulse();
      if (progresswindow.cancel) {
// todo        unix_kill (pid);
        cancelled = true;
      }
    }
  }
  // Handle sync mode.
  if (!myasync) {
    // Wait for it to finish.
    WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
    WaitForSingleObject(ProcessInfo.hThread, INFINITE);
  }
  // Get the exit code.
  ULONG rc;
  if (!GetExitCodeProcess(ProcessInfo.hProcess, &rc))
    rc = 0;
  exitstatus = rc;

  // Close handles.
  CloseHandle(ProcessInfo.hThread);
  CloseHandle(ProcessInfo.hProcess);

  // Read the pipe files if we don't sent the output to "nul".
  if (!mydevnull) {
    gchar *standard_output;
    g_file_get_contents(pipe_out.c_str(), &standard_output, NULL, NULL);
    gchar *standard_error;
    g_file_get_contents(pipe_err.c_str(), &standard_error, NULL, NULL);
    // Handle case we read the output. Else dump it to stdout/err.
    if (myread) {
      if (standard_output) {
        ParseLine parse_out(standard_output);
        standardout = parse_out.lines;
      }
      if (standard_error) {
        ParseLine parse_err(standard_error);
        standarderr = parse_err.lines;
      }
    } else {
      if (standard_output)
        tiny_spawn_write(1, standard_output);
      if (standard_error)
        tiny_spawn_write(2, standard_error);
    }
    // Free data.
    if (standard_output)
      g_free(standard_output);
    if (standard_error)
      g_free(standard_error);
  }
}
#endif

