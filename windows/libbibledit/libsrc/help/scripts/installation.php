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
<h1><?php echo Locale_Translate::_("Installation") ?></h1>

<h2><?php echo Locale_Translate::_("Standard installation") ?></h2>
<p><a href="installwindows.php"><?php echo Locale_Translate::_("Bibledit-Web on Windows") ?></a></p>
<p><a href="installandroid.php"><?php echo Locale_Translate::_("Bibledit-Web on Android") ?></a></p>
<p><a href="installmacosx.php"><?php echo Locale_Translate::_("Bibledit-Web on Mac OS X") ?></a></p>
<p><a href="installios.php"><?php echo Locale_Translate::_("Bibledit-Web on iOS: iPad and iPhone") ?></a></p>
<p><a href="installnewubuntu.php"><?php echo Locale_Translate::_("Bibledit-Web on Ubuntu through Apache") ?></a></p>
<p><a href="installubuntulighty.php"><?php echo Locale_Translate::_("Bibledit-Web on Ubuntu through Lighty") ?></a></p>
<p><a href="installcentos.php"><?php echo Locale_Translate::_("Bibledit-Web on CentOS") ?></a></p>
<p><a href="installpclinuxos.php"><?php echo Locale_Translate::_("Bibledit-Web on PCLinuxOS") ?></a></p>

<h2><?php echo Locale_Translate::_("Upgrading") ?></h2>
<p>
  <?php echo Locale_Translate::_("A certain release of Bibledit-Web can upgrade the most recent previous release, but not older releases.") ?>
  <?php echo Locale_Translate::_("A new release can be installed over the previous release.") ?>
  <?php echo Locale_Translate::_("There should be no loss of information.") ?>
  <?php echo Locale_Translate::_("Create a backup of the web space before upgrading, just to be sure.") ?>
</p>

<h2><?php echo Locale_Translate::_("Client") ?></h2>
<p>
  <?php echo Locale_Translate::_("Bibledit-Web normally runs in Server mode.") ?>
  <?php echo Locale_Translate::_("It can also be set to Client mode.") ?>
  <?php echo Locale_Translate::_("A Client stores the data offline.") ?>
  <?php echo Locale_Translate::_("You can translate the Bible on a Client.") ?>
  <?php echo Locale_Translate::_("The user needs to synchronize the data on the client with the data on the server regularly.") ?>
  <?php echo Locale_Translate::_("A Client is useful when the Internet connection to the Bibledit-Web server is not fast or not stable.") ?>
</p>
<p>
  <?php echo Locale_Translate::_("Install the Client as you would install Bibledit-Web.") ?>
  <?php echo Locale_Translate::_("During setup you can switch it to client mode.") ?>
  <?php echo Locale_Translate::_("In client mode, connect to the server once, and provide your username and password as you normally log in on the server.") ?>
  <?php echo Locale_Translate::_("Then synchronize the data.") ?>
  <?php echo Locale_Translate::_("The first time all Bible data and Notes will be downloaded from the server to the client.") ?>
  <?php echo Locale_Translate::_("That may take a while.") ?>
  <?php echo Locale_Translate::_("After that, synchronizing will send and receive the changes only.") ?>
</p>
<p>
  <?php echo Locale_Translate::_("The client mode is useful in a situation where the Internect connection is not optimal.") ?>
  <?php echo Locale_Translate::_("All data is stored locally on your computing device.") ?>
  <?php echo Locale_Translate::_("During translation, the Internet connection is not used.") ?>
  <?php echo Locale_Translate::_("It is used during synchronization.") ?>
</p>

<h2><?php echo Locale_Translate::_("Email") ?></h2>
<p>
  <?php echo Locale_Translate::_("Bibledit-Web works with email.") ?>
  <?php echo Locale_Translate::_("After completing the installation of Bibledit-Web, sending and receiving emails does not yet work.") ?>
  <?php echo Locale_Translate::_("This needs to be set up.") ?>
  <?php echo Locale_Translate::_("Setting up the email means to pick an email address and setting details for the mail servers in Bibledit-Web.") ?>
  <?php echo Locale_Translate::_("This can be done from within the web application.") ?>
</p>

<h2><?php echo Locale_Translate::_("Multiple translation teams") ?></h2>
<p><?php echo Locale_Translate::_("There are two ways to have multiple translation teams on one server. ") ?></p>
<ol>
<li>
  <?php echo Locale_Translate::_("Install multiple parallel copies of Bibledit-Web on one server.") ?>
  <?php echo Locale_Translate::_("Unpack the installation zip archive in sub folders with different names.") ?>
  <?php echo Locale_Translate::_("Each copy has its own web address.") ?>
  <?php echo Locale_Translate::_("Everything is completely separated between the parallel copies.") ?>
  <?php echo Locale_Translate::_("This includes the data and the databases.") ?>
  <?php echo Locale_Translate::_("Every copy has its own independent databases.") ?>
</li>
<li>
  <?php echo Locale_Translate::_("Use one installation of Bibledit-Web.") ?>
  <?php echo Locale_Translate::_("Give various users read or write access to various Bibles.") ?>
  <?php echo Locale_Translate::_("Users with write access together constitute a translation team that can edit the Bible.") ?>
</li>
</ol>

<h2><?php echo Locale_Translate::_("Install development version") ?></h2>
<p>
  <?php echo Locale_Translate::_("Read the instructions for a standard Bibledit-Web installation to find out about server setup and prerequisites.") ?>
</p>
<br>
<p>
  <?php echo Locale_Translate::_("The Bibledit-Web code is kept in a git repository.") ?>
  <a href="https://savannah.nongnu.org/projects/bibledit-web" target="_blank"><?php echo Locale_Translate::_("The repository is accessible from the Savannah Project Page.") ?></a>
  <?php echo Locale_Translate::_("To view the code on the web, click Source Code - Browse Sources Repository.") ?>
  <?php echo Locale_Translate::_("To use the repository in another way, click on Source Code - Use Git.") ?>
  <?php echo Locale_Translate::_("Information will be provided there.") ?>
  <?php echo Locale_Translate::_("The code can be checked out from the repository.") ?>
  <?php echo Locale_Translate::_("This gives the most recent version of Bibledit-Web.") ?>
  <?php echo Locale_Translate::_("The code is supported.") ?>
</p>
<br>
<p>
  <?php echo Locale_Translate::_("To get the most recent code, run the following in a terminal:") ?>
</p>
<p><code>$ git clone git://git.savannah.nongnu.org/bibledit-web</code></p>
<p>
  <?php echo Locale_Translate::_("This clones the code repository to your computer.") ?>
  <?php echo Locale_Translate::_("It will create directory") ?>
  <code>bibledit-web</code>.
  <?php echo Locale_Translate::_("Change into that directory.") ?>
  <?php echo Locale_Translate::_("Then create an installation package:") ?>
</p>
<p><code>$ ./scripts/package</code></p>
<p>
  <?php echo Locale_Translate::_("It will display where the package will be stored.") ?>
  <?php echo Locale_Translate::_("This package can be used to do a standard installation of Bibledit-Web.") ?>
</p>

<h2><?php echo Locale_Translate::_("Open installation") ?></h2>
<p>
  <?php echo Locale_Translate::_("You can configure Bibledit-Web to be open.") ?>
  <?php echo Locale_Translate::_("It means that there is no need to login.") ?>
  <?php echo Locale_Translate::_("The website appears to be logged in as user 'admin' to any visitor.") ?>
  <?php echo Locale_Translate::_("This is useful for demo installations, or to provide a free service to the the community.") ?>
  <?php echo Locale_Translate::_("A disadvantage is that the crawlers of the search engines keep clicking links, and will disturb data and settings.") ?>
  <?php echo Locale_Translate::_('Open the following file:') ?>
  <code>config/open.php</code>.
  <?php echo Locale_Translate::_('Update one line as follows:') ?>
  <code>$open_installation = true;</code>
</p>

<h2><?php echo Locale_Translate::_("Hosting providers") ?></h2>
<p>
  <?php echo Locale_Translate::_("Bibledit-Web is not supported on a shared Linux hosting account.") ?>
  <?php echo Locale_Translate::_("On several occassions Bibledit-Web has been used on such an account though.") ?>
  <?php echo Locale_Translate::_("Bibledit-Web is a heavy application.") ?>
  <?php echo Locale_Translate::_("It uses a lot of resources compared to what is available on a shared hosting account.") ?>
  <?php echo Locale_Translate::_("The hosting providers have responded in various ways to having Bibledit-Web on their shared platform:") ?>
</p>
<p>
  * <?php echo Locale_Translate::_("Closing the website saying it causes inconvenience on the shared platform.") ?>
</p>
<p>
  * <?php echo Locale_Translate::_("Silently modifying the scheduled tasks to run less often, crippling Bibledit-Web operations.") ?>
</p>
<p>
  * <?php echo Locale_Translate::_("Suggesting it to be moved to a virtual private server.") ?>
</p>
<p>
  * <?php echo Locale_Translate::_("Slowing down the network connection.") ?>
</p>
<p>
  * <?php echo Locale_Translate::_("Slowing down the shell server.") ?>
</p>
<p>
  <?php echo Locale_Translate::_("It has also been observed that the Bibledit databases regularly got corrupted due to insufficient file locking on the shared platform.") ?>
</p>
<p>
  <?php echo Locale_Translate::_("To make good use of Bibledit-Web, a VPS (Virtual Private Server) is recommended.") ?>
  <?php echo Locale_Translate::_("Good providers are:") ?>
</p>
<p>
  * <a href="http://ramnode.com/vps.php">RamNode</a>, Premium OpenVZ SSD 128MB for $15 per year (2014).
</p>
<p>
  * <a href="https://www.digitalocean.com/?refcode=5189a8e56d7a">Digital Ocean</a>, 512 Mb / 1 CPU / 20 Gb SSD Disk for $5 per month = $60 per year (2014).
</p>
