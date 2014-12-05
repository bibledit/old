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
<h1><?php echo gettext("Installation") ?></h1>

<h2><?php echo gettext("Standard installation") ?></h2>
<p><a href="installwindows.php"><?php echo gettext("Bibledit-Web on Windows") ?></a></p>
<p><a href="installandroid.php"><?php echo gettext("Bibledit-Web on Android") ?></a></p>
<p><a href="installmacosx.php"><?php echo gettext("Bibledit-Web on Mac OS X") ?></a></p>
<p><a href="installios.php"><?php echo gettext("Bibledit-Web on iOS: iPad and iPhone") ?></a></p>
<p><a href="installnewubuntu.php"><?php echo gettext("Bibledit-Web on Ubuntu through Apache") ?></a></p>
<p><a href="installubuntulighty.php"><?php echo gettext("Bibledit-Web on Ubuntu through Lighty") ?></a></p>
<p><a href="installcentos.php"><?php echo gettext("Bibledit-Web on CentOS") ?></a></p>
<p><a href="installpclinuxos.php"><?php echo gettext("Bibledit-Web on PCLinuxOS") ?></a></p>

<h2><?php echo gettext("Upgrading") ?></h2>
<p>
  <?php echo gettext("A certain release of Bibledit-Web can upgrade the most recent previous release, but not older releases.") ?>
  <?php echo gettext("A new release can be installed over the previous release.") ?>
  <?php echo gettext("There should be no loss of information.") ?>
  <?php echo gettext("Create a backup of the web space before upgrading, just to be sure.") ?>
</p>

<h2><?php echo gettext("Client") ?></h2>
<p>
  <?php echo gettext("Bibledit-Web normally runs in Server mode.") ?>
  <?php echo gettext("It can also be set to Client mode.") ?>
  <?php echo gettext("A Client stores the data offline.") ?>
  <?php echo gettext("You can translate the Bible on a Client.") ?>
  <?php echo gettext("The user needs to synchronize the data on the client with the data on the server regularly.") ?>
  <?php echo gettext("A Client is useful when the Internet connection to the Bibledit-Web server is not fast or not stable.") ?>
</p>
<p>
  <?php echo gettext("Install the Client as you would install Bibledit-Web.") ?>
  <?php echo gettext("During setup you can switch it to client mode.") ?>
  <?php echo gettext("In client mode, connect to the server once, and provide your username and password as you normally log in on the server.") ?>
  <?php echo gettext("Then synchronize the data.") ?>
  <?php echo gettext("The first time all Bible data and Notes will be downloaded from the server to the client.") ?>
  <?php echo gettext("That may take a while.") ?>
  <?php echo gettext("After that, synchronizing will send and receive the changes only.") ?>
</p>
<p>
  <?php echo gettext("The client mode is useful in a situation where the Internect connection is not optimal.") ?>
  <?php echo gettext("All data is stored locally on your computing device.") ?>
  <?php echo gettext("During translation, the Internet connection is not used.") ?>
  <?php echo gettext("It is used during synchronization.") ?>
</p>

<h2><?php echo gettext("Email") ?></h2>
<p>
  <?php echo gettext("Bibledit-Web works with email.") ?>
  <?php echo gettext("After completing the installation of Bibledit-Web, sending and receiving emails does not yet work.") ?>
  <?php echo gettext("This needs to be set up.") ?>
  <?php echo gettext("Setting up the email means to pick an email address and setting details for the mail servers in Bibledit-Web.") ?>
  <?php echo gettext("This can be done from within the web application.") ?>
</p>

<h2><?php echo gettext("Multiple translation teams") ?></h2>
<p><?php echo gettext("There are two ways to have multiple translation teams on one server. ") ?></p>
<ol>
<li>
  <?php echo gettext("Install multiple parallel copies of Bibledit-Web on one server.") ?>
  <?php echo gettext("Unpack the installation zip archive in sub folders with different names.") ?>
  <?php echo gettext("Each copy has its own web address.") ?>
  <?php echo gettext("Everything is completely separated between the parallel copies.") ?>
  <?php echo gettext("This includes the data and the databases.") ?>
  <?php echo gettext("Every copy has its own independent databases.") ?>
</li>
<li>
  <?php echo gettext("Use one installation of Bibledit-Web.") ?>
  <?php echo gettext("Give various users read or write access to various Bibles.") ?>
  <?php echo gettext("Users with write access together constitute a translation team that can edit the Bible.") ?>
</li>
</ol>

<h2><?php echo gettext("Install development version") ?></h2>
<p>
  <?php echo gettext("Read the instructions for a standard Bibledit-Web installation to find out about server setup and prerequisites.") ?>
</p>
<br>
<p>
  <?php echo gettext("The Bibledit-Web code is kept in a git repository.") ?>
  <a href="https://savannah.nongnu.org/projects/bibledit-web" target="_blank"><?php echo gettext("The repository is accessible from the Savannah Project Page.") ?></a>
  <?php echo gettext("To view the code on the web, click Source Code - Browse Sources Repository.") ?>
  <?php echo gettext("To use the repository in another way, click on Source Code - Use Git.") ?>
  <?php echo gettext("Information will be provided there.") ?>
  <?php echo gettext("The code can be checked out from the repository.") ?>
  <?php echo gettext("This gives the most recent version of Bibledit-Web.") ?>
  <?php echo gettext("The code is supported.") ?>
</p>
<br>
<p>
  <?php echo gettext("To get the most recent code, run the following in a terminal:") ?>
</p>
<p><code>$ git clone git://git.savannah.nongnu.org/bibledit-web</code></p>
<p>
  <?php echo gettext("This clones the code repository to your computer.") ?>
  <?php echo gettext("It will create directory") ?>
  <code>bibledit-web</code>.
  <?php echo gettext("Change into that directory.") ?>
  <?php echo gettext("Then create an installation package:") ?>
</p>
<p><code>$ ./scripts/package</code></p>
<p>
  <?php echo gettext("It will display where the package will be stored.") ?>
  <?php echo gettext("This package can be used to do a standard installation of Bibledit-Web.") ?>
</p>

<h2><?php echo gettext("Open installation") ?></h2>
<p>
  <?php echo gettext("You can configure Bibledit-Web to be open.") ?>
  <?php echo gettext("It means that there is no need to login.") ?>
  <?php echo gettext("The website appears to be logged in as user 'admin' to any visitor.") ?>
  <?php echo gettext("This is useful for demo installations, or to provide a free service to the the community.") ?>
  <?php echo gettext("A disadvantage is that the crawlers of the search engines keep clicking links, and will disturb data and settings.") ?>
  <?php echo gettext('Open the following file:') ?>
  <code>config/open.php</code>.
  <?php echo gettext('Update one line as follows:') ?>
  <code>$open_installation = true;</code>
</p>

<h2><?php echo gettext("Hosting providers") ?></h2>
<p>
  <?php echo gettext("Bibledit-Web is not supported on a shared Linux hosting account.") ?>
  <?php echo gettext("On several occassions Bibledit-Web has been used on such an account though.") ?>
  <?php echo gettext("Bibledit-Web is a heavy application.") ?>
  <?php echo gettext("It uses a lot of resources compared to what is available on a shared hosting account.") ?>
  <?php echo gettext("The hosting providers have responded in various ways to having Bibledit-Web on their shared platform:") ?>
</p>
<p>
  * <?php echo gettext("Closing the website saying it causes inconvenience on the shared platform.") ?>
</p>
<p>
  * <?php echo gettext("Silently modifying the scheduled tasks to run less often, crippling Bibledit-Web operations.") ?>
</p>
<p>
  * <?php echo gettext("Suggesting it to be moved to a virtual private server.") ?>
</p>
<p>
  * <?php echo gettext("Slowing down the network connection.") ?>
</p>
<p>
  * <?php echo gettext("Slowing down the shell server.") ?>
</p>
<p>
  <?php echo gettext("It has also been observed that the Bibledit databases regularly got corrupted due to insufficient file locking on the shared platform.") ?>
</p>
<p>
  <?php echo gettext("To make good use of Bibledit-Web, a VPS (Virtual Private Server) is recommended.") ?>
  <?php echo gettext("Good providers are:") ?>
</p>
<p>
  * <a href="http://ramnode.com/vps.php">RamNode</a>, Premium OpenVZ SSD 128MB for $15 per year (2014).
</p>
<p>
  * <a href="https://www.digitalocean.com/?refcode=5189a8e56d7a">Digital Ocean</a>, 512 Mb / 1 CPU / 20 Gb SSD Disk for $5 per month = $60 per year (2014).
</p>
