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


#ifndef INCLUDED_GIT_H
#define INCLUDED_GIT_H


#include "libraries.h"
#include "types.h"
#include "reference.h"
#include "urltransport.h"


enum GitConflictHandlingType {gchtTakeMe, gchtTakeServer};


void git_upgrade ();
void git_revert_to_internal_repository (const ustring& project);
void git_resolve_conflict_chapter (const ustring& project, unsigned int book, unsigned int chapter);
ustring git_mine_conflict_marker ();
void git_resolve_conflicts (const ustring& project, const vector <ustring>& errors);
void git_process_feedback (const ustring& project, const vector <ustring>& feedback, unsigned int watched_book, unsigned int watched_chapter, bool& watch_updated);


#endif
