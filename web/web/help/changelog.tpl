<h1>{t}ChangeLog{/t}</h1>

<h2>Version in repository</h2>
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
<p>Fixed: Setting up collaboration works gave an error message on recent PHP versions.</p>
<p>The mail database was updated so that emails with details about changes in the Bible can have sizes larger than 65535 bytes.</p>
<p></p>
<p></p>
<p></p>

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
<p>No longer uses a poor man's crontab, but sets up the proper Linux one.</p>
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
<p>Many improvements were made</p>

<h2>Version 0.1</h2>
<p>26 June 2010</p>
<p>Initial release</p>
