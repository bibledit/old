<?php
/*
Copyright (©) 2003-2013 Teus Benschop.

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
<h1><?php echo gettext ("Backup") ?></h1>
<p><?php echo gettext ("Every day at midnight, Bibledit-Web creates a backup of the database.") ?></p>
<p><?php echo gettext ("This backs up all relevant data.") ?></p>
<p><?php echo gettext ("The complete backup will be stored at the following location:") ?></p>
<p><?php echo $this->file ?></p>
<p><?php echo gettext ("In addition, there will be .sql files for each table in the following folder:") ?></p>
<p><?php echo $this->folder ?></p>
<p><?php echo gettext ("The Logbook shows the backup progress.") ?></p>
<p><a href="?create="><?php echo gettext ("Backup now.") ?></a></p>
<p class="error"><?php echo $this->error ?></p>
<p class="success"><?php echo $this->success ?></p>
<h2><?php echo gettext ("Restore") ?></h2>
<p><?php echo gettext ("Make sure you work with an existing an operational system. If this is not the case, then follow all installation procedures before proceeding to the next steps.") ?></p>
<br>
<p><?php echo gettext ("Login to the server or open a terminal.") ?></p>
<br>
<p><?php echo gettext ("Go to a directory on the server. Copy the backup file there.") ?></p>
<p><?php echo gettext ("Assuming that the backup is called backup.tar.gz, enter this to unpack it:") ?></p>
<code>gunzip backup.sql.gz</code>
<p><?php echo gettext ("This should give a file called backup.sql.") ?></p>
<br>
<p><?php echo gettext ("Start the mysql client:") ?></p>
<code>mysql --host=<?php echo $this->database_host ?> --user=<?php echo $this->database_user ?> --password=<?php echo $this->database_pass ?></code>
<p><?php echo gettext ("It will welcome you to the MySQL monitor.") ?></p>
<p><?php echo gettext ("Use the correct table:") ?></p>
<code>use <?php echo $this->database_name ?></code>
<p><?php echo gettext ("Restore the backup:") ?></p>
<code>source backup.sql</code>
<p><?php echo gettext ("Press enter after the above command, and then let it run till it is through.") ?></p>
<br>
<p><?php echo gettext ("If there are projects that use collaboration through Send/Receive, then their parameters will have been restored, but their internal git repositories will not have been restored.") ?></p>
<p><?php echo gettext ("Such project will not operate correctly. For correct operation these project need to be set up again.") ?></p>
