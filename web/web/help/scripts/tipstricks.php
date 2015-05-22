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
<h2><?php echo Locale_Translate::_("Tips and Tricks") ?></h2>

<h3><?php echo Locale_Translate::_("Extensions and plugins") ?></h3>
<p>
  <?php echo Locale_Translate::_("Browser extensions and plugins may interfere with Bibledit-Web.") ?>
  <?php echo Locale_Translate::_("There was a case that the USFM editor didn't load the text.") ?>
  <?php echo Locale_Translate::_("Disabling an extension in Chrome resolved it.") ?>
</p>

<h3><?php echo Locale_Translate::_("Enable consultants to view the Bible in a workbench") ?></h3>
<p>
  <?php echo Locale_Translate::_("Just add the desired Bible to the Resources through the link organizer.") ?>
  <?php echo Locale_Translate::_("Then add the Resources to the workbench through URL resource/index.php.") ?>
</p>

<h3><?php echo Locale_Translate::_("Backup") ?></h3>
<p>
  <?php echo Locale_Translate::_("Bibledit-Web does not do any backups of its own.") ?>
  <?php echo Locale_Translate::_("Backup is very important.") ?>
  <?php echo Locale_Translate::_("The system administrator should have a reliable backup strategy in place.") ?>
  <?php echo Locale_Translate::_("Bibledit-Web uses SQLite for data storage.") ?>
  <?php echo Locale_Translate::_("If a database is backed up while SQLite is storing data in it, the database may get malformed.") ?>
  <a href="http://www.sqlite.org/faq.html#q21"><?php echo Locale_Translate::_("Information.") ?></a>
  <?php echo Locale_Translate::_("This applies to any database software, not just SQLite.") ?>
  <?php echo Locale_Translate::_("To accomodate backup, Bibledit-Web does not access databases from 0:00 to 0:05 at midnight.") ?>
  <?php echo Locale_Translate::_("If no users access Bibledit-Web during that time, then there is no database access at all.") ?>
  <?php echo Locale_Translate::_("Backup can be safely done during that time.") ?>
</p>

<h3><?php echo Locale_Translate::_("Creating SWORD modules on shared hosting") ?></h3>
<p>
  <?php echo Locale_Translate::_("Bibledit-Web can create a SWORD module from the USFM Bible text.") ?>
  <?php echo Locale_Translate::_("It uses the following program for that:") ?>
  <code>osis2mod</code>.
  <a href="http://www.crosswire.org/sword/index.jsp"><?php echo Locale_Translate::_("It is part of the SWORD library.") ?></a>
  <?php echo Locale_Translate::_("Installing this on shared hosting requires some steps.") ?>
  <?php echo Locale_Translate::_("If for example the shared hosting account runs on Ubuntu 12.04 64 bits, then the following helps.") ?>
  <?php echo Locale_Translate::_("Install the Ubuntu server version in a virtual machine.") ?>
  <?php echo Locale_Translate::_("Install the following packages:") ?>
  <code>sudo apt-get install libsword*</code>.
  <?php echo Locale_Translate::_("Change to the following directory:") ?>
  <code>cd /var/cache/apt/archives</code>.
  <?php echo Locale_Translate::_("Look for the following packages:") ?>
  <code>ls libsword*</code>.
  <?php echo Locale_Translate::_("There will be five packages.") ?>
  <?php echo Locale_Translate::_("Unpack them to a target directory:") ?>
  <code>dpkg-deb -x libsword9*.deb /path/to/target/dir/</code>.
  <?php echo Locale_Translate::_("Copy all unpacked files to the following sample target directory on the shared hosting account:") ?>
  <code>~/domains/yourdomain.org/htdocs/www/bibledit-web/sword</code>.
  <?php echo Locale_Translate::_("Do the same for the following libraries:") ?>
  <code>libclucene*</code>.
  <code>libicu48</code>.
</p>

<h3><?php echo Locale_Translate::_("Syncing") ?></h3>
<p>
  <?php echo Locale_Translate::_("If syncing no longer works, check the Journal for clues.") ?>
</p>
<p>
  <?php echo Locale_Translate::_("The Journal may indicate any of the following:") ?>
  Please tell me who you are.
  Run git config --global user.email "you@example.com".
  Run git config --global user.name "Your Name".
  <?php echo Locale_Translate::_("This may be resolved in two ways:") ?>
</p>
<p>
  1. 
  <?php echo Locale_Translate::_("Manually run the two commands in the appropriate directory from a terminal.") ?>
</p>
<p>
  2. 
  <?php echo Locale_Translate::_("Redo the setup of the Collaboration.") ?>
  <?php echo Locale_Translate::_("It will run those two commands for you.") ?>
</p>

<h3><?php echo Locale_Translate::_("Bible books in custom order") ?></h3>
<p>
  <?php echo Locale_Translate::_("To sort the Bible books in a custom order, do this:") ?>
  <?php echo Locale_Translate::_("Go to menu Bible / Bibles.") ?>
  <?php echo Locale_Translate::_("Select a Bible.") ?>
  <?php echo Locale_Translate::_("Go to Book order.") ?>
  <?php echo Locale_Translate::_("Drag the books into the desired order.") ?>
</p>

<h3><?php echo Locale_Translate::_("Font, text direction and writing mode of the Bible editors") ?></h3>
<p>
  <?php echo Locale_Translate::_("To set the font, the text direction, or the writing mode for the Bible text editors, do this:") ?>
  <?php echo Locale_Translate::_("Go to menu Bible / Bibles.") ?>
  <?php echo Locale_Translate::_("Select a Bible.") ?>
  <?php echo Locale_Translate::_("Go to Font and text direction.") ?>
  <?php echo Locale_Translate::_("Make the settings.") ?>
</p>
