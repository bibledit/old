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


require_once ("../bootstrap/bootstrap.php");


Filter_Cli::assert ();


error_reporting (E_ERROR);


$object = Filter_Cli::argument (@$argv, 1);


$database_config_bible = Database_Config_Bible::getInstance();
$database_notes = Database_Notes::getInstance ();


$url = $database_config_bible->getRemoteRepositoryUrl ($object);
$directory = Filter_Git::git_directory ($object);


// Copy the data from the local cloned repository, and store it in Bibledit-Web's Bible given in $object,
// overwriting the whole Bible that was there before.
echo Locale_Translate::_("Copying the data from the local cloned repository, and storing it in the Bible") . "\n";
Filter_Git::syncGit2Bible ($directory, $object);

exec ("sync");


$database_shell = Database_Shell::getInstance ();
$database_shell->stopProcess ("collaboration_take_repo", 0);


?>
