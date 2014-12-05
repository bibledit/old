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
<h2><?php echo gettext("Tips and Tricks") ?></h2>

<h3><?php echo gettext("Extensions and plugins") ?></h3>
<p>
  <?php echo gettext("Browser extensions and plugins may interfere with Bibledit-Web.") ?>
  <?php echo gettext("There was a case that the USFM editor didn't load the text.") ?>
  <?php echo gettext("Disabling an extension in Chrome resolved it.") ?>
</p>

<h3><?php echo gettext("Enable consultants to view the Bible in a workbench") ?></h3>
<p>
  <?php echo gettext("Just add the desired Bible to the Resources through the link organizer.") ?>
  <?php echo gettext("Then add the Resources to the workbench through URL resource/index.php.") ?>
</p>

<h3><?php echo gettext("Backup") ?></h3>
<p>
  <?php echo gettext("Bibledit-Web does not do any backups of its own.") ?>
  <?php echo gettext("Backup is very important.") ?>
  <?php echo gettext("The system administrator should have a reliable backup strategy in place.") ?>
  <?php echo gettext("Bibledit-Web uses SQLite for data storage.") ?>
  <?php echo gettext("If a database is backed up while SQLite is storing data in it, the database may get malformed.") ?>
  <a href="http://www.sqlite.org/faq.html#q21"><?php echo gettext("Information.") ?></a>
  <?php echo gettext("This applies to any database software, not just SQLite.") ?>
  <?php echo gettext("To accomodate backup, Bibledit-Web does not access databases from 0:00 to 0:05 at midnight.") ?>
  <?php echo gettext("If no users access Bibledit-Web during that time, then there is no database access at all.") ?>
  <?php echo gettext("Backup can be safely done during that time.") ?>
</p>

<h3><?php echo gettext("Creating SWORD modules on shared hosting") ?></h3>
<p>
  <?php echo gettext("Bibledit-Web can create a SWORD module from the USFM Bible text.") ?>
  <?php echo gettext("It uses the following program for that:") ?>
  <code>osis2mod</code>.
  <a href="http://www.crosswire.org/sword/index.jsp"><?php echo gettext("It is part of the SWORD library.") ?></a>
  <?php echo gettext("Installing this on shared hosting requires some steps.") ?>
  <?php echo gettext("If for example the shared hosting account runs on Ubuntu 12.04 64 bits, then the following helps.") ?>
  <?php echo gettext("Install the Ubuntu server version in a virtual machine.") ?>
  <?php echo gettext("Install the following packages:") ?>
  <code>sudo apt-get install libsword*</code>.
  <?php echo gettext("Change to the following directory:") ?>
  <code>cd /var/cache/apt/archives</code>.
  <?php echo gettext("Look for the following packages:") ?>
  <code>ls libsword*</code>.
  <?php echo gettext("There will be five packages.") ?>
  <?php echo gettext("Unpack them to a target directory:") ?>
  <code>dpkg-deb -x libsword9*.deb /path/to/target/dir/</code>.
  <?php echo gettext("Copy all unpacked files to the following sample target directory on the shared hosting account:") ?>
  <code>~/domains/yourdomain.org/htdocs/www/bibledit-web/sword</code>.
  <?php echo gettext("Do the same for the following libraries:") ?>
  <code>libclucene*</code>.
  <code>libicu48</code>.
</p>

<h3><?php echo gettext("Syncing") ?></h3>
<p>
  <?php echo gettext("If syncing no longer works, check the Journal for clues.") ?>
</p>
<p>
  <?php echo gettext("The Journal may indicate any of the following:") ?>
  Please tell me who you are.
  Run git config --global user.email "you@example.com".
  Run git config --global user.name "Your Name".
  <?php echo gettext("This may be resolved in two ways:") ?>
</p>
<p>
  1. 
  <?php echo gettext("Manually run the two commands in the appropriate directory from a terminal.") ?>
</p>
<p>
  2. 
  <?php echo gettext("Redo the setup of the Collaboration.") ?>
  <?php echo gettext("It will run those two commands for you.") ?>
</p>

<h3><?php echo gettext("Bible books in custom order") ?></h3>
<p>
  <?php echo gettext("To sort the Bible books in a custom order, do this:") ?>
  <?php echo gettext("Go to menu Bible / Bibles.") ?>
  <?php echo gettext("Select a Bible.") ?>
  <?php echo gettext("Go to Book order.") ?>
  <?php echo gettext("Drag the books into the desired order.") ?>
</p>

<h3><?php echo gettext("Font, text direction and writing mode of the Bible editors") ?></h3>
<p>
  <?php echo gettext("To set the font, the text direction, or the writing mode for the Bible text editors, do this:") ?>
  <?php echo gettext("Go to menu Bible / Bibles.") ?>
  <?php echo gettext("Select a Bible.") ?>
  <?php echo gettext("Go to Font and text direction.") ?>
  <?php echo gettext("Make the settings.") ?>
</p>
