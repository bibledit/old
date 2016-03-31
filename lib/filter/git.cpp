/*
 Copyright (©) 2003-2016 Teus Benschop.
 
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


#include <filter/git.h>
#include <filter/url.h>
#include <filter/string.h>
#include <filter/shell.h>
#include <filter/merge.h>
#include <database/logs.h>
#include <database/books.h>
#include <database/jobs.h>
#include <database/config/general.h>
#ifdef HAVE_GIT
#include <git2.h>
#endif
#include <bible/logic.h>
#include <locale/translate.h>


// This function returns the directory of the git repository belonging to $object.
string filter_git_directory (string object)
{
  return filter_url_create_root_path ("git", object);
}


string filter_git_check_error (int result)
{
#ifdef HAVE_GIT
  string msg;
  if (result != 0) {
    const git_error * error = giterr_last ();
    msg = error->message;
    Database_Logs::log (error->message);
  }
  return msg;
#else
  if (result) {};
  return "";
#endif
}


void filter_git_check_error (string data)
{
  vector <string> lines = filter_string_explode (data, '\n');
  for (auto & line : lines) Database_Logs::log (line);
}


// Runs the equivalent of "git init".
bool filter_git_init (string directory, bool bare)
{
#ifdef HAVE_GIT
  git_threads_init ();
  git_repository *repo = NULL;
  int result = git_repository_init (&repo, directory.c_str(), bare);
  filter_git_check_error (result);
  git_repository_free (repo);
  git_threads_shutdown ();
  return (result == 0);
#else
  vector <string> parameters = {"init"};
  if (bare) parameters.push_back ("--bare");
  string output, error;
  int result = filter_shell_run (directory, "git", parameters, &output, &error);
  filter_git_check_error (output);
  filter_git_check_error (error);
  return (result == 0);
#endif
}


// This filter takes the Bible data as it is stored in Bibledit's database,
// and puts this information into the layout in books and chapters
// such as is used in Bibledit-Gtk into the $git folder.
// The $git is a git repository, and may contain other data as well.
// The filter focuses on reading the data in the git repository, and only writes to it if necessary,
// This speeds up the filter.
void filter_git_sync_bible_to_git (void * webserver_request, string bible, string repository)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  
  // First stage.
  // Read the chapters in the git repository,
  // and check if they occur in the database.
  // If a chapter is not in the database, remove it from the repository.
  vector <int> books = request->database_bibles()->getBooks (bible);
  vector <string> bookfiles = filter_url_scandir (repository);
  for (auto & bookname : bookfiles) {
    string path = filter_url_create_path (repository, bookname);
    if (filter_url_is_dir (path)) {
      int book = Database_Books::getIdFromEnglish (bookname);
      if (book) {
        if (in_array (book, books)) {
          // Book exists in the database: Check the chapters.
          vector <int> chapters = request->database_bibles()->getChapters (bible, book);
          vector <string> chapterfiles = filter_url_scandir (filter_url_create_path (repository, bookname));
          for (auto & chaptername : chapterfiles) {
            string path = filter_url_create_path (repository, bookname, chaptername);
            if (filter_url_is_dir (path)) {
              if (filter_string_is_numeric (chaptername)) {
                int chapter = convert_to_int (chaptername);
                string filename = filter_url_create_path (repository, bookname, chaptername, "data");
                if (file_exists (filename)) {
                  if (!in_array (chapter, chapters)) {
                    // Chapter does not exist in the database.
                    filter_url_rmdir (filter_url_create_path (repository, bookname, chaptername));
                  }
                }
              }
            }
          }
        } else {
          // Book does not exist in the database: Remove it from $git.
          filter_url_rmdir (filter_url_create_path (repository, bookname));
        }
      }
    }
  }
  
  // Second stage.
  // Read the books / chapters from the database,
  // and check if they occur in the repository, and the data matches.
  // If necessary, save the chapter to the repository.
  books = request->database_bibles()->getBooks (bible);
  for (auto & book : books) {
    string bookname = Database_Books::getEnglishFromId (book);
    string bookdir = filter_url_create_path (repository, bookname);
    if (!file_exists (bookdir)) filter_url_mkdir (bookdir);
    vector <int> chapters = request->database_bibles()->getChapters (bible, book);
    for (auto & chapter : chapters) {
      string chapterdir = filter_url_create_path (bookdir, convert_to_string (chapter));;
      if (!file_exists (chapterdir)) filter_url_mkdir (chapterdir);
      string datafile = filter_url_create_path (chapterdir, "data");
      string contents = filter_url_file_get_contents (datafile);
      string usfm = request->database_bibles()->getChapter (bible, book, chapter);
      if (contents != usfm) filter_url_file_put_contents (datafile, usfm);
    }
  }
}


// This filter takes the Bible data as it is stored in the git $repository folder,
// and puts this information into Bibledit's database.
// The $repository is a git repository, and may contain other data as well.
// The filter focuses on reading the data in the git repository and the database,
// and only writes to the database if necessary,
// This speeds up the filter.
void filter_git_sync_git_to_bible (void * webserver_request, string repository, string bible)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;

  // Stage one:
  // Read the chapters in the git repository,
  // and check that they occur in the database.
  // If any does not occur, add the chapter to the database.
  // This stage does not check the contents of the chapters.
  vector <string> bookfiles = filter_url_scandir (repository);
  for (auto & bookname : bookfiles) {
    string bookpath = filter_url_create_path (repository, bookname);
    if (filter_url_is_dir (bookpath)) {
      int book = Database_Books::getIdFromEnglish (bookname);
      if (book) {
        // Check the chapters.
        vector <int> chapters = request->database_bibles()->getChapters (bible, book);
        vector <string> chapterfiles = filter_url_scandir (bookpath);
        for (auto & chapterfile : chapterfiles) {
          string chapterpath = filter_url_create_path (bookpath, chapterfile);
          if (filter_url_is_dir (chapterpath)) {
            if (filter_string_is_numeric (chapterfile)) {
              int chapter = convert_to_int (chapterfile);
              string filename = filter_url_create_path (chapterpath, "data");
              if (file_exists (filename)) {
                if (!in_array (chapter, chapters)) {
                  // Chapter does not exist in the database: Add it.
                  string usfm = filter_url_file_get_contents (filename);
                  Bible_Logic::storeChapter (bible, book, chapter, usfm);
                  // Log it.
                  string message = translate("A translator added chapter") + " " + bible + " " + bookname + " " + chapterfile;
                  Database_Logs::log (message);
                }
              }
            }
          }
        }
      }
    }
  }
  
  
  // Stage two:
  // Read through the chapters in the database,
  // and check that they occur in the git folder.
  // If necessary, remove a chapter from the database.
  // If a chapter matches, check that the contents of the data in the git
  // folder and the contents in the database match.
  // If necessary, update the data in the database.
  vector <int> books = request->database_bibles()->getBooks (bible);
  for (auto & book : books) {
    string bookname = Database_Books::getEnglishFromId (book);
    string bookdir = filter_url_create_path (repository, bookname);
    if (file_exists (bookdir)) {
      vector <int> chapters = request->database_bibles()->getChapters (bible, book);
      for (auto & chapter : chapters) {
        string chapterdir = filter_url_create_path (bookdir, convert_to_string (chapter));
        if (file_exists (chapterdir)) {
          string datafile = filter_url_create_path (chapterdir, "data");
          string contents = filter_url_file_get_contents (datafile);
          string usfm = request->database_bibles()->getChapter (bible, book, chapter);
          if (contents != usfm) {
            Bible_Logic::storeChapter (bible, book, chapter, contents);
            Database_Logs::log (translate("A translator updated chapter") + " " + bible + " " + bookname + " " + convert_to_string (chapter));
          }
        } else {
          Bible_Logic::deleteChapter (bible, book, chapter);
          Database_Logs::log (translate("A translator deleted chapter") + " " + bible + " " + bookname + " " + convert_to_string (chapter));
        }
      }
    } else {
      Bible_Logic::deleteBook (bible, book);
      Database_Logs::log (translate("A translator deleted book") + " " + bible + " " + bookname);
    }
  }
}


string filter_git_disabled ()
{
  return "Git has been disabled on iOS and Android, and can be enabled on Linux, Windows and OS X";
}


// This filter takes one chapter of the Bible data as it is stored in the $git folder,
// and puts this information into Bibledit's database.
// The $git is a git repository, and may contain other data as well.
void filter_git_sync_git_chapter_to_bible (string repository, string bible, int book, int chapter)
{
  // Filename for the chapter.
  string bookname = Database_Books::getEnglishFromId (book);
  string filename = filter_url_create_path (repository, bookname, convert_to_string (chapter), "data");
  
  if (file_exists (filename)) {
    
    // Store chapter in database.
    string usfm = filter_url_file_get_contents (filename);
    Bible_Logic::storeChapter (bible, book, chapter, usfm);
    Database_Logs::log (translate("A collaborator updated") + " " + bible + " " + bookname + " " + convert_to_string (chapter));
    
  } else {
    
    // Delete chapter from database.
    Bible_Logic::deleteChapter (bible, book, chapter);
    Database_Logs::log (translate("A collaborator deleted chapter") + " " + bible + " " + bookname + " " + convert_to_string (chapter));
    
  }
}


#ifdef HAVE_GIT
static int cred_acquire_cb (git_cred **out, const char * url, const char * username_from_url, unsigned int allowed_types, void *payload)
{
  if (url) {};
  if (username_from_url) {};
  if (allowed_types) {};
  if (payload) {};
  
  char username[128] = {0};
  char password[128] = {0};
  
  return git_cred_userpass_plaintext_new (out, username, password);
}
#endif


// Returns true if the git repository at "url" is online.
bool filter_git_remote_read (string url, string & error)
{
  int result = 0;

#ifdef HAVE_GIT
  git_threads_init ();
  
  // Before running the actual test create an instance of a local repository.
  git_repository * repo = NULL;
  string repository_path = filter_url_tempfile ();
  filter_url_mkdir (repository_path);
  filter_git_init (repository_path);
  result = git_repository_open (&repo, repository_path.c_str());
  error = filter_git_check_error (result);
  
  // Create an instance of a remote from the URL.
  // The transport to use is detected from the URL
  git_remote * remote = NULL;
  if (result == 0) {
    result = git_remote_create (&remote, repo, "test", url.c_str ());
    error = filter_git_check_error (result);
  }

  // Callbacks.
  git_remote_callbacks callbacks;
  git_remote_init_callbacks (&callbacks, GIT_REMOTE_CALLBACKS_VERSION);
  if (result == 0) {
    callbacks.credentials = cred_acquire_cb;
    git_remote_set_callbacks (remote, &callbacks);
  }
  
  // When connecting, the underlying code needs to know wether to push or fetch.
  if (result == 0) {
    result = git_remote_connect (remote, GIT_DIRECTION_FETCH);
    error = filter_git_check_error (result);
  }

  // Read from the repository.
  const git_remote_head **refs;
  size_t refs_len;
  if (result == 0) {
    result = git_remote_ls (&refs, &refs_len, remote);
    error = filter_git_check_error (result);
  }
  
  // Put output into the journal.
  if (result == 0) {
    for (size_t i = 0; i < refs_len; i++) {
      char oid [GIT_OID_HEXSZ + 1] = {0};
      git_oid_fmt (oid, &refs[i]->oid);
      string msg = "Reading remote repository: ";
      msg.append (oid);
      msg.append (" ");
      msg.append (refs[i]->name);
      Database_Logs::log (msg);
    }
  }
  
  // Free resources.
  if (remote) git_remote_free (remote);
  if (repo) git_repository_free (repo);
  git_threads_shutdown ();

#else
  string output;
  result = filter_shell_run ("", "git", {"ls-remote", url}, &output, &error);
  filter_git_check_error (output);
  filter_git_check_error (error);
#endif

  return (result == 0);
}


typedef struct git_progress_data {
  int job_identifier;
  int seconds;
} git_progress_data;


#ifdef HAVE_GIT
static int fetch_progress (const git_transfer_progress *stats, void *payload)
{
  git_progress_data *pd = (git_progress_data*) payload;
  int seconds = filter_date_seconds_since_epoch ();
  if (seconds != pd->seconds) {
    size_t received_kilo_bytes = stats->received_bytes / 1048;
    int percentage = round (100 * stats->received_objects / stats->total_objects);
    string progress = convert_to_string (received_kilo_bytes) + " kb";
    if (pd->job_identifier) {
      Database_Jobs database_jobs;
      database_jobs.setPercentage (pd->job_identifier, percentage);
      database_jobs.setProgress (pd->job_identifier, progress);
    }
    pd->seconds = seconds;
  }
  return 0;
}
#endif


#ifdef HAVE_GIT
static void checkout_progress (const char *path, size_t cur, size_t tot, void *payload)
{
  git_progress_data *pd = (git_progress_data*) payload;
  int seconds = filter_date_seconds_since_epoch ();
  if (seconds != pd->seconds) {
    int percentage = round (100 * cur / tot);
    if (pd->job_identifier) {
      Database_Jobs database_jobs;
      database_jobs.setPercentage (pd->job_identifier, percentage);
      if (path) database_jobs.setProgress (pd->job_identifier, path);
    }
    pd->seconds = seconds;
  }
}
#endif


bool filter_git_remote_clone (string url, string path, int jobid, string & error)
{
  // Clear a possible existing git repository directory.
  filter_url_rmdir (path);

#ifdef HAVE_GIT
  
  git_progress_data pd = {0, 0};
  pd.job_identifier = jobid;
  
  git_threads_init ();
  
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
  clone_opts.remote_callbacks.credentials = cred_acquire_cb;
  clone_opts.remote_callbacks.payload = &pd;
  
  // Do the clone
  int result = git_clone (&cloned_repo, url.c_str(), path.c_str(), &clone_opts);
  if (result != 0) {
    const git_error *err = giterr_last();
    error = err->message;
  }
  if (cloned_repo) git_repository_free (cloned_repo);
  git_threads_shutdown ();
  
  return (result == 0);
#else
  if (jobid) {}
  string output;
  int result = filter_shell_run ("", "git", {"clone", url, path}, &output, &error);
  filter_git_check_error (output);
  filter_git_check_error (error);
  error.clear ();
  return (result == 0);
#endif
}


#ifdef HAVE_GIT
static int filter_git_index_remove_matched_path_cb (const char *path, const char *matched_pathspec, void *payload)
{
  if (matched_pathspec) {};
  if (path) {
    string * directory = (string *) payload;
    string filename = filter_url_create_path (* directory, path);
    if (file_exists (filename)) return 1;
    else return 0;
  }
  return 0;
}
#endif


bool filter_git_add_remove_all (string repository, string & error)
{
#ifdef HAVE_GIT
  git_repository * repo = NULL;
  git_index * index = NULL;

  git_threads_init();

  int result = git_repository_open (&repo, repository.c_str());
  if (result != 0) {
    error = filter_git_check_error (result);
  }

  if (result == 0) {
    result = git_repository_index (&index, repo);
    error = filter_git_check_error (result);
  }

  if (result == 0) {
    result = git_index_add_all (index, NULL, 0, NULL, NULL);
    error = filter_git_check_error (result);
  }
  
  if (result == 0) {
    result = git_index_remove_all (index, NULL, filter_git_index_remove_matched_path_cb, &repository);
    error = filter_git_check_error (result);
  }

  if (result == 0) {
    result = git_index_write (index);
    error = filter_git_check_error (result);
  }
  
  // Free resources.
  if (index) git_index_free (index);
  if (repo) git_repository_free (repo);
  git_threads_shutdown();

  return (result == 0);
#else
  string output;
  int result = filter_shell_run (repository, "git", {"add", "--all", "."}, &output, &error);
  filter_git_check_error (output);
  filter_git_check_error (error);
  return (result == 0);
#endif
}


bool filter_git_commit (string repository, string user, string email, string message, string & error)
{
  int result = 0;
  
#ifdef HAVE_GIT
  // Initialize the git system.
  git_threads_init();

  // Open the git repository.
  git_repository * repo = NULL;
  result = git_repository_open (&repo, repository.c_str());
  if (result != 0) {
    error = filter_git_check_error (result);
  }
  
  // Get the index to be committed to HEAD.
  git_index * index = NULL;
  if (result == 0) {
    result = git_repository_index (&index, repo);
    error = filter_git_check_error (result);
  }

  // Create a tree from the index.
  git_oid tree_oid;
  if (result == 0) {
    result = git_index_write_tree (&tree_oid, index);
    error = filter_git_check_error (result);
  }

  // Create a signature.
  git_signature * signature = NULL;
  time_t ctime = filter_date_seconds_since_epoch ();
  if (result == 0) {
    git_signature_new ((git_signature **)&signature, user.c_str(), email.c_str(), ctime, 0);
    error = filter_git_check_error (result);
  }

  // Get the tree.
  git_tree * tree = NULL;
  if (result == 0) {
    result = git_tree_lookup (&tree, repo, &tree_oid);
    error = filter_git_check_error (result);
  }

  // Format the commit message.
  git_buf buffer;
  memset (&buffer, 0, sizeof (git_buf));
  if (result == 0) {
    result = git_message_prettify (&buffer, message.c_str(), 0, '#');
    error = filter_git_check_error (result);
  }
  
  // Check whether the repository is empty, so HEAD is not yet committed.
  bool head_unborn = git_repository_head_unborn (repo) == 1;

  // Get HEAD as a commit object to use as the parent of the commit, if available.
  git_oid parent_id;
  git_commit *parent = NULL;
  if (!head_unborn) {
    if (result == 0) {
      result = git_reference_name_to_id (&parent_id, repo, "HEAD");
      error = filter_git_check_error (result);
    }
    if (result == 0) {
      result = git_commit_lookup (&parent, repo, &parent_id);
      error = filter_git_check_error (result);
    }
  }

  git_oid commit_oid;
  if (head_unborn) {
    if (result == 0) {
      result = git_commit_create_v (&commit_oid, repo, "HEAD", signature, signature, NULL, buffer.ptr, tree, 0);
      error = filter_git_check_error (result);
    }
  } else {
    if (result == 0) {
      result = git_commit_create_v (&commit_oid, repo, "HEAD", signature, signature, NULL, buffer.ptr, tree, 1, parent);
      error = filter_git_check_error (result);
    }
  }

  // Free resources.
  git_buf_free (&buffer);
  if (tree) git_tree_free (tree);
  if (signature) git_signature_free (signature);
  if (index) git_index_free (index);
  if (repo) git_repository_free (repo);
  git_threads_shutdown();

#else
  user.clear ();
  email.clear ();
  string output;
  result = filter_shell_run (repository, "git", {"commit", "-a", "-m", message}, &output, &error);
  filter_git_check_error (error);
#endif

  return (result == 0);
}


// This function runs "git commit" through the shell.
// It is needed because so far the libgit2 calls, in the parallel function, do not yet do the same as the shell call.
bool filter_git_commit (string repository, string message, vector <string> & messages)
{
  string out, err;
  int result = filter_shell_run (repository, "git", {"commit", "-a", "-m", message}, &out, &err);
  out = filter_string_trim (out);
  err = filter_string_trim (err);
  messages = filter_string_explode (out, '\n');
  vector <string> lines = filter_string_explode (err, '\n');
  messages.insert (messages.end(), lines.begin(), lines.end());
  return (result == 0);
}


void filter_git_config_set_bool (string repository, string name, bool value)
{
#ifdef HAVE_GIT
  git_repository * repo = NULL;
  int result = git_repository_open (&repo, repository.c_str());
  if (result != 0) {
    filter_git_check_error (result);
  }
  git_config * cfg = NULL;
  result = git_repository_config (&cfg, repo);
  if (result != 0) {
    filter_git_check_error (result);
  }
  result = git_config_set_bool (cfg, name.c_str(), value);
  if (cfg) git_config_free (cfg);
  if (repo) git_repository_free (repo);
#else
  string svalue = value ? "true" : "false";
  filter_git_config_set_string (repository, name, svalue);
#endif
}


void filter_git_config_set_int (string repository, string name, int value)
{
#ifdef HAVE_GIT
  git_repository * repo = NULL;
  int result = git_repository_open (&repo, repository.c_str());
  if (result != 0) {
    filter_git_check_error (result);
  }
  git_config * cfg = NULL;
  result = git_repository_config (&cfg, repo);
  if (result != 0) {
    filter_git_check_error (result);
  }
  result = git_config_set_int32 (cfg, name.c_str(), value);
  if (cfg) git_config_free (cfg);
  if (repo) git_repository_free (repo);
#else
  string svalue = convert_to_string (value);
  filter_git_config_set_string (repository, name, svalue);
#endif
}


void filter_git_config_set_string (string repository, string name, string value)
{
#ifdef HAVE_GIT
  git_repository * repo = NULL;
  int result = git_repository_open (&repo, repository.c_str());
  if (result != 0) {
    filter_git_check_error (result);
  }
  git_config * cfg = NULL;
  result = git_repository_config (&cfg, repo);
  if (result != 0) {
    filter_git_check_error (result);
  }
  result = git_config_set_string (cfg, name.c_str(), value.c_str());
  if (cfg) git_config_free (cfg);
  if (repo) git_repository_free (repo);
#else
  string output, error;
  filter_shell_run (repository, "git", {"config", name, value}, &output, &error);
#endif
}


// This filter takes a $line of the output of the git pull command.
// It tries to interpret it to find a passage that would have been updated.
// If a valid book and chapter are found, it returns them.
Passage filter_git_get_passage (string line)
{
  // Sample lines for git pull:

  // "From https://github.com/joe/test"
  // "   443579b..90dcb57  master     -> origin/master"
  // "Updating 443579b..90dcb57"
  // "Fast-forward"
  // " Genesis/1/data | 2 +-"
  // " 1 file changed, 1 insertion(+), 1 deletion(-)"
  // " delete mode 100644 Leviticus/1/data"
  // " create mode 100644 Leviticus/2/data"

  // Sample lines for git status:

  // On branch master
  // Your branch is up-to-date with 'origin/master'.
  
  // Changes not staged for commit:
  //   (use "git add <file>..." to update what will be committed)
  //   (use "git checkout -- <file>..." to discard changes in working directory)
  //      modified:   Genesis/1/data
  // no changes added to commit (use "git add" and/or "git commit -a")
  
  Passage passage;
  vector <string> bits = filter_string_explode (line, '/');
  if (bits.size () == 3) {
    size_t pos = bits [0].find (":");
    if (pos != string::npos) bits [0].erase (0, pos + 1);
    string bookname = filter_string_trim (bits [0]);
    int book = Database_Books::getIdFromEnglish (bookname);
    if (book) {
      if (filter_string_is_numeric (bits [1])) {
        int chapter = convert_to_int (bits [1]);
        string data = bits [2];
        if (data.find ("data") != string::npos) {
          passage.book = book;
          passage.chapter = chapter;
        }
      }
    }
  }
  return passage;
}


// Reports information comparable to "git status".
// Repository: "repository".
// All changed files will be returned.
vector <string> filter_git_status (string repository)
{
  vector <string> paths;

#ifdef HAVE_GIT
  
  int result = 0;
  
  git_repository * repo = NULL;
  result = git_repository_open (&repo, repository.c_str());
  if (result != 0) {
    filter_git_check_error (result);
  }

  git_status_list * status = NULL;
  if (result == 0) {
    result = git_status_list_new (&status, repo, NULL);
    if (result != 0) {
      filter_git_check_error (result);
    }
  }

  if (result == 0) {
    size_t maxi = git_status_list_entrycount (status);
    for (size_t i = 0; i < maxi; ++i) {
      const git_status_entry * entry = git_status_byindex (status, i);
      if (entry->status == GIT_STATUS_CURRENT) continue;
      if (entry->status & GIT_STATUS_IGNORED) continue;
      if (entry) {
        git_diff_delta * mut = NULL;
        if (entry->head_to_index) mut = entry->head_to_index;
        if (entry->index_to_workdir) mut = entry->index_to_workdir;
        if (mut) {
          string path;
          if (mut->old_file.path) path = mut->old_file.path;
          else if (mut->new_file.path) path = mut->new_file.path;
          if (!path.empty ()) paths.push_back (path);
        }
      }
      if (entry->status & GIT_STATUS_INDEX_NEW) {};
      if (entry->status & GIT_STATUS_INDEX_MODIFIED) {};
      if (entry->status & GIT_STATUS_INDEX_DELETED) {};
      if (entry->status & GIT_STATUS_INDEX_RENAMED) {};
      if (entry->status & GIT_STATUS_INDEX_TYPECHANGE) {};
      if (entry->status & GIT_STATUS_WT_NEW) {};
      if (entry->status & GIT_STATUS_WT_MODIFIED) {};
      if (entry->status & GIT_STATUS_WT_DELETED) {};
      if (entry->status & GIT_STATUS_WT_RENAMED) {};
      if (entry->status & GIT_STATUS_WT_TYPECHANGE) {};
    }
  }
  
  if (status) git_status_list_free (status);
  if (repo) git_repository_free (repo);
  
#else
  string output, error;
  filter_shell_run (repository, "git", {"status"}, &output, &error);
  filter_git_check_error (error);
  paths = filter_string_explode (output, '\n');
#endif

  return paths;
}


// Runs "git pull" and returns true if it ran fine.
// It puts the messages in container "messages".
bool filter_git_pull (string repository, vector <string> & messages)
{
  string out, err;
  int result = filter_shell_run (repository, "git", {"pull"}, &out, &err);
  out = filter_string_trim (out);
  err = filter_string_trim (err);
  messages = filter_string_explode (out, '\n');
  vector <string> lines = filter_string_explode (err, '\n');
  messages.insert (messages.end(), lines.begin(), lines.end());
  return (result == 0);
}


// Runs "git pull" and returns true if it ran fine.
// It puts the push messages in container "messages".
bool filter_git_push (string repository, vector <string> & messages, bool all)
{
  string out, err;
  vector <string> parameters = {"push"};
  if (all) parameters.push_back ("--all");
  int result = filter_shell_run (repository, "git", parameters, &out, &err);
  out = filter_string_trim (out);
  err = filter_string_trim (err);
  messages = filter_string_explode (out, '\n');
  vector <string> lines = filter_string_explode (err, '\n');
  messages.insert (messages.end(), lines.begin(), lines.end());
  return (result == 0);
}


// Resolves any conflicts in "repository".
// It fills "paths" with the paths to the files with the resolved merge conflicts.
// It fills "error" with any error that the library generates.
// It returns true on success, that is, no errors occurred.
bool filter_git_resolve_conflicts (string repository, vector <string> & paths, string & error)
{
  int result = 0;
  paths.clear();

#ifdef HAVE_GIT

  // Open the repository.
  git_repository * repo = NULL;
  result = git_repository_open (&repo, repository.c_str());
  if (result != 0) error = filter_git_check_error (result);
  
  // Get the index.
  git_index * index = NULL;
  if (result == 0) {
    result = git_repository_index (&index, repo);
    if (result != 0) error = filter_git_check_error (result);
  }

  // Get a handle on the object database.
  git_odb *odb = NULL;
  if (result == 0) {
    git_repository_odb (&odb, repo);
  }

  // Proceed if the index contains conflicts.
  if (result == 0) {
    if (git_index_has_conflicts (index)) {
      
      // Iterator for going through the conflicts.
      git_index_conflict_iterator * iter = NULL;
      result = git_index_conflict_iterator_new (&iter, index);
      if (result != 0) error = filter_git_check_error (result);
      if (result == 0) {

        // Iterate through the conflicts.
        const git_index_entry *ancestor = NULL;
        const git_index_entry *ours = NULL;
        const git_index_entry *theirs = NULL;
        while (git_index_conflict_next (&ancestor, &ours, &theirs, iter) != GIT_ITEROVER) {
          
          // Obtain string contents for ancestor, ours, and theirs.
          string ancestorcontents;
          if (ancestor) {
            git_odb_object *ancestor_file = NULL;
            git_odb_read (&ancestor_file, odb, &ancestor->id);
            const void *ancestor_data = git_odb_object_data (ancestor_file);
            size_t ancestor_size = git_odb_object_size (ancestor_file);
            const char * ancestor_chars = (const char *) ancestor_data;
            for (size_t i = 0; i < ancestor_size; i++) ancestorcontents += ancestor_chars [i];
            if (ancestor_file) git_odb_object_free (ancestor_file);
          }
          string ourcontents;
          if (ours) {
            git_odb_object *ours_file = NULL;
            git_odb_read (&ours_file, odb, &ours->id);
            const void *ours_data = git_odb_object_data (ours_file);
            size_t ours_size = git_odb_object_size (ours_file);
            const char * our_chars = (const char *) ours_data;
            for (size_t i = 0; i < ours_size; i++) ourcontents += our_chars [i];
            if (ours_file) git_odb_object_free (ours_file);
          }
          string theircontents;
          if (theirs) {
            git_odb_object *theirs_file = NULL;
            git_odb_read (&theirs_file, odb, &theirs->id);
            const void *theirs_data = git_odb_object_data (theirs_file);
            size_t theirs_size = git_odb_object_size (theirs_file);
            const char * their_chars = (const char *) theirs_data;
            for (size_t i = 0; i < theirs_size; i++) theircontents += their_chars [i];
            if (theirs_file) git_odb_object_free (theirs_file);
          }
          
          // Merge and store in the filesystem.
          string mergedcontents = filter_merge_run (ancestorcontents, ourcontents, theircontents);
          string file = filter_url_create_path (repository, ours->path);
          filter_url_file_put_contents (file, mergedcontents);
          
          // Mark the conflict as resolved.
          // git_index_conflict_remove (index, ours->path);
          // git_index_add_bypath (index, ours->path);
          
          // Record the path of the file with the resolved conflict.
          paths.push_back (ours->path);
        }
      }
  
      // Free resources.
      if (iter) git_index_conflict_iterator_free (iter);
      
      // Write the updated index to disk.
      if (result == 0) {
        //result = git_index_write (index);
        //error = filter_git_check_error (result);
      }

      // Clean up repository state.
      //git_repository_state_cleanup (repo);
    }
  }

  // Free resources.
  if (odb) git_odb_free (odb);
  if (index) git_index_free (index);
  if (repo) git_repository_free (repo);

#else

  // Get the unmerged paths.
  vector <string> unmerged_paths;
  vector <string> lines = filter_git_status (repository);
  for (auto & line : lines) {
    if (line.find ("both modified:") != string::npos) {
      line = filter_string_trim (line);
      line.erase (0, 15);
      line = filter_string_trim (line);
      unmerged_paths.push_back (line);
    }
  }
  
  // Deal with each unmerged path.
  for (auto & unmerged_path : unmerged_paths) {

    string common_ancestor;
    filter_shell_run (repository, "git", {"show", ":1:" + unmerged_path}, &common_ancestor, &error);

    string head_version;
    filter_shell_run (repository, "git", {"show", ":2:" + unmerged_path}, &head_version, &error);
    
    string merge_head_version;
    filter_shell_run (repository, "git", {"show", ":3:" + unmerged_path}, &merge_head_version, &error);
    
    string mergeBase (common_ancestor);
    string userData (head_version);
    string serverData (merge_head_version);
    
    string mergedData = filter_merge_run (mergeBase, userData, serverData);
    mergedData = filter_string_trim (mergedData);
    filter_url_file_put_contents (filter_url_create_path (repository, unmerged_path), mergedData);
    
    paths.push_back (unmerged_path);
  }

  if (!unmerged_paths.empty ()) {
    vector <string> messages;
    filter_git_commit (repository, "Bibledit fixed merge conflicts", messages);
  }
  
#endif

  // Done.
  return (result == 0);
}


// Configure the $repository: Make certain settings.
void filter_git_config (string repository)
{
  // At times there's a stale index.lock file that prevents any collaboration.
  // This is to be removed.
  string index_lock = filter_url_create_path (repository, ".git", "index.lock");
  if (file_exists (index_lock)) {
    Database_Logs::log ("Cleaning out index lock " + index_lock);
    filter_url_unlink (index_lock);
  }

  // On some machines the mail name and address are not set properly; therefore these are set here.
  string user = Database_Config_General::getSiteMailName ();
  if (user.empty ()) user = "Bibledit";
  filter_git_config_set_string (repository, "user.name", user);
  
  string mail = Database_Config_General::getSiteMailAddress ();
  if (mail.empty ()) mail = "bibledit@bibledit.org";
  filter_git_config_set_string (repository, "user.email", mail);

  // Switch rename detection off.
  // This is for the consultation notes, since git has been seen to falsely "detect" renames.
  filter_git_config_set_int (repository, "diff.renamelimit", 0);
  filter_git_config_set_bool (repository, "diff.renames", false);

  // Current versions of git ask the user to set the default push method.
  filter_git_config_set_string (repository, "push.default", "matching");
}
