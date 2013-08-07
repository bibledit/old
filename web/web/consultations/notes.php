<?php
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
*/


// Forward links in emails to the new page for displaying one note.
// It is only needed during the transition from bibledit-web version 1.1 to 1.2.
// After that, it can be removed.
if (isset ($_GET['consultationnote'])) {
  $id = $_GET['consultationnote'];
  header ("Location: ../notes/note.php?id=$id");
  die;
}


?>
