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


#include <tasks/run.h>
#include <tasks/logic.h>
#include <database/logs.h>
#include <filter/string.h>
#include <filter/url.h>
#include <email/receive.h>
#include <email/send.h>
#include <search/rebibles.h>
#include <search/renotes.h>
#include <styles/sheets.h>
#include <bible/import_task.h>
#include <compare/compare.h>
#include <database/maintenance.h>
#include <tmp/tmp.h>
#include <collaboration/link.h>
#include <sendreceive/sendreceive.h>
#include <sendreceive/settings.h>
#include <sendreceive/bibles.h>
#include <sendreceive/usfmresources.h>
#include <sendreceive/externalresources.h>
#include <sendreceive/notes.h>
#include <sendreceive/changes.h>
#include <demo/logic.h>
#include <config/logic.h>
#include <resource/convert2resource.h>
#include <resource/convert2bible.h>
#include <resource/print.h>
#include <resource/download.h>
#include <statistics/statistics.h>
#include <changes/modifications.h>
#include <sprint/burndown.h>
#include <checks/run.h>
#include <export/index.h>
#include <export/web.h>
#include <export/html.h>
#include <export/usfm.h>
#include <export/textusfm.h>
#include <export/odt.h>
#include <export/info.h>
#include <export/esword.h>
#include <export/onlinebible.h>
#include <export/bibledropbox.h>
#include <manage/hyphenate.h>
#include <paratext/logic.h>


mutex mutex_tasks; 
int running_tasks = 0;


void tasks_run_one (string filename)
{
  // Increase running tasks count.
  mutex_tasks.lock ();
  running_tasks++;
  mutex_tasks.unlock ();

  // Read the task from disk and erase the file.
  string path = filter_url_create_path (tasks_logic_folder (), filename);
  vector <string> lines = filter_string_explode (filter_url_file_get_contents (path), '\n');
  filter_url_unlink (path);
  
  // Interpret the task's command and its parameters, if any.
  string command = "";
  if (!lines.empty ()) {
    command = lines [0];
    lines.erase (lines.begin ());
  }
  string parameter1;
  if (!lines.empty ()) {
    parameter1 = lines [0];
    lines.erase (lines.begin ());
  }
  string parameter2;
  if (!lines.empty ()) {
    parameter2 = lines [0];
    lines.erase (lines.begin ());
  }
  string parameter3;
  if (!lines.empty ()) {
    parameter3 = lines [0];
    lines.erase (lines.begin ());
  }
  
  if (command == ROTATEJOURNAL) {
    Database_Logs database_logs = Database_Logs ();
    database_logs.checkup ();
    database_logs.rotate ();
  } else if (command == RECEIVEEMAIL) {
    email_receive ();
  } else if (command == SENDEMAIL) {
    email_send ();
  } else if (command == REINDEXBIBLES) {
    search_reindex_bibles ();
  } else if (command == REINDEXNOTES) {
    search_reindex_notes ();
  } else if (command == CREATECSS) {
    styles_sheets_create_all_run ();
  } else if (command == IMPORTUSFM) {
    bible_import_task (parameter1, parameter2);
  } else if (command == COMPAREUSFM) {
    compare_compare (parameter1, parameter2, convert_to_int (parameter3));
  } else if (command == MAINTAINDATABASE) {
    database_maintenance ();
  } else if (command == CLEANTMPFILES) {
    tmp_tmp ();
  } else if (command == LINKGITREPOSITORY) {
    collaboration_link (parameter1, convert_to_int (parameter2), parameter3);
  } else if (command == SENDRECEIVEBIBLES) {
    sendreceive_sendreceive (parameter1);
  } else if (command == SYNCNOTES) {
    sendreceive_notes ();
  } else if (command == SYNCBIBLES) {
    sendreceive_bibles ();
  } else if (command == SYNCSETTINGS) {
    sendreceive_settings ();
  } else if (command == SYNCEXTERNALRESOURCES) {
    sendreceive_externalresources ();
  } else if (command == SYNCUSFMRESOURCES) {
    sendreceive_usfmresources ();
  } else if (command == SYNCCHANGES) {
    sendreceive_changes ();
  } else if (command == CLEANDEMO) {
    demo_clean_data ();
  } else if (command == CONVERTBIBLE2RESOURCE) {
    convert_bible_to_resource (parameter1);
  } else if (command == CONVERTRESOURCE2BIBLE) {
    convert_resource_to_bible (parameter1);
  } else if (command == PRINTRESOURCES) {
    resource_print_job (parameter1, parameter2, parameter3);
  } else if (command == DOWNLOADRESOURCE) {
    resource_download_job (parameter1);
  } else if (command == NOTESSTATISTICS) {
    statistics_statistics ();
  } else if (command == GENERATECHANGES) {
    changes_modifications ();
  } else if (command == SPRINTBURNDOWN) {
    sprint_burndown ("", false);
  } else if (command == CHECKBIBLE) {
    checks_run (parameter1);
  } else if (command == EXPORTALL) {
    export_index ();
  } else if (command == EXPORTWEBMAIN) {
    export_web_book (parameter1, convert_to_int (parameter2), convert_to_bool (parameter3));
  } else if (command == EXPORTWEBINDEX) {
    export_web_index (parameter1, convert_to_bool (parameter2));
  } else if (command == EXPORTHTML) {
    export_html_book (parameter1, convert_to_int (parameter2), convert_to_bool (parameter3));
  } else if (command == EXPORTUSFM) {
    export_usfm (parameter1, convert_to_bool (parameter2));
  } else if (command == EXPORTTEXTUSFM) {
    export_text_usfm_book (parameter1, convert_to_int (parameter2), convert_to_bool (parameter3));
  } else if (command == EXPORTODT) {
    export_odt_book (parameter1, convert_to_int (parameter2), convert_to_bool (parameter3));
  } else if (command == EXPORTINFO) {
    export_info (parameter1, convert_to_bool (parameter2));
  } else if (command == EXPORTESWORD) {
    export_esword (parameter1, convert_to_bool (parameter2));
  } else if (command == EXPORTONLINEBIBLE) {
    export_onlinebible (parameter1, convert_to_bool (parameter2));
  } else if (command == HYPHENATE) {
    manage_hyphenate (parameter1, parameter2);
  } else if (command == SETUPPARATEXT) {
    Paratext_Logic::setup (parameter1, parameter2);
  } else if (command == SYNCPARATEXT) {
    Paratext_Logic::synchronize ();
  } else if (command == SUBMITBIBLEDROPBOX) {
    export_bibledropbox (parameter1, parameter2);
  } else {
    Database_Logs::log ("Unknown task: " + command);
  }

  // Decrease running tasks count.
  mutex_tasks.lock ();
  running_tasks--;
  mutex_tasks.unlock ();
}


void tasks_run_check ()
{
  // Don't run more than so many tasks.
  if (tasks_run_active_count () >= config_logic_max_parallel_tasks ()) return;
  // Get and start first available task.
  vector <string> tasks = filter_url_scandir (tasks_logic_folder ());
  if (tasks.empty ()) return;
  thread task_thread = thread (tasks_run_one, tasks [0]);
  // Detach the thread to the thread continues to run independently,
  // when the thread object goes out of scope, and no memory is leaked this way.
  task_thread.detach ();
}


int tasks_run_active_count ()
{
  int taskscount = 0;
  mutex_tasks.lock ();
  taskscount = running_tasks;
  mutex_tasks.unlock ();
  return taskscount;
}

