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
<h1><?php echo gettext("Introduction") ?></h1>
<p><?php echo gettext("Bibledit is a web-based application for translating the Bible. It is part of the Bibledit suite.") ?></p>
<br>
<p><?php echo gettext("People can sign up for an account and log in.") ?></p>
<p><?php echo gettext("Each user has a certain role within the application.") ?></p>
<p><?php echo gettext("The first role is Member. A member is somebody who has signed up to the site, and has confirmed this signup through his email.") ?></p>
<p><?php echo gettext("The second role is Consultant. Consultants are people who can be consulted because of their knowledge of the original language such as Hebrew or Greek, or because of their knowledge of their target language in case they are language scholars, or for any other knowledge they have.") ?></p>
<p><?php echo gettext("The next role is Translator. These are the ones that have all the rights of the Consultant, but in addition can change the Bible text.") ?></p>
<p><?php echo gettext("Then there's the Manager. The manager has all the privileges of the previous levels, but in addition can add books to the Bible, delete books, add chapters, copy Bibles, and so on. The manager can assign privileges to the translators, consultants, and so on.") ?></p>
<p><?php echo gettext("Finally, there is the Administrator. The administrator has all rights to do anything, and will do the technical setup and maintenance of the site.") ?></p>
<br>
<p><?php echo gettext("Everybody who has an account can enable the personal notifications. Those who have switched the relevant notifications on can receive email from Bibledit-Web. They receive email when new consultation notes get created, or when consultation notes get changed, or when consultation notes get assigned to them. They can also get automatic daily email detailing the changes that have been made in the Bible since the previous email.") ?></p>
<br>
<p><?php echo gettext("Everybody who has an account can create consultation notes by email, and respond to these by email.") ?></p>
<br>
<p><?php echo gettext("Members can view and download all exported Bibles and lists with recent changes in the Bibles.") ?></p>
<br>
<p><?php echo gettext("Consultants can create consultation notes online, view these, set parameters for these, assign the notes to people, and delete notes.") ?></p>
<br>
<p><?php echo gettext("Translators can do the following things:") ?></p>
<p>* <?php echo gettext("Edit the Bible text.") ?></p>
<p>* <?php echo gettext("Switch the stylesheet.") ?></p>
<p>* <?php echo gettext("View and suppress results of the automatic checks.") ?></p>
<br>
<p><?php echo gettext("Managers can do several things:") ?></p>
<p>* <?php echo gettext("Edit the Bible in raw USFM format.") ?></p>
<p>* <?php echo gettext("Send and receive the Bible text. This means that they can synchronize the Bible text with a remote git repository. In this way they can collaborate with people who use Bibledit-Gtk.") ?></p>
<p>* <?php echo gettext("View and clear the logbook.") ?></p>
<p>* <?php echo gettext("Process the changes, so that lists are created, and email is sent out.") ?></p>
<p>* <?php echo gettext("View and edit people who have an account, and create new accounts.") ?></p>
<p>* <?php echo gettext("Create and delete Bibles, books, chapters, and import USFM data.") ?></p>
<p>* <?php echo gettext("Initiate the daily export of selected Bibles to OpenDocument, USFM, html, Online Bible.") ?></p>
<p>* <?php echo gettext("Create and edit versification systems.") ?></p>
<p>* <?php echo gettext("Create and edit stylesheets.") ?></p>
<p>* <?php echo gettext("Import consultation notes from Bibledit-Gtk and remove duplicates.") ?></p>
<p>* <?php echo gettext("Deal with automatic hyphenation when exporting to OpenDocument.") ?></p>
<p>* <?php echo gettext("Make settings for the automatic check and select Bibles to run them on.") ?></p>
<br>
<p><?php echo gettext("Administrator can do several things:") ?></p>
<p>* <?php echo gettext("Set up Bibledit-Web's email account.") ?></p>
<p>* <?php echo gettext("Create backups and restore them.") ?></p>
<p>* <?php echo gettext("Set up collaboration through setting up a connection with a remote git repository.") ?></p>
<p>* <?php echo gettext("Maintain the database.") ?></p>
<p>* <?php echo gettext("Set the site's timezone.") ?></p>
<p>* <?php echo gettext("Set the site's language.") ?></p>
<p>* <?php echo gettext("View the PHP Info.") ?></p>
