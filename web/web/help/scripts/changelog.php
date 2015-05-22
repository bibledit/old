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
<h1>ChangeLog</h1>

<h2>Version in repository</h2>
<p>Fixed: Cannot setup collaboration.</p>
<p></p>

<h2>Version 1.9</h2>
<p>11 November 2014</p>
<p>Kills tasks that run for more than a few days, instead of six hours as previously.</p>
<p>Scheduled tasks can no longer be prioritized.</p>
<p>The offline external resources are now stored in a database per book rather than to file per verse, to reduce the number of small files.</p>
<p>Fixed: Large amounts of changes jam the nightly generation of change notifications.</p>
<p>Sending and receiving consultation notes stops when the systems notice a relevant damaged or unavailable database on client or server.</p>
<p>Fixed: Large amount of entries in the journal jam the rotation mechanism.</p>
<p>Fetches external resources from the new website of studylight.org.</p>
<p>Includes phpLiteAdmin for database administration.</p>
<p>Simplified journal display.</p>
<p>The Journal displays no more than 1000 entries initially.</p>
<p>More checksumming for better validation of Bible data sent from the server to the client.</p>
<p>If a Bible chapter on a client is empty for some reason, the client does not submit this chapter to the server.</p>
<p>When the user enables client mode, the client immediately schedules a sync operation.</p>
<p>Fixed: With a corrupted change notifications database, the default Bible for a user may change.</p>
<p>Fixed: Quickly deleting many change notifications may corrupt the change notifications database.</p>
<p>Does not send a Sprint mail when the Sprint has no tasks.</p>
<p>Fixed: Click in a new verse in a Bible editor, but the navigator stays in the former position when the network is poor.</p>
<p>Fixed: On slower connections, if the top bar gets added, this makes the rest of the page to jump down.</p>
<p>The ✗ and ✎ buttons of the workbench configurations are farther apart.</p>
<p>The ✗ button asks the user for confirmation before deleting the workbench configuration.</p>
<p>Workbench configurations for a user made on the server are available to the same user on the client.</p>
<p>Client does not send and receive to a git Bible repository.</p>
<p>Client synchronizes the local offline external resources with the ones on the Server.</p>
<p>Client synchronizes the local USFM resources with the ones on the Server.</p>
<p>Some scripts do not run when Bibledit is in setup mode or when client mode is prepared but not yet enabled.</p>
<p>Bibledit for Android was released.</p>
<p>The client only synchronizes the Bibles on the server to the client after the client has sent all local edits successfully.</p>
<p>The change notifications for a user also work when the client sends Bible changes to the server, not only when a user enters changes online.</p>
<p>Fixed: The Bible editor changed a footnote like "\fk keyword \ft text" to "\fk keyword \fk text" losing the \ft markup.</p>
<p>When going to another passage, and all resources have not yet been fetched, it cancels all pending requests, making the Resources viewer more responsive.</p>
<p>Truncates huge journal entries to avoid disk full errors.</p>
<p>Fixed: Sending and receiving through more recent versions of git create many entries in the journal.</p>
<p>Bibledit for Windows was released.</p>
<p>Support for Lighty: A fast and efficient webserver.</p>
<p>Fixed: The links from the statistics emails no longer work via the login mechanism.</p>
<p>Fixed: The caret jumps from verse to verse while navigating on a slow connection or slow website.</p>
<p>Fixed: Focus verse 1 in the Bible editor, use the mouse to scroll down, and click on the last verse in the chapter: The editor scrolls back to verse 1, and then back to the last verse.</p>
<p>Instructions for installing Bibledit-Web on iOS: iPad and iPhone.</p>
<p>Support for PCLinuxOS 2014.</p>
<p>Disable the user menu in client mode.</p>
<p>Use Ezra SIL as web font for Hebrew Resources, and Cardo for Greek Resources, because most tablets do not provide for installing fonts.</p>
<p>Redirection is now conform the standard.</p>
<p>External Resources cannot be deleted in an open installation.</p>
<p>No longer requires the PHP iconv extension.</p>

<h2>Version 1.8</h2>
<p>7 August 2014</p>
<p>The notes in the Bible editor are numbered, making it is easier to see in the editor which note belongs to which caller in the text.</p>
<p>Fixed: Cannot insert endnote because the style (fe) consists of more than one character.</p>
<p>It is no longer needed to clear the browser's cache after upgrading Bibledit-Web.</p>
<p>Fixed: The Hebrew and Greek of the Biblehub Interlinear filter display too small on a tablet and far too small on a phone.</p>
<p>With an unstable connection to the server, the Bible editors keep trying to save the Bible text, till they succeed.</p>
<p>Displays no more than 1000 change notifications on the page, to prevent a timeout in case there are thousands of them.</p>
<p>Fixed: Importing or copying a whole Bible jams the Change Notifications.</p>
<p>The Administrator can also select Consultation Notes that refer to a non-existing Bible.</p>
<p>The three Bible editors use checksumming for loading and saving, to cope with unstable internet connections.</p>
<p>Fixed: The eSword export grows very large over time.</p>
<p>Fixed: With an unstable internet connection the focused passage may jump from one verse to another.</p>
<p>View the journal in the user's timezone.</p>
<p>The login link is more visible.</p>
<p>Automatic repair of damaged consultation notes index.</p>
<p>Smart update of consultation notes index.</p>
<p>Improved stability of the Bible editor.</p>
<p>When a Client connects to the Server with correct credentials, the client erases all local users and keeps the correct credentials.</p>
<p>During setup, it can be set to server mode or to client mode.</p>
<p>When the system has no users, it goes into client mode, and displays the page to connect to a Bibledit-Web server.</p>
<p>In client mode the user is always automatically logged in.</p>
<p>Each time the client synchronizes its data with the server, it requests the user's role from the server and applies it to the client.</p>
<p>Syncing sends updated Bible data from client to server.</p>
<p>During sync the client receives updated Bible data from the server.</p>
<p>Does not export to OSIS and to SWORD modules.</p>
<p>Includes the Zend Framework.</p>
<p>Client mode is always visible in an open installation.</p>
<p>The sync page shows relevant information depending whether the web app runs in either server mode or client mode.</p>
<p>In case of an error while fetching a resource, the browser will retry.</p>
<p>No longer requires the PHP Tidy extension.</p>
<p>More robust Consultation Notes databases.</p>
<p>Fixed: Timeouts while clients synchronize the Consultation Notes.</p>
<p>Faster Journal display.</p>
<p>Better merging of changes on client and server.</p>
<p>Atomic submission of Bible edits to the server.</p>
<p>Faster checksum calculation for the consultation notes.</p>
<p>Many shell commands have been removed in preparation for running on Android.</p>
<p>Fixed: Warnings when comparing Bibles and/or USFM Resources.</p>
<p>Only admins can see login failures in the Journal.</p>
<p>The Journal issues a warning in case the minutely tasks do not run regularly.</p>
<p>The Setup creates more databases.</p>
<p>The Client easily connects to the Demo Server.</p>
<p>An open installation regularly checks and updates its own contents.</p>
<p>Displays warnings when the Client is connected to the open public demo Server.</p>
<p>When a client connects to a server for the first time, it enables the option for regular send / receive.</p>
<p>Can automatically send and receive every five minutes, every hour, or never.</p>
<p>Improved tasks scheduler.</p>
<p>Does not run more than one set of sync actions simultaneously to support slower computing devices or slower network speeds.</p>

<h2>Version 1.7</h2>
<p>4 June 2014</p>
<p>The database with Strong's numbers and English glosses is created after setup.</p>
<p>The Hebrew and Greek databases are created after setup.</p>
<p>Resource filter for Westminster Leningrad Codex from biblehub.com.</p>
<p>Nightly cleanup of the folder structure of deleted consultation notes.</p>
<p>Client mode.</p>
<p>Installation on Mac OS X.</p>
<p>Fixed: The references in the NET Bible were invisible.</p>
<p>Localization through Zend_Translate.</p>
<p>The web server no more starts shell processes of its own, but offloads them to the shell server.</p>
<p>Fixed: Enter two or more notes in the text editor: After reload, the notes move to the end of the chapter.</p>
<p>Fixed: Click in a note: The display scrolls the note out of view and the focused verse into view.</p>
<p>Updated padding in the Bible editor.</p>
<p>Email is unavailable in Client mode.</p>
<p>Client mode disables the notifications for the Consultation Notes.</p>
<p>It is easier to add a style to a stylesheet.</p>
<p>Sort the Bible books in any order.</p>
<p>In general it uses the font the system and browser come up with.</p>
<p>Set font, text direction and writing mode for the Bible editors.</p>
<p>Upload font for the Bible editors.</p>
<p>Fixed: Could not create consultation notes for a Bible with a space in the name.</p>
<p>Fixed: The Journal does not properly display added entries.</p>
<p>The clients send the consultation notes to the server.</p>
<p>The clients receive updated consultation notes from the server.</p>

<h2>Version 1.6</h2>
<p>15 March 2014</p>
<p>Fixed: Setup in Firefox fails on the AJAX calls for the database scripts.</p>
<p>Uses manually installed osis2mod in the sword directory of the bibledit installation on shared hosting.</p>
<p>Displays parallel resources based on matching content, not on matching passage.</p>
<p>Fixed: Users were getting online change notifications for Bibles they didn't have read access to.</p>
<p>Continues to search and replace in the Bible indicated on the respective pages after changing the active Bible.</p>
<p>Fixed: One can assign anybody a role higher than himself.</p>
<p>More robust background task runner.</p>
<p>Speed up deleting change notifications.</p>
<p>When the editor navigates to verse 0 in a chapter, it scroll that verse into view.</p>
<p>Exporting a Bible uses the proper sequence for USFM > OSIS > SWORD.</p>
<p>Can search and replace in the underlying USFM of a Bible.</p>
<p>More robust search database.</p>
<p>Fixed: With many changes, the statistics are sent too early, before the changes have been processed.</p>
<p>Removes expired temporal directories.</p>
<p>Fixed: Can assign a note to any user, not just users in the relevant translation team.</p>
<p>Clicking a passage in the Consistency tool, in the Change Notifications, and in the Checks, properly opens the Bible editor at that passage.</p>
<p>More robust inter process communications database.</p>
<p>Can edit text of one verse in USFM.</p>
<p>Can insert footnotes in the online Bible editor.</p>
<p>Fixed: If one book has changed, all other books get exported as well.</p>
<p>Fix: The filter for Biblehub Interlinear now links to the grammatical information online.</p>
<p>Fixed: After changing the focused passage it may jump back.</p>
<p>Mattias Põldaru made the passage navigator work in Firefox.</p>
<p>Print a portion of selected Resources.</p>
<p>Daily check on the integrity of the search database and recreate if necessary.</p>
<p>Smart updating of the search database to save processing power.</p>
<p>Support for nested character markup in USFM.</p>
<p>The installer is clearer about Bibledit-Web needing write access to certain folders.</p>
<p>The bulk notes updater remembers the identifiers of the notes it operates on for safer bulk operations.</p>
<p>Set the abbreviations of the books of a Bible.</p>
<p>Tool for copying cross references from a source Bible to a destination Bible.</p>
<p>Tool for easily moving cross references to a previous or a next word.</p>
<p>Exports to any of the supported formats can be enabled individually.</p>
<p>Bible data exported to USFM and OpenDocument can be secured with a password.</p>
<p>The page to edit the note source focuses the entry for the note identifier.</p>
<p>Fixed: Editing note summary skips the bit after a quote.</p>
<p>Importing BibleWorks data works again.</p>
<p>The Bible editors retry after save failure.</p>
<p>Finds similar verses in the active Bible.</p>
<p>Finds verses similar to the focused verse with regard to their Strong's numbers.</p>
<p>Finds all verses with a certain Strong's number.</p>
<p>Finds verses similar to the focused verse with regard to their Hebrew or Greek words.</p>
<p>Fixed: Online Bible editing during send/receive reverts the most recent changes.</p>
<p>A Resource hides when it is has no content for a passage.</p>
<p>Resource with links to the Blue Letter Bible</p>
<p>Focusing a change notification also focuses a Bible to the corresponding Bible passage.</p>
<p>The Bible text editor can apply monospace USFM styles.</p>
<p>Displays the paragraph and character styles in the message bar of the Bible editor.</p>
<p>Fixed: Notes created from Change Notifications set their Bible to Unknown.</p>
<p>Displaying all styles in the Bible editor also displays the category for each style.</p>
<p>The page for managing exports links to The Bible Drop Box.</p>

<h2>Version 1.5</h2>
<p>8 January 2014</p>
<p>Fixed: Selecting a style from the list of all styles did not apply that style to the text.</p>
<p>Fixed: After the initial save, the editor saves the chapter even without real changes in the text.</p>
<p>Fixed: The Bible editor sometimes reloads the text without apparent reason.</p>
<p>Fixed: In Chrome, the Bible editor loses the caret and focus after reloading the text.</p>
<p>Fixed: The Alt / Ctrl Shift keys trigger save actions in the Bible editor.</p>
<p>Fixed: Spurious scrolling of the Bible editor window in some edge cases </p>
<p>Fixed: Alt-Up or Alt-Down go to another verse, but soon after the focus returns to the old verse.</p>
<p>If the caret had focus before reloading the Bible editor, it keeps the focus after the reload.</p>
<p>When the Bible editor receives focus, if necessary it moves the caret to the correct verse.</p>
<p>Opening another Bible in the editor optionally updates the focused book to one that exists in that Bible.</p>
<p>The defined workbenches can be selected straight from the menu.</p>
<p>Improved web-based installer.</p>
<p>Search page.</p>
<p>Can upgrade Bibledit-Web version 1.4, but no older versions.</p>
<p>The binary 'git' is no longer required at installation time. It is only required for distributed collaboration.</p>
<p>Fixed: Search on empty string in search entry in the header results in errors and very many hits.</p>
<p>Uses a temporary directory in the web space because on shared hosting the system's temporal directory may have read or write failures.</p>
<p>More robust task runner.</p>
<p>For greater robustness all important data is stored on the filesystem, not in a database.</p>
<p>Manual refresh of the search index.</p>
<p>Regularly cleans the temporary directory.</p>
<p>Search/Replace page.</p>
<p>The logbook is made more robust and is now called the Journal.</p>
<p>Consistency checking tool also works with longer lists of passages.</p>
<p>Fixed: Setup times out during detection of the Zend libraries in case the web server contains many files.</p>
<p>Advanced search page.</p>
<p>The installer asks whether to install the cron tasks on the web server, or to install them manually, because some hosting providers prefer no cron tasks on their web server.</p>
<p>The Bible data and the Consultation Notes are stored in a structure in the filesystem for greater robustness, rather than in databases that can be corrupted easily.</p>
<p>The installer copies the Bible data and the Consultation Notes from the databases to the filesystem.</p>
<p>Recreates the Bible and Notes index every night.</p>
<p>Records changes in the filesystem instead of a database.</p>
<p>Order, remove, create and edit the workbenches in the workbench organizer.</p>
<p>The workbench editor has sample configurations.</p>
<p>One user can login from different browsers simultaneously.</p>
<p>Users can be granted access to Bibles to form translation teams.</p>
<p>A user in a role with access to a Bible can be set to have read-only access.</p>
<p>Checks are managed and run per Bible.</p>
<p>Bible checks can run in parallel.</p>
<p>Users have access to consultation notes that refer to Bibles they have access to.</p>
<p>The history page displays items for Bibles a user has access to.</p>
<p>The change notifications system deals with multiple translation teams.</p>
<p>The export of Bibles handles multiple-team setups.</p>
<p>Website search in Bibles and Notes the user has read access to.</p>
<p>After changing the Sprint month and year it reverts to the current month and year after two days.</p>
<p>The binary "sqlite3" is no longer used.</p>
<p>More efficiently fetches external resources without forking.</p>

<h2>Version 1.4</h2>
<p>12 December 2013</p>
<p>Mattias Põldaru styled the passage navigator and the buttons.</p>
<p>No longer installs through ./configure && make && sudo make install.</p>
<p>The new web-based graphical installer can install the software on shared hosting.</p>
<p>The locations for exports and changes have been updated due to restrictions of shared hosting.</p>
<p>The binaries sqlite3, python, perl, and osis2mod are no longer required, but optional.</p>
<p>Runs on CentOS 6.4.</p>
<p>The menu in the top bar works on touch-enabled devices.</p>
<p>No longer requires wdiff, unzip, merge, and tidy.</p>
<p>Requires package php5-tidy and php5-sqlite.</p>
<p>No longer requires a full (virtual) server, but also runs on an inexpensive shared Linux hosting account.</p>
<p>Database credentials are no longer visible in an open installation.</p>
<p>The header of the Bible editor remains always visible.</p>
<p>Clicking in the USFM or Bible editor updates the focused verse again.</p>
<p>Scrivener's Greek text was added to the Resources.</p>
<p>The Bible overview gives the number of books it contains.</p>
<p>Regenerates session files less often.</p>
<p>Clears out logbook entries older than 5 days.</p>
<p>Since the MySQL server may go away or hit the maximum number of concurrent connections on shared hosting, all databases now use SQLite.</p>
<p>New task runner handles increased load with a much smaller memory footprint.</p>
<p>Re-creates exported files only when the Bibles have changed.</p>
<p>Compares a Bible with another Bible or with a USFM Resource.</p>
<p>Parallel processing speeds up downloading external Resources.</p>
<p>More stable persistent login mechanism.</p>
<p>A login expires after a day of inactivity.</p>
<p>Fixed: The Biblehub Interlinear external Resource floated at the side of other Resources.</p>
<p>No longer clears out session files every night, because these files are no longer used.</p>
<p>Refreshes the search index every night.</p>
<p>A lot of unit tests were written on all the database objects.</p>
<p>It keeps the history of the changes in the Bibles indefinitely.</p>
<p>Does not automatically maintain the databases.</p>
<p>Backup is left to the hosting provider.</p>

<h2>Version 1.3</h2>
<p>11 November 2013</p>
<p>Fixed: Bibledit focus clients fetch the wrong passage focus from Bibledit-Web.</p>
<p>No animation anymore after clicking notes in the change notifications, or clicking passages.</p>
<p>The Change History page displays where and when changes were made by whom.</p>
<p>The Change History can be filtered on book / chapter / verse / author.</p>
<p>The Workbench now also works in Firefox.</p>
<p>When text has been added in the USFM editor near the end of a verse, the navigation no longer jumps to the next verse.</p>
<p>The Change Notifications provide feedback about the active filter: all / personal / team.</p>
<p>The widths of the columns in the sprint burndown chart are all equal.</p>
<p>The Bible portions formatted to html use an external stylesheet instead of internal styles.</p>
<p>Supports Ubuntu 13.10.</p>
<p>The ./configure script checks for PHP JSON support.</p>
<p>Fixed: In certain circumstances the "personal" change notifications are more than "all" of them.</p>
<p>User setting for holding on to email generated when creating a consultation note, and send it in the evening.</p>
<p>The keyboard shortcuts for the Navigator also work from within the Workbench.</p>
<p>Alt-Down goes to the next verse, and Alt-Up goes to the previous verse.</p>
<p>Security fix for revealing database password.</p>
<p>Includes sample JavaScript for Adobe InDesign.</p>
<p>Better uninstallation and faster configuration reload for web server.</p>
<p>Installer requires package autoconf.</p>
<p>Bible editor with formatted USFM.</p>
<p>The Edit link starts the formatted Bible editor.</p>
<p>The Translate link was renamed to [usfm].</p>
<p>The focused control has a blue glow for easier operation by the keyboard.</p>
<p>Mattias Põldaru styled the top bar.</p>
<p>The menus no longer self-organize based on the number of clicks.</p>
<p>The Bible editor has a style button for applying a selection of the USFM styles.</p>

<h2>Version 1.2</h2>
<p>12 October 2013</p>
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
<p>The sprint burndown chart gives the business days only, so the graph is smoother and more realistic.</p>
<p>The Spint page uses completion categories for the tasks instead of percentages.</p>
<p>The Sprint page focuses the entry for adding new tasks when a new task has just been added.</p>
<p>Export to e-Sword now also works with Bibles that have spaces in their names.</p>
<p>Compresses USFM files to a zip file during export.</p>
<p>Fix: Uploading a zipped file with USFM files imports the USFM files.</p>
<p>The table for storing Bible data in the database was optimized for better performance.</p>
<p>Pages not found redirect to the home page.</p>
<p>Bibledit-Web now has its own repository and project hosted at https://savannah.nongnu.org/projects/bibledit-web.</p>
<p>Bibledit-Web now has its own wiki at https://sites.google.com/site/bibleditweb.</p>
<p>The yellow highlighting no longer affects the capitalization of the search results.</p>
<p>A default external resource has been added that displays the Elberfelder Bibel from www.bibleserver.com.</p>
<p>Recognizes Roman numerals like I, II and III in book names.</p>
<p>Workbench to display internal and/or external pages side by side.</p>
<p>Selectable workbench configurations.</p>
<p>The user can view all the change notifications, only the personal ones, or only the team ones.</p>
<p>All pages use jQuery.</p>
<p>The Navigator has moved to the top bar.</p>
<p>Bibledit-Web processes running for more than five hours get killed as they are considered stuck.</p>
<p>Edit two or more Bibles simultaneously side by side in a workbench.</p>
<p>The interlinear Bible from biblehub.com has been added to the Resources.</p>
<p>Requires program "tidy".</p>
<p>The text of the Dutch Statenbijbel has been added to the Resources.</p>
<p>The Resources include the Dutch Statenbijbel Plus: The canonical text, plus headers, introductions, cross references, and notes.</p>
<p>Clicking passages on some Resources navigates to that passage.</p>
<p>Clicking a passage in the Change Notifications navigates to that passage.</p>
<p>Two new resources: King James Version with only canonical text, and one with all the headers, introductions, cross references, and notes as well.</p>
<p>The notes editor has some preset selections to be activated from the URL.</p>
<p>Activating notes from the change notifications opens the notes in an existing note editor if available, else in a new tab in the browser.</p>
<p>The USFM editor saves the text more reliably at page unload.</p>

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
