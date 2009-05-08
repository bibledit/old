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

#include "git.h"
#include "gwrappers.h"
#include "directories.h"
#include <libxml/xmlwriter.h>
#include <libxml/xmlreader.h>
#include "ipc.h"
#include "utilities.h"
#include "projectutils.h"
#include "shell.h"
#include "progresswindow.h"
#include "books.h"
#include "tiny_utilities.h"
#include "settings.h"
#include "date_time_utils.h"
#include "unixwrappers.h"
#include "gtkwrappers.h"
#include "generalconfig.h"
#include "git-exec.h"

vector < GitTask > gittasks;

GitTask::GitTask(GitTaskType task_in, const ustring & project_in, unsigned int book_in, unsigned int chapter_in, unsigned int failures_in, const ustring & data_in)
{
  task = task_in;
  project = project_in;
  book = book_in;
  chapter = chapter_in;
  failures = failures_in;
  data = data_in;
}

GitChapterState::GitChapterState(const ustring & project, unsigned int book, unsigned int chapter)
{
  // Store variables.
  myproject = project;
  mybook = book;
  mychapter = chapter;
  // Read and store the current state of the file.
  ustring filename = project_data_filename_chapter(project, book, chapter, false);
  gchar *contents;
  g_file_get_contents(filename.c_str(), &contents, NULL, NULL);
  if (contents) {
    state = contents;
    g_free(contents);
  }
}

bool GitChapterState::changed()
{
  // Compare the current state of the file with the previous one,
  // and see whether it differs.
  ustring filename = project_data_filename_chapter(myproject, mybook, mychapter, false);
  gchar *contents;
  g_file_get_contents(filename.c_str(), &contents, NULL, NULL);
  bool filechanged = false;
  if (contents) {
    filechanged = state != contents;
    g_free(contents);
  }
  return filechanged;
}

ustring git_job_filename()
// Returns the filename of the xml files with the git jobs
{
  return gw_build_filename(directories_get_temp(), "gitjobs.xml");
}

void git_initialize_subsystem()
{
  // Load any tasks that were left undone at the previous shutdown.
  if (g_file_test(git_job_filename().c_str(), G_FILE_TEST_IS_REGULAR)) {

    // Load file in memory and create the input buffer.
    gchar *contents;
    g_file_get_contents(git_job_filename().c_str(), &contents, NULL, NULL);
    xmlParserInputBufferPtr inputbuffer;
    inputbuffer = xmlParserInputBufferCreateMem(contents, strlen(contents), XML_CHAR_ENCODING_NONE);

    // Create a reader.    
    xmlTextReaderPtr reader = xmlNewTextReader(inputbuffer, NULL);

    // Iterate through the nodes in streaming mode, loading tasks.
    if (reader) {
      int read;
      GitTask task(gttUpdateProject, "", 0, 0, 0, "");
      ustring text;
      while ((read = xmlTextReaderRead(reader) == 1)) {
        switch (xmlTextReaderNodeType(reader)) {
        case XML_READER_TYPE_ELEMENT:
          {
            break;
          }
        case XML_READER_TYPE_TEXT:
          {
            xmlChar *value = xmlTextReaderValue(reader);
            if (value) {
              text = (const char *)value;
              xmlFree(value);
            }
            break;
          }
        case XML_READER_TYPE_END_ELEMENT:
          {
            // Retrieve the name of the element and process it.
            xmlChar *element_name = xmlTextReaderName(reader);
            if (!xmlStrcmp(element_name, BAD_CAST "job")) {
              gittasks.push_back(task);
            } else if (!xmlStrcmp(element_name, BAD_CAST "task")) {
              task.task = (GitTaskType) convert_to_int(text);
            } else if (!xmlStrcmp(element_name, BAD_CAST "project")) {
              task.project = text;
            } else if (!xmlStrcmp(element_name, BAD_CAST "book")) {
              task.book = convert_to_int(text);
            } else if (!xmlStrcmp(element_name, BAD_CAST "chapter")) {
              task.chapter = convert_to_int(text);
            } else if (!xmlStrcmp(element_name, BAD_CAST "failures")) {
              task.failures = convert_to_int(text);
            } else if (!xmlStrcmp(element_name, BAD_CAST "data")) {
              task.data = text;
            }
            break;
          }
        }
      }
    }
    // Free memory.
    if (reader)
      xmlFreeTextReader(reader);
    if (inputbuffer)
      xmlFreeParserInputBuffer(inputbuffer);
    if (contents)
      free(contents);

    // Remove the file.
    unlink(git_job_filename().c_str());
  }

}

void git_finalize_subsystem()
{
  // Send the shutdown command to bibledit-git.
  vector < ustring > payload;
  payload.push_back(" ");
  extern InterprocessCommunication *ipc;
  ipc->send(ipcstBibleditGit, ipcctGitShutdown, payload);

  // If there are still any git jobs left, save them to file
  // for loading and execution during the next run.
  if (!gittasks.empty()) {

    // Create a new XML buffer, to which the document will be written
    xmlBufferPtr buffer = xmlBufferCreate();

    // Create a new XmlWriter for memory, with no compression.
    xmlTextWriterPtr writer = xmlNewTextWriterMemory(buffer, 0);

    // Create first bit of the document.
    xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);

    // Indent the output.
    xmlTextWriterSetIndent(writer, 1);

    // Write the main element.
    xmlTextWriterStartElement(writer, BAD_CAST "git-tasks");

    // Go through all available tasks and write them out.
    for (unsigned int i = 0; i < gittasks.size(); i++) {
      xmlTextWriterStartElement(writer, BAD_CAST "job");
      xmlTextWriterStartElement(writer, BAD_CAST "task");
      xmlTextWriterWriteFormatString(writer, "%d", gittasks[i].task);
      xmlTextWriterEndElement(writer);
      xmlTextWriterStartElement(writer, BAD_CAST "project");
      xmlTextWriterWriteFormatString(writer, "%s", gittasks[i].project.c_str());
      xmlTextWriterEndElement(writer);
      xmlTextWriterStartElement(writer, BAD_CAST "book");
      xmlTextWriterWriteFormatString(writer, "%d", gittasks[i].book);
      xmlTextWriterEndElement(writer);
      xmlTextWriterStartElement(writer, BAD_CAST "chapter");
      xmlTextWriterWriteFormatString(writer, "%d", gittasks[i].chapter);
      xmlTextWriterEndElement(writer);
      xmlTextWriterStartElement(writer, BAD_CAST "failures");
      xmlTextWriterWriteFormatString(writer, "%d", gittasks[i].failures);
      xmlTextWriterEndElement(writer);
      xmlTextWriterStartElement(writer, BAD_CAST "data");
      xmlTextWriterWriteFormatString(writer, "%s", gittasks[i].data.c_str());
      xmlTextWriterEndElement(writer);
      xmlTextWriterEndElement(writer);
    }

    // We could close any open elements by hand, but closing the document does it all.
    xmlTextWriterEndDocument(writer);

    // Flush the writer, else nothing gets transferred to the buffer.
    xmlTextWriterFlush(writer);

    // Write the lines to disk.
    ParseLine parseline((const char *)buffer->content);
    write_lines(git_job_filename(), parseline.lines);

    // Free memory.
    if (writer)
      xmlFreeTextWriter(writer);
    if (buffer)
      xmlBufferFree(buffer);
  }
}

void git_initial_check_all(bool gui)
// Does initial checks on the local git repositories.
{
  extern Settings *settings;

  // Determine whether to run the health programs on the git repository.
  int githealth = settings->genconfig.git_health_get();
  int currentday = date_time_julian_day_get_current();
  bool run_githealth = false;
  if (ABS(currentday - githealth) >= 30) {
    run_githealth = true;
    settings->genconfig.git_health_set(currentday);
  }
  // Do all editable projects.
  vector < ustring > projects = projects_get_all();
  for (unsigned int i = 0; i < projects.size(); i++) {
    ProjectConfiguration *projectconfig = settings->projectconfig(projects[i]);
    if (projectconfig->editable_get()) {
      git_initial_check_project(projects[i], run_githealth);
    }
  }
}

void git_initial_check_project(const ustring & project, bool health)
// Does initial checks on the git repository.
// Upgrades if needed.
{
  // Schedule project initialization through git.
  git_schedule(gttInitializeProject, project, health, 0, "");
}

void git_task_human_readable(unsigned int task, const ustring & project, unsigned int book, unsigned int chapter, unsigned int fail, gchar * &human_readable_task, ustring & human_readable_description, ustring & human_readable_status)
{
  // Describe the task.
  switch ((GitTaskType) task) {
  case gttInitializeProject:
    {
      human_readable_task = (gchar *) "Initialize project";
      break;
    }
  case gttCommitProject:
    {
      human_readable_task = (gchar *) "Commit project";
      break;
    }
  case gttStoreChapter:
    {
      human_readable_task = (gchar *) "Store chapter";
      break;
    }
  case gttUpdateProject:
    {
      human_readable_task = (gchar *) "Update project";
      break;
    }
  }
  // Give description of task.
  switch ((GitTaskType) task) {
  case gttStoreChapter:
    {
      human_readable_description = books_id_to_english(book) + " " + convert_to_string(chapter);
      break;
    }
  case gttUpdateProject:
  case gttInitializeProject:
  case gttCommitProject:
    {
      break;
    }
  }
  // Describe status.  
  if (fail == 0) {
    human_readable_status = "Pending";
  } else {
    human_readable_status = "Failed " + convert_to_string(fail) + " times, will retry";
  }
}

void git_schedule(GitTaskType task, const ustring & project, unsigned int book, unsigned int chapter, const ustring & data)
// This schedules a git task.
{
  GitTask gittask(task, project, book, chapter, 0, data);
  gittasks.push_back(gittask);
}

unsigned int git_tasks_count()
{
  return gittasks.size();
}

void git_get_tasks(vector < unsigned int >&tasks, vector < ustring > &projects, vector < unsigned int >&books, vector < unsigned int >&chapters, vector < unsigned int >&fails)
{
  for (unsigned int i = 0; i < gittasks.size(); i++) {
    tasks.push_back(gittasks[i].task);
    projects.push_back(gittasks[i].project);
    books.push_back(gittasks[i].book);
    chapters.push_back(gittasks[i].chapter);
    fails.push_back(gittasks[i].failures);
  }
}

vector < ustring > git_get_next_task()
{
  vector < ustring > task;
  if (!gittasks.empty()) {
    task.push_back(convert_to_string(gittasks[0].task));
    task.push_back(gittasks[0].project);
    task.push_back(convert_to_string(gittasks[0].book));
    task.push_back(convert_to_string(gittasks[0].chapter));
    task.push_back(gittasks[0].data);
  }
  return task;
}

void git_erase_task_done()
{
  // Erase the task on top of the stack, but also all other equal tasks.
  if (!gittasks.empty()) {
    GitTask task = gittasks[0];
    vector < GitTask > newtasks;
    for (unsigned int i = 0; i < gittasks.size(); i++) {
      if (task.task != gittasks[i].task || task.project != gittasks[i].project || task.book != gittasks[i].book || task.chapter != gittasks[i].chapter)
        newtasks.push_back(gittasks[i]);
    }
    gittasks = newtasks;
  }
}

void git_fail_task_done()
{
  if (!gittasks.empty()) {
    GitTask task = gittasks[0];
    git_erase_task_done();
    task.failures++;
    gittasks.push_back(task);

  }
}

void git_erase_task(GitTaskType task, const ustring & project, unsigned int book, unsigned int chapter)
{
  vector < GitTask > newtasks;
  for (unsigned int i = 0; i < gittasks.size(); i++) {
    if (task != gittasks[i].task || project != gittasks[i].project || book != gittasks[i].book || chapter != gittasks[i].chapter)
      newtasks.push_back(gittasks[i]);
  }
  gittasks = newtasks;
}

void git_store_chapter(const ustring & project, unsigned int book, unsigned int chapter)
{
  git_schedule(gttStoreChapter, project, book, chapter, "");
}

void git_move_project(const ustring & project, const ustring & newproject)
{
  // Update all pending operations for this project.
  for (unsigned int i = 0; i < gittasks.size(); i++) {
    if (gittasks[i].project == project)
      gittasks[i].project = newproject;
  }
}

void git_remove_book(const ustring & project, unsigned int book)
{
  // Remove all pending operations for this project/book.
  vector < GitTask > newtasks;
  for (unsigned int i = 0; i < gittasks.size(); i++) {
    if ((gittasks[i].project != project) || (gittasks[i].book != book))
      newtasks.push_back(gittasks[i]);
  }
  gittasks = newtasks;
  // Schedule project commit.
  // The way we commit data, git will remove the book on its own.
  git_schedule(gttCommitProject, project, 0, 0, "");
}

void git_remove_chapter(const ustring & project, unsigned int book, unsigned int chapter)
{
  // Remove all pending operations for this project/book/chapter.
  vector < GitTask > newtasks;
  for (unsigned int i = 0; i < gittasks.size(); i++) {
    if ((gittasks[i].project != project) || (gittasks[i].book != book) || (gittasks[i].chapter != chapter))
      newtasks.push_back(gittasks[i]);
  }
  gittasks = newtasks;
  // Schedule a project commit.
  // The way we commit data, git will remove the chapter on its own.
  git_schedule(gttCommitProject, project, 0, 0, "");
}

void git_remove_project(const ustring & project)
{
  // Remove all pending operations for this project.
  vector < GitTask > newtasks;
  for (unsigned int i = 0; i < gittasks.size(); i++) {
    if (gittasks[i].project != project)
      newtasks.push_back(gittasks[i]);
  }
  gittasks = newtasks;
}

int git_count_tasks_project(const ustring & project)
{
  int count = 0;
  for (unsigned int i = 0; i < gittasks.size(); i++) {
    if (gittasks[i].project == project)
      count++;
  }
  return count;
}

void git_revert_to_internal_repository(const ustring & project)
// This reverts the repository to the internal one, if that is not yet the case.
{
  // Set in the configuration that we're using a local repository only.
  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(project);
  projectconfig->git_use_remote_repository_set(false);
}

void git_resolve_conflict_chapter(const ustring & project, unsigned int book, unsigned int chapter)
// This solves a conflicting chapter.
{
  // Log message
  gw_message("Resolving conflict for project " + project + " book " + books_id_to_english(book) + " " + convert_to_string(chapter));

  // Directory of the chapter in the data.
  ustring directory = project_data_directory_chapter(project, book, chapter);

  extern Settings *settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(project);
  GitConflictHandlingType conflicthandling = (GitConflictHandlingType) projectconfig->git_remote_repository_conflict_handling_get();

  // Data filename.
  ustring datafile = project_data_filename_chapter(project, book, chapter, false);

  /* Read the datafile. If there is a conflict it will look like the example below:

     \c 1
     <<<<<<< HEAD:3 John/1/data
     \v 1 my text.
     =======
     \v 1 server's text.
     >>>>>>> a62f843ce41ed2d0325c8a2767993df6acdbc933:3 John/1/data
     \v 2

   */
  ReadText rt(datafile, true);

  // Set about to resolve the conflict.
  vector < ustring > newdata;
  bool withinmine = false;
  bool withinserver = false;
  for (unsigned int i = 0; i < rt.lines.size(); i++) {
    // Find out when we've a marker, no actual data.    
    bool minemarker = rt.lines[i].find(git_mine_conflict_marker()) != string::npos;
    bool separatormarker = rt.lines[i] == "=======";
    bool servermarker = rt.lines[i].find(">>>>>>> ") == 0;
    // Do conflict management only if we've actual data.
    bool takeit = false;
    if (!minemarker && !separatormarker && !servermarker) {
      if (withinmine) {
        if (conflicthandling == gchtTakeMe)
          takeit = true;
      } else if (withinserver) {
        if (conflicthandling == gchtTakeServer)
          takeit = true;
      } else {
        takeit = true;
      }
    }
    if (takeit) {
      newdata.push_back(rt.lines[i]);
    } else {
    }
    // Set whether we're within a conflict, my data, or the server's data.
    // This setting applies to the next line of data.
    if (minemarker) {
      withinmine = true;
      withinserver = false;
    }
    if (separatormarker) {
      withinmine = false;
      withinserver = true;
    }
    if (servermarker) {
      withinmine = false;
      withinserver = false;
    }
  }
  write_lines(datafile, newdata);

  // To inform git that the conflict has been resolved.
  git_exec_store_chapter(project, book, chapter);
}

ustring git_mine_conflict_marker()
/*
 If there is a conflicting merge, we may have data like this:

 <<<<<<< HEAD:3 John/1/data
 \v 1 xFrom the church leader.
 =======
 \v 1 xxFrom the church leader.
 >>>>>>> a62f843ce41ed2d0325c8a2767993df6acdbc933:3 John/1/data

 */
{
  return "<<<<<<< HEAD";
}

void git_log_read(const ustring & directory, vector <ustring>& commits, vector <unsigned int>& seconds, const ustring& path)
/*
 Reads git's log, and retrieves the commits and the date/time from it.
 Date/time expressed in seconds.
 If a path is given, it only reads the commits to this path.
 git-log gives the most recent commit first and the older ones following.

 Example:

 commit a3e9fe9b43521007c81a2145b5e8bc5a37fdd7fd
 tree 0eb4be44c2d42cd219c3a1d14df385392d126f29
 parent 55aea5bdc961dfc043001546fa9e95ca16fd24b2
 author joe <joe@nw9440.(none)> 1193934787 +0200
 committer joe <joe@nw9440.(none)> 1193934787 +0200

 */
{
  // Read the log, using the raw format so as to easily extract the seconds since Epoch.
  // At first we read the log through GwSpawn, but as the logs became larger,
  // GwSpawn would choke on the large log file. We now read through the shell.
  // This is lightning fast as compared to GwSpawn.
  ustring command("cd" + shell_quote_space(directory) + " && ");
  command.append("git log --pretty=raw");
  if (!path.empty())
    command.append(shell_quote_space(path));
  ustring logfile = gw_build_filename(directories_get_temp(), ".git_log_read");
  command.append(" > " + logfile);
  if (system(command.c_str())) ;

  // Process the log.
  ustring commit;
  ustring second;
  unsigned int offset = 0;
  ReadText rt(logfile, true, false);
  for (unsigned int i = 0; i < rt.lines.size(); i++) {
    if (rt.lines[i].find("commit ") == 0) {
      commit = rt.lines[i];
      commit.erase(0, 6);
      commit = trim(commit);
    }
    if (rt.lines[i].find("committer") == 0) {
      Parse parse(rt.lines[i]);
      if (parse.words.size() >= 5) {
        second = parse.words[parse.words.size() - 2];
        ustring timezone = parse.words[parse.words.size() - 1];
        bool add = timezone.find("+") == 0;
        timezone = number_in_string(timezone);
        if (timezone.length() == 4) {
          unsigned int hours = convert_to_int(timezone.substr(0, 2));
          unsigned int minutes = convert_to_int(timezone.substr(2, 2));
          offset = 60 * minutes + 3600 * hours;
          if (!add)
            offset = 0 - offset;
        }
      } else
        commit.clear();
    }
    if (!commit.empty() && !second.empty()) {
      commits.push_back(commit);
      commit.clear();
      seconds.push_back(convert_to_int(second) + offset);
      second.clear();
    }
  }
}

ustring git_log_pick_commit_at_date_time(const vector < ustring > &commits, vector < unsigned int >&seconds, unsigned int second)
/*
 Picks the commit that was made at or before a certain date and time.

 Input:
 commits: The list of commits.
 seconds: The list of seconds. 
 It is assumed that the most recent time is a the top of the list.
 The above two are lists of the same size.
 second: The date/time at which to pick the commit.
 It can happen that under the constraints passed no commit is found.

 It is assumed that the list of commits is sorted on time, that the most recent
 commit is given first, and the older ones following, for example:
 Tuesday, 22 April 2008, 17:30:22 dbb65805e613a5ba7034f2ad08d26a8439f54f4a
 Tuesday, 22 April 2008, 17:26:22 21ffbc7a17aa3bd193f35c70ce772ea9fce47c7a
 Tuesday, 22 April 2008, 17:25:28 f4a3893c5184566c7a8dd3546cef4b6f1455e570
 Tuesday, 22 April 2008, 17:23:57 f203cf815885c748a53461d4fd6446a9263c81cc
 */
{
  ustring commit;
  for (int i = commits.size() - 1; i >= 0; i--) {
    if (second >= seconds[i]) {
      commit = commits[i];
    }
  }
  return commit;
}

bool git_log_extract_date_time(const ustring & line, int &year, int &month, int &day, int &hour, int &minute, int &second)
/* 
 Reads a line of "svn log" and extract the date and time from it.
 Typical lines:

 ------------------------------------------------------------------------
 r1 | joe | 2006-12-25 07:47:15 +0200 (Mon, 25 Dec 2006) | 1 line

 store book
 ------------------------------------------------------------------------

 */
{
  bool success = false;
  if (line.length() > 20) {
    if (line.substr(0, 1) == "r") {
      if (line.find("|") != string::npos) {
        Parse parse(line, false, "|");
        if (parse.words.size() == 4) {
          ustring datetime = parse.words[2];
          if (datetime.length() > 20) {
            replace_text(datetime, "-", " ");
            replace_text(datetime, ":", " ");
            Parse parse2(datetime);
            if (parse2.words.size() > 6) {
              year = convert_to_int(parse2.words[0]);
              month = convert_to_int(parse2.words[1]);
              day = convert_to_int(parse2.words[2]);
              hour = convert_to_int(parse2.words[3]);
              minute = convert_to_int(parse2.words[4]);
              second = convert_to_int(parse2.words[5]);
              success = true;
            }
          }
        }
      }
    }
  }
  return success;
}

Reference git_execute_retrieve_reference()
// Retrieves the editor's reference from the database.
{
  Reference reference(0);
  extern InterprocessCommunication *ipc;
  vector < ustring > payload = ipc->get_payload(ipcctGitJobDescription);
  if (payload.size() == 2) {
    reference.book = convert_to_int(payload[0]);
    reference.chapter = convert_to_int(payload[1]);
  }
  return reference;
}

void git_command_pause(bool pause)
// Sets a flag whether the git subsystem has to pause or resume.
{
  extern Settings *settings;
  settings->session.git_pause = pause;
}

void git_resolve_conflicts(const ustring & project, const vector < ustring > &errors)
/*
 This function reads through the "errors" to see if any conflicting merge occurred.
 If that happened, it resolves the conflicts.
 */
{
  /*
     See if the errors indicate a conflict. If not, bail out.

     The first time that a "git pull" is done, and there is a conflict, it says this:

     Auto-merged 3 John/1/data
     CONFLICT (content): Merge conflict in 3 John/1/data
     Automatic merge failed; fix conflicts and then commit the result.  

     The second time that a "git pull" would be done, it would say this:

     You are in the middle of a conflicted merge.

   */
  bool conflict = false;
  for (unsigned int i = 0; i < errors.size(); i++) {
    if (errors[i].find("conflict") != string::npos) {
      conflict = true;
    }
  }
  if (!conflict)
    return;

  // Get the working directory.
  ustring directory = project_data_directory_project(project);

  // Run a "git status" to find the books and chapters that have a merge conflict.
  vector < Reference > conflicted_chapters;
  {
    GwSpawn spawn("git status");
    spawn.workingdirectory(directory);
    spawn.read();
    spawn.run();
    for (unsigned int i = 0; i < spawn.standardout.size(); i++) {
      ustring line = spawn.standardout[i];
      size_t pos = line.find("unmerged:");
      if (pos != string::npos) {
        line.erase(0, pos + 10);
        line = trim(line);
        Parse parse(line, false, G_DIR_SEPARATOR_S);
        if (parse.words.size() >= 2) {
          unsigned int book = books_english_to_id(parse.words[0]);
          if (book) {
            unsigned int chapter = convert_to_int(parse.words[1]);
            Reference reference(book, chapter, "");
            conflicted_chapters.push_back(reference);
          }
        }
      }
    }
  }

  // Resolve all conflicts according to the settings of the user.
  for (unsigned int i = 0; i < conflicted_chapters.size(); i++) {
    git_resolve_conflict_chapter(project, conflicted_chapters[i].book, conflicted_chapters[i].chapter);
  }

  // Commit the changes.
  {
    GwSpawn spawn("git commit");
    spawn.workingdirectory(directory);
    spawn.arg("-m");
    spawn.arg("Resolved conflict");
    spawn.arg("-a");
    spawn.run();
  }
}

vector < ustring > git_retrieve_chapter_commit(const ustring & project, unsigned int book, unsigned int chapter, const ustring & commit)
// Retrieves chapter text from history.
// project: project to retrieve from.
// book: book to retrieve from.
// chapter: chapter text to retrieve.
// commit: named commit to retrieve text from.
{
  // Get the directory of the chapter and the project.
  ustring chapterdirectory = project_data_directory_chapter(project, book, chapter);
  ustring projectdirectory = project_data_directory_project(project);

  // Just to be sure, commit anything that might be outstanding.
  {
    GwSpawn spawn("git commit");
    spawn.workingdirectory(chapterdirectory);
    spawn.arg("-a");
    spawn.arg("-m");
    spawn.arg("Commit");
    spawn.run();
  }

  // Check the desired commit out in a new branch.
  {
    GwSpawn spawn("git checkout");
    spawn.workingdirectory(chapterdirectory);
    spawn.arg("-b");
    spawn.arg("git_retrieve_chapter_commit");
    spawn.arg(commit);
    spawn.run();
  }

  // Load the chapter data.
  ustring chapterfilename = project_data_filename_chapter(project, book, chapter, false);
  ReadText rt(chapterfilename, true, false);

  // The branch just checked out can have removed data and directories.
  // So to be sure, check on that, and if needed work from another directory.
  ustring workingdirectory(chapterdirectory);
  if (!g_file_test(chapterdirectory.c_str(), G_FILE_TEST_IS_DIR)) {
    workingdirectory = projectdirectory;
  }
  // Switch back to the master branch.
  {
    GwSpawn spawn("git checkout");
    spawn.workingdirectory(workingdirectory);
    spawn.arg("master");
    spawn.run();
  }

  // Remove the temporal branch.  
  {
    GwSpawn spawn("git branch");
    spawn.workingdirectory(workingdirectory);
    spawn.arg("-d");
    spawn.arg("git_retrieve_chapter_commit");
    spawn.run();
  }

  // Give results.  
  return rt.lines;
}


/*

Todo git update

Send/receive scriptures. Works on git only. Normally only once in so many minutes, can be set. 
Default every hour or so. The git system is only used when remote git is used as well, apart from that it is not used. 
This prevents a lot of disk churning on startup since nothing is initialized if git is not used.

When a remote update is used for the first time in Bibledit, in a session, git is initialized first, once, and then to do the remote update. 
This way a lot of disk churning is avoided at startup.

When a remote update results in files locally changed, we need to make a snapshot of each of the changed chapters.
How to we know that local files were changed? We might need to roam through the snapshots and compare these with the actual 
state of the files so as to see where there was a change. This method might be more reliable than relying on git's output, we don't know yet.



*/


