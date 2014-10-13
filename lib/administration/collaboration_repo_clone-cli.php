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

$object = Filter_Cli::argument (@$argv, 1);
$directory = Filter_Cli::argument (@$argv, 2);
$escapedDir = escapeshellarg ($directory);

$database_config_bible = Database_Config_Bible::getInstance();
$url = $database_config_bible->getRemoteRepositoryUrl ($object);

$command = "cd $escapedDir; git clone $url .";
echo "$command\n";
passthru ($command, $exit_code);
if ($exit_code == 0) {
  echo Locale_Translate::_("Ok: The repository was cloned successfully.");
} else {
  echo Locale_Translate::_("Error: The repository could not be cloned.");
}
echo "\n";

// Switch rename detection off.
// This is necessary for the consultation notes, since git has been seen to "detect" spurious renames.
exec ("cd $escapedDir; git config diff.renamelimit 0");
exec ("cd $escapedDir; git config diff.renames false");

// Newer versions of git ask the user to set the default pushing method.
exec ("cd $escapedDir; git config push.default matching");

// On some machines the mail name and address are not set properly; therefore these are set here.
$database_config_general = Database_Config_General::getInstance();
$mail_name = $database_config_general->getSiteMailName();
if (!$mail_name) $mail_name = "Bibledit-Web";
exec ("cd $escapedDir; git config user.name \"$mail_name\"");
$mail_address = $database_config_general->getSiteMailAddress();
if (!$mail_address) $mail_address = "bibledit-web@bibledit.org";
exec ("cd $escapedDir; git config user.email \"$mail_address\"");

exec ("sync");

$database_shell = Database_Shell::getInstance ();
$database_shell->stopProcess ("collaboration_repo_clone", 0);

?>
