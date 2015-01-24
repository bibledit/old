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


#include <collaboration/clone.h>
#include <assets/view.h>
#include <assets/page.h>
#include <assets/header.h>
#include <filter/roles.h>
#include <filter/string.h>
#include <filter/git.h>
#include <filter/url.h>
#include <locale/translate.h>
#include <access/bible.h>
#include <database/config/bible.h>
#include <database/jobs.h>
#include <git2.h>


typedef struct progress_data {
  int job_identifier;
  int seconds;
} progress_data;
// Todo need to have "percentage complete" field in the jobs database, so the page can display a progress bar.
// Todo need to have a "before", progress" and "after" field in the database,
// as well as a "complete" text (as it has now).
// Todo start it as a thread.


static int fetch_progress (const git_transfer_progress *stats, void *payload)
{
  progress_data *pd = (progress_data*) payload;
  int seconds = filter_string_date_seconds_since_epoch ();
  if (seconds != pd->seconds) {
    size_t received_kilo_bytes = stats->received_bytes / 1048;
    int percentage = round (100 * stats->received_objects / stats->total_objects);
    cout << percentage << "% " << received_kilo_bytes << "kb" << endl; // Todo
    pd->seconds = seconds;
  }
  return 0;
}


static void checkout_progress (const char *path, size_t cur, size_t tot, void *payload)
{
  progress_data *pd = (progress_data*) payload;
  int seconds = filter_string_date_seconds_since_epoch ();
  if (seconds != pd->seconds) {
    int percentage = round (100 * cur / tot);
    cout << percentage << "%" << " " << path << endl; // Todo
    pd->seconds = seconds;
  }
}


static int clone_cred_acquire_cb (git_cred **out, const char * url, const char * username_from_url, unsigned int allowed_types, void *payload)
{
  if (url) {};
  if (username_from_url) {};
  if (allowed_types) {};
  if (payload) {};
  
  char username[128] = {0};
  char password[128] = {0};
  
  return git_cred_userpass_plaintext_new (out, username, password);
}


void collaboration_clone (string object, int jobid) // Todo writing it.
{
  Database_Jobs database_jobs;

  Assets_View view = Assets_View ();

  view.set_variable ("object", object);
  
  string url = Database_Config_Bible::getRemoteRepositoryUrl (object);
  view.set_variable ("url", url);

  string path = filter_git_directory (object);

  if (!object.empty ()) {
    
    // Clear a possible existing git repository directory.
    filter_url_rmdir (path);

    git_threads_init ();

    progress_data pd = {0, 0};
    git_repository *cloned_repo = NULL;
    git_clone_options clone_opts;
    git_clone_init_options (&clone_opts, GIT_CLONE_OPTIONS_VERSION);
    git_checkout_options checkout_opts;
    git_checkout_init_options (&checkout_opts, GIT_CHECKOUT_OPTIONS_VERSION);
    
    // Set up options
    checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE_CREATE;
    checkout_opts.progress_cb = checkout_progress;
    checkout_opts.progress_payload = &pd;
    clone_opts.checkout_opts = checkout_opts;
    clone_opts.remote_callbacks.transfer_progress = &fetch_progress;
    clone_opts.remote_callbacks.credentials = clone_cred_acquire_cb;
    clone_opts.remote_callbacks.payload = &pd;
    
    // Do the clone
    int result = git_clone (&cloned_repo, url.c_str(), path.c_str(), &clone_opts);
    if (result != 0) {
      const git_error *err = giterr_last();
      cout << err->message << endl;
    }
    if (cloned_repo) git_repository_free (cloned_repo);
    git_threads_shutdown ();

/* Todo
      echo gettext("Ok: The repository was cloned successfully.");

 echo gettext("Error: The repository could not be cloned.");
 
 
    // Switch rename detection off.
    // This is necessary for the consultation notes, since git has been seen to "detect" spurious renames.
    exec ("cd $escapedDir; git config diff.renamelimit 0");
    exec ("cd $escapedDir; git config diff.renames false");
    
    // Newer versions of git ask the user to set the default pushing method.
    exec ("cd $escapedDir; git config push.default matching");
    
    // On some machines the mail name and address are not set properly; therefore these are set here.
    $database_config_general = Database_Config_General::getInstance();
    $mail_name = Database_Config_General::getSiteMailName();
    if (!$mail_name) $mail_name = "Bibledit";
    exec ("cd $escapedDir; git config user.name \"$mail_name\"");
    $mail_address = Database_Config_General::getSiteMailAddress();
    if (!$mail_address) $mail_address = "bibledit-web@bibledit.org";
    exec ("cd $escapedDir; git config user.email \"$mail_address\"");
*/
    
    
    
  }

  
  string page = collaboration_clone_header ();
  page += view.render ("collaboration", "clone");
  page += Assets_Page::footer ();

  database_jobs.setProgress (jobid, page);
}


string collaboration_clone_header ()
{
  return "<h1>" + gettext ("Clone repository") + "</h1>\n";
}
