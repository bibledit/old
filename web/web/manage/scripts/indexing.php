<?php
/*
Copyright (©) 2003-2014 Teus Benschop.

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
?>
<h1><?php echo Locale_Translate::_("Indexing") ?></h1>
<p>
  <?php echo Locale_Translate::_("Bibledit-Web uses indexes for fast lookup of data.") ?>
  <?php echo Locale_Translate::_("Normally the indexes are refreshed automatically.") ?>
  <?php echo Locale_Translate::_("When the Journal indicates unusual behaviour of an index, or errors, it needs to be recreated.") ?>
</p>
<p><a href="?refresh=bibles"><?php echo Locale_Translate::_("Recreate Bibles search index.") ?></a></p>
<p><a href="?refresh=notes"><?php echo Locale_Translate::_("Recreate Consultation Notes selection and search index.") ?></a></p>
