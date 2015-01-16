<?php
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


require_once ("../bootstrap/bootstrap");


Filter_Cli::assert ();


$object = Filter_Cli::argument (@$argv, 1);


$database_config_bible = Database_Config_Bible::getInstance ();
$database_notes = Database_Notes::getInstance ();


$url = Database_Config_Bible::getRemoteRepositoryUrl ($object);
$directory = filter_git_git_directory ($object);
$consultationsfolder = $database_notes->mainFolder ();


// Our data goes into the local repository.
echo gettext("Step 1/2:") . " ";
echo gettext("Exporting the local Bible data to the local repository") . "\n";
Filter_Git::syncBible2Git ($object, $directory, true);


// Directory for use by the shell.
$shelldirectory = escapeshellarg ($directory);


// Add and commit the data.
$command = "cd $shelldirectory; git add --all .";
echo "$command\n";
exec ($command, $output, $exit_code);
echo "Exit code $exit_code\n";
if ($exit_code != 0) {
  for ($output as $line) {
    echo "$line\n";
  }
}
$command = "cd $shelldirectory; git commit -a -m admin-sync";
echo "$command\n";
exec ($command, $output, $exit_code);
echo "Exit code $exit_code\n";
if ($exit_code != 0) {
  for ($output as $line) {
    echo "$line\n";
  }
}


// Push data to remote repository.
echo gettext("Step 2/2: Pushing the data to the remote repository") . "\n";
$command = "cd $shelldirectory; git push 2>&1";
echo "$command\n";
exec ($command, $output, $exit_code);
if ($exit_code == 0) {
  echo gettext("Your data was pushed to the remote repository successfully.") . "\n";
} else {
  echo "Exit code $exit_code\n";
  for ($output as $line) {
    echo "$line\n";
  }
  echo gettext("Pushing your data to the remote repository failed.") . "\n";
}


// Sync the data to the USB flash drive to avoid data loss when the USB flash drive is unplugged before data was written to it.
exec ("sync");


$database_shell = Database_Shell::getInstance ();
$database_shell->stopProcess ("collaboration_take_yourself", 0);


?>
