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
<h1>ChangeLog</h1>

<h2>Version in repository</h2>
<p>The tool to remove duplicate consultation notes was renamed to "Clean Consultation Notes".</p>
<p>The tool to clean the consultation notes converts the note summaries to clean text.</p>
<p>Users can opt for having their personal change proposals visible among the Change Notifications online.</p>
<p>The logbook shows for which day of the month the sprint history gets updated.</p>
<p>The clickable elements of the Navigator have titles that show up if the mouse hovers over the links.</p>
<p>The Navigator has a link ↶ for going back in the navigation history, and a link ↷ for going forward.</p>
<p>The Navigator responds to the Alt-Left arrow key press by going back. Alt-Right goes forward.</p>
<p>Personal change proposals are marked with ☺ in the change notifications, and changes in the team's Bible are marked with ♺.</p>
<p>Automatically remove personal change proposals and their matching change notifications.</p>
<p>Updated information about typesetting.</p>
<p>Some bugs in the styles editor were fixed.</p>
<p>The sprint burndown chart gives the business days only, so the chart is smoother and more realistic.</p>
<p>The Spint page uses completion categories for the tasks instead of percentages.</p>
<p>The Sprint page only focuses the entry for adding new tasks when a new task has just been added.</p>
<p></p>
<p></p>
<p></p>
<p></p>

<h2>Version 1.1</h2>
<p>20 September 2013</p>
<p>Improved automatic save in online USFM editor.</p>
<p>Every night it clears out all sessions.</p>
<p>Improved passage navigator: Enter nothing or + for next verse, and - for previous verse.</p>
<p>Internal cleanup of the Consultation Notes editor.</p>
<p>Improved passage detection when editing the passage of a consultation note.</p>
<p>Better user experience while dealing with consultation notes.</p>
<p>Privacy handling was removed from the consultation notes.</p>
<p>Creating a consultation note from a change notification allows the summary and content to be edited.</p>
<p>Expanding a Change Notifications gives "Loading..." feedback.</p>
<p>The menu is available from a Start button.</p>
<p>The menu dynamically moves more often used items to the top.</p>
<p>Can make external resources available offline for faster display.</p>
<p>After login, takes the user straight to the desired function, for a smoother user experience.</p>
<p>Optimized data transfer into collaboration repository.</p>
<p>The translators can also Send/Receive, no longer the managers only.</p>
<p>It has a setting to send and receive every five minutes.</p>
<p>More robust synchronization from the collaboration repository to the Bible database.</p>
<p>A note can be marked to be deleted after a week.</p>
<p>Requires program "merge" in package "rcs".</p>
<p>Resolves conflicts during Send/receive by merging the two versions with grapheme granularity.</p>
<p>The Sword module export links to information about Sword module making.</p>
<p>The installer sets the maximum size of file uploads to 8 Mbyte for PHP.</p>
<p>Time consuming operations on Bibles no longer time out.</p>
<p>Does not delete session files younger than the cookie timeout of 12 hours.</p>
<p>The consultation notes editor is as wide as the browser window.</p>
<p>Adding a comment to a consultation note focuses the notes editor so the user can type straightaway.</p>
<p>For safety the USFM editor only saves a chapter when the length of the new text does not differ more than 20% from the length of the existing text, and the new text is at least 80% similar to the existing text.</p>
<p>Pasting text into the consultation notes editor converts it to plain text.</p>
<p>There is a tool for checking the consistency of the Bible translation.</p>
<p>Subscribed users get a daily email detailing the changes they have entered in the web-based Bible editor.</p>
<p>The caret of the USFM editor scrolls to the center of the browser window.</p>
<p>A vertically moving yellow arrow points to the position of the caret in the Bible editor.</p>
<p>Tasks can be defined for a sprint, and moved to another sprint.</p>
<p>The percentage complete can be set for each task in steps.</p>
<p>Sends weekly email with the sprint planning and a burndown chart to subscribed users.</p>
<p></p>
<p></p>

<h2>Version 1.0</h2>
<p>30 July 2013</p>
<p>Users can set an option to have Change Notifications generated for them.</p>
<p>Consultations / Changes displays all Change Notifications for the user.</p>
<p>Details for each Change Notification include relevant Consultation Notes.</p>
<p>Users can unsubscribe from notes on the Change details page.</p>
<p>Users can unassign notes from themselves on the Change details page.</p>
<p>The manager can view all open Change Notifications for all the users.</p>
<p>Statistics about pending changes and assigned notes will be mailed to subscribed users.</p>
<p>The results of the checks and the suppressed results have been separated into two pages.</p>
<p>Session timeout is now 12 hours.</p>
<p>Users can suppress receiving email from an updated Consultation Note in case the update was made by themselves.</p>
<p>It checks on books, chapters and verses according to the versification system.</p>
<p>Requires Python, Perl, and the Sword utilities.</p>
<p>Exports Bibles to simple and full OSIS formats through usfm2osis.py and through usfm2osis.pl.</p>
<p>Creates Sword modules based on the four OSIS flavours above.</p>
<p>Contains an editor for the Sword configuration file.</p>
<p>Contains information about setting up a Sword module repository from the exported Bibles.</p>
<p>Collaboration is possible with Bible names with spaces.</p>
<p>Support for ssh access to secured repositories works in case the GNU/Linux host server can access such a repository.</p>
<p>Can check whether a paragraph starts with a capital and ends with correct punctuation.</p>
<p>Can check on two punctuation marks in sequence.</p>
<p>Can check on malformed USFM.</p>
<p>Can check on matching opening and closing markers in USFM.</p>
<p>Can check on punctuation at the end of each verse.</p>
<p>Can check on the existence of patterns in the verse text.</p>
<p>Switch from Smarty to Zend_View.</p>
<p>Includes jQuery and jQuery UI.</p>
<p>Improved user experience for displaying the logbook.</p>
<p>Statistics about subscribed consultation notes will be mailed to subscribed users.</p>
<p>The links in the daily email with statistics take the user to the correct selections of consultation notes.</p>
<p>Removes snapshots older than 30 days.</p>
<p>Trims and optimizes the database every night.</p>
<p>The page with Change Notifications is more user-friendly.</p>
<p>Logs removing change notifications.</p>
<p>Logs deleting consultation notes.</p>
<p>The editor for the consultation notes has been simplified.</p>
<p>The Administrator can create, modify, test, and delete scripts for fetching external resources such as Bibles and commentaries.</p>
<p>The user can view internal Bibles and external web snippets as Resources.</p>
<p>Can fetch several commentaries as external resources.</p>
<p>Can convert a Bible to a USFM Resource, and vice versa.</p>
<p>The Resources page has a Navigator.</p>
<p>Provides information about the Scrum-based Bible translation method.</p>
<p>The Navigator can be operated through the mouse and the keyboard.</p>
<p>Information about USFM is no longer included, but links to external sources online.</p>
<p>The text editors use the new Passage Navigator widget.</p>

<h2>Version 0.9</h2>
<p>29 May 2013</p>
<p>The personal notifications settings uses checkboxes for improved clarity.</p>
<p>Users can check the option to be notified by email about deleted consultation notes.</p>
<p>Exports USFM to clear text each midnight.</p>
<p>Clearer emails for the daily changes.</p>
<p>Can access password protected git repositories through the https protocol, as used on github.com.</p>
<p>The layout of the logbook is more compact in case of long entries.</p>
<p>It installs on Ubuntu Server 13.04 which no longer includes the smarty 2 engine.</p>
<p>If any of the regular automatic tasks behaves unexpectedly, information about it will be in the logbook.</p>
<p>Checks on the Bibles run every night.</p>
<p>Results of the checks are visible on the web.</p>
<p>Checks can be enabled or disabled.</p>
<p>Users who subscribe to the Bible checks receive regular emails if there are results of the checks.</p>
<p>It has a check on double spaces in the raw USFM of the Bibles.</p>
<p>It has a check on a full stop at the end of a heading in the Bibles.</p>
<p>It has a check on a space before a comma, semicolon, colon, full stop, question mark, or exclamation mark in the Bible text.</p>
<p>It has a check on a the structure and punctuation of sentences in the Bible text.</p>
<p>Requires the PHP intl module for internationalization support.</p>
<p>User login is written to the logbook.</p>
<p>Contains script update-0.8-to-0.9.php to be manually run when upgrading bibledit-web-0.8 to 0.9.</p>
<p>The Notes Selector searches better on text.</p>
<p>The site search works through MySQL full-text search.</p>
<p>The search results display the Consultations Notes at the left and the Bible text at the right.</p>
<p>No longer requires package sphinxsearch, smarty or smarty-gettext.</p>

<h2>Version 0.8</h2>
<p>1 April 2013</p>
<p>Run ./configure --with-open-installation=true to configure the Bibledit-Web installation from password protected to open.</p>
<p>Processing the changes in a Bible occurs at 0:15 AM.</p>
<p>The import BibleWorks page has a checkbox whether to keep the grammatical tags.</p>
<p>Exports to eSword module.</p>
<p>Many strict warnings were fixed for PHP version 5.4.</p>
<p>Copies the smarty-gettext plugin just in case the package does not install it correctly.</p>
<p>Requires package sphinxsearch.</p>
<p>The link to logout has been moved from the header to the user's page.</p>
<p>Searches the Consultation Notes and the Bibles.</p>
<p>Better display of results of creating chapters in a Bible book.</p>
<p>Improved export from USFM to linked web pages.</p>
<p>The export from USFM to Web has an index and can be searched.</p>
<p>The page with search results in the exported web pages have a back link to the Bible and/or book and/or chapter where it was called from.</p>
<p>The buttons to submit a new note or add a comment change upon click to prevent a multiple submission.</p>
<p>When somebody responds by email to a Consultation Note, and the email address is not recognized, it still posts that response.</p>
<p>Improved selection for Consultation Notes based on the length of the period they have not been edited.</p>
<p>The email notifications of the Consultation Notes have a mailto: link for an easier and cleaner response by email.</p>
<p>The manager can edit the contents of a Consultation Note.</p>
<p>Fixed: Setting up collaboration gave an error message on recent PHP versions.</p>
<p>The mail database was updated so that emails with details about changes in the Bible can have sizes larger than 65535 bytes.</p>

<h2>Version 0.7</h2>
<p>11 October 2012</p>
<p>No longer logs memory usage once a day.</p>
<p>The configure script checks whether the smarty-gettext plugin is present.</p>
<p>No longer logs errors every five minutes when the email storage host has not been defined.</p>
<p>Passage focus clients like Bibledit-Bibleworks and others no longer time out after a few hours of non-use.</p>
<p>Fixed: When the database has not been initialized, the timer won't run repeating tasks, as it thinks that the database server is down.</p>
<p>Fixed: PHP Warning: rmdir(): Directory not empty.</p>
<p>The unit tests clean up their temporal data after finish.</p>
<p>Copyright notices were updated from 2009 to 2012.</p>
<p>The setup.php script is gone. Its functions have been moved to the 'make install' stage.</p>
<p>Installation in a user-defined location is possible through ./configure --with-location=location.</p>
<p>The logbook displays 60 entries per page instead of 20.</p>
<p>No symbolic linking for the bibledit configuration for apache, but store it in the apache conf.d directory.</p>
<p>Creating of Filesystem Hierarchy Standard directories has been centralized in the installer.</p>
<p>The installer secures the created directories.</p>
<p>The backup folder is /var/lib/bibledit-web/bibledit-web so that parallel instances of Bibledit-Web keep separate backups.</p>
<p>CPU-intensive operations like 'Send/receive', 'Export Bibles', and 'Daily changes' run at midnight each day rather than at the time of their most recent manual operation.</p>
<p>It no longer changes the permissions of the exported Bibles.</p>
<p>Backups are created at midnight each day rather than at the time of their most recent manual trigger.</p>
<p>Trimming the databases now happens at midnight.</p>
<p>The operations that run at midnight indicate their identify in the logbook through a prefix.</p>
<p>New export web folder names: Web and PlainWeb.</p>
<p>Imports BibleWorks Bible text.</p>

<h2>Version 0.6.1</h2>
<p>22 February 2012</p>
<p>Fixed: setup.php: Error: Smarty compile directory is not writeable.</p>

<h2>Version 0.6</h2>
<p>20 February 2012</p>
<p>The online help about typesetting procedures was updated.</p>
<p>Improvements in importing the GBS Statenbijbel.</p>
<p>Outdated installation information was removed.</p>
<p>Installs into /usr/share/bibledit-web/www.</p>
<p>Configures Apache for accessing Bibledit-Web at http://localhost/bibledit-web.</p>
<p>No longer uses a poor man's crontab, but sets up the proper GNU/Linux one.</p>
<p>Warnings in timer/index.php send email to the admin every minute. These were eliminated.</p>
<p>The git folders moved from among the web pages to /var/lib/bibledit-web/git.</p>
<p>The daily changes moved from among the web pages to /var/lib/bibledit-web/changes.</p>
<p>The exported Bibles moved from among the web pages to /var/lib/bibledit-web/exports.</p>
<p>The backup moved from among the web pages to /var/lib/bibledit-web.</p>
<p>Reshuffling of data in preparation of creating a Debian package.</p>
<p>The configure script also locates the Zend library on Debian systems.</p>
<p>No longer requires java.</p>
<p>No longer needs or includes a copy of the daisydiff library.</p>
<p>Requires wdiff.</p>
<p>Sheds the included smarty library and instead requires the smarty and smarty-gettext libraries the distro provides.</p>
<p>Security: Cron jobs run as user and group www-data rather than root.</p>
<p>Security: All files owned by user www-data, rather than root.</p>
<p>Locally installed files have been moved from /usr/share/bibledit-web/www to /usr/share/bibledit-web.</p>
<p>Automatic backups work again without first setting permissions and ownership.</p>
<p>Automatic daily generation of lists of changes in the Bibles work again without first setting permissions and ownership.</p>
<p>Daily generated emails detailing changes in the Bible text no longer contain links to the online version.</p>
<p>No longer includes the changed chapters in the set of daily changes.</p>
<p>Automatic responses to comments on Consultation Notes generated by email now have proper links to the online version of this Consultation Note.</p>
<p>The administrator can update a user's email address.</p>

<h2>Version 0.5</h2>
<p>26 August 2011</p>
<p>Select consultation notes that have not been edited for a given time.</p>
<p>Consultation notes: Improved sorting on passage.</p>
<p>Consultation notes: Post comment by email: Both top and bottom posting work.</p>
<p>Exports Bibles to files suitable for the Online Bible module creation process.</p>
<p>The Administrator can set the default language for the site.</p>
<p>Uses the system's Zend Framework instead of the partial one that was included.</p>
<p>User sessions persist for 12 hours.</p>
<p>When selecting Consultation Notes by Bible, the actual Bible switches along with it.</p>
<p>Provides help with typesetting through Adobe InDesign.</p>
<p>New lines are replaced with spaces in comments that get posted by email. Less clutter.</p>
<p>Can export to web pages with links for easier navigation.</p>
<p>Properly exports combined verses such as 3-4, 2b-4, 5,6, and so on.</p>
<p>Email setup was clarified.</p>
<p>Installation help was updated.</p>

<h2>Version 0.4</h2>
<p>20 April 2011</p>
<p>ChangeLogs have been moved from http://sites.google.com/site/bibledit to here.</p>
<p>Requires a Java Runtime Environment.</p>
<p>Pages detailing recent changes in the Bible texts are generated daily.</p>
<p>The users can opt to receive emails detailing recent changes in the Bible texts.</p>
<p>Exports selected Bibles to USFM on a daily basis.</p>
<p>Can export extra information for each Bible: word lists, abbreviations, table of contents, and more.</p>
<p>Import USFM data into a Bible through file upload.</p>
<p>Requires zip and unzip.</p>
<p>Daily export of selected Bibles to OpenDocument files in various editions: standard, text only, text with note citations, and notes only.</p>
<p>Hyphenator can insert soft hyphens into Bibles at defined locations.</p>
<p>Automatic Send/Receive once a day.</p>
<p>Logbook refreshes the page less often.</p>
<p>Updated helpfiles, including one about Bible typesetting through LibreOffice.</p>
<p>Export Bibles to web pages.</p>
<p>Older changes sets are archived instead of deleted.</p>

<h2>Version 0.3</h2>
<p>4 March 2011</p>
<p>Tool for automatic deletion of duplicate Consultation Notes.</p>
<p>Bulk delete operation for Consultation Notes.</p>
<p>Clicking a Consultation Note to open it will update the Navigator to the appropriate verse.</p>
<p>Assigning a Consultation Note to a user will optionally email him.</p>
<p>User can add comments to Consultation Notes by email.</p>
<p>User can create new Consultation Notes by email.</p>

<h2>Version 0.2</h2>
<p>27 September 2010</p>
<p>Many improvements were made.</p>

<h2>Version 0.1</h2>
<p>26 June 2010</p>
<p>Initial release.</p>
