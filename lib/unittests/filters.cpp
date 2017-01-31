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


#include <unittests/unittest.h>
#include <unittests/utilities.h>
#include <config/libraries.h>
#include <library/bibledit.h>
#include <database/config/user.h>
#include <database/styles.h>
#include <database/books.h>
#include <database/config/bible.h>
#include <database/modifications.h>
#include <database/state.h>
#include <database/git.h>
#include <database/login.h>
#include <database/logs.h>
#include <config/globals.h>
#include <filter/url.h>
#include <filter/string.h>
#include <filter/roles.h>
#include <filter/usfm.h>
#include <filter/archive.h>
#include <filter/text.h>
#include <filter/css.h>
#include <filter/diff.h>
#include <filter/abbreviations.h>
#include <filter/git.h>
#include <filter/merge.h>
#include <filter/date.h>
#include <filter/shell.h>
#include <filter/memory.h>
#include <session/logic.h>
#include <text/text.h>
#include <esword/text.h>
#include <onlinebible/text.h>
#include <html/text.h>
#include <odf/text.h>
#include <styles/logic.h>
#include <styles/css.h>
#include <ipc/notes.h>
#include <client/logic.h>
#include <bible/logic.h>
#include <developer/logic.h>
#include <ldap/logic.h>


// Todo move into smaller units.


void test_filter_ldap ()
{
}


void test_filter_dev ()
{
  trace_unit_tests (__func__);
}
