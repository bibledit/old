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


#ifndef INCLUDED_FILTER_GIT_H
#define INCLUDED_FILTER_GIT_H


#include <config/libraries.h>


string filter_git_directory (string object);
bool filter_git_init (string directory, bool bare = false);
void filter_git_sync_bible_to_git (void * webserver_request, string bible, string repository);
void filter_git_sync_git_to_bible (void * webserver_request, string repository, string bible);
void filter_git_sync_git_chapter_to_bible (string repository, string bible, int book, int chapter);
bool filter_git_remote_read (string url, string & error);
bool filter_git_remote_clone (string url, string path, int jobid, string & error);
bool filter_git_add_remove_all (string repository, string & error); // Todo
bool filter_git_commit (string repository, string user, string email, string message, string & error); // Todo
void filter_git_config_set_bool (string repository, string name, bool value);
void filter_git_config_set_int (string repository, string name, int value);
void filter_git_config_set_string (string repository, string name, string value);


#endif
