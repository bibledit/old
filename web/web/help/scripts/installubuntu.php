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
<h1><?php echo gettext ("Install Bibledit-Web 1.0 on Ubuntu 13.04") ?></h1>
<p><?php echo gettext ("This assumes a standard installation of the operating system.") ?></p>
<p><?php echo gettext ("Connect to or open a terminal.") ?></p>
<p><?php echo gettext ("Update the software sources. Type the following command, and then press Enter.") ?></p>
<p><code>sudo apt-get update</code></p>

<h2><?php echo gettext ("Install the web server") ?></h2>
<p><?php echo gettext ("In the terminal issue these commands:") ?></p>
<p><code>sudo apt-get install tasksel</code></p>
<p><code>sudo tasksel</code></p>
<p><?php echo gettext ("Tick the LAMP server, and press Enter. Installation takes a while.") ?></p>
<p><?php echo gettext ("While installing mysql-server, set the administrative password. This password is needed later.") ?></p>
<p><?php echo gettext ("Test the web server. Open the site in the browser. The web address of the site depends on your situation. Examples:") ?></p>
<p><?php echo gettext ("http://localhost") ?></p>
<p><?php echo gettext ("http://website.org") ?></p>
<p>When your web server works, the browser displays: It works!</p>

<h2><?php echo gettext ("Install the software Bibledit-Web relies on") ?></h2>
<p><?php echo gettext ("In the terminal issue these commands:") ?></p>
<p><code>sudo apt-get install make git curl zip unzip libzend-framework-php wdiff sqlite3 php5-intl python perl libsword-utils</code></p>
<p><?php echo gettext ("Anything to be set up, choose the default settings and enter the correct passwords.") ?></p>

<h2><?php echo gettext ("Install Bibledit-Web") ?></h2>
<p><?php echo gettext ("Download Bibledit-Web: Type the following command, and press Enter:") ?></p>
<code>wget http://download.savannah.nongnu.org/releases/bibledit/source/web/bibledit-web-1.0.tar.gz</code>
<p><?php echo gettext ("Unpack Bibledit-Web: Type the following command, and press Enter:") ?></p>
<code>tar zxf bibledit-web-1.0.tar.gz</code>
<p><?php echo gettext ("Enter Bibledit-Web's code: Type the following command, and press Enter:") ?></p>
<code>cd bibledit-web-1.0</code>
<p><?php echo gettext ("Configure Bibledit-Web: Type the following command, replace 'password' with the administrative password of the mysql-server as entered above, and press Enter:") ?></p>
<code>./configure --with-mysql-root-password=password</code>
<p><?php echo gettext ("Install Bibledit-Web: Type the following command, and press Enter:") ?></p>
<code>sudo make install</code>

<h2><?php echo gettext ("Access Bibledit-Web") ?></h2>
<p><?php echo gettext ("Bibledit-Web should now be ready, and accessible through:") ?> "http://website.org/bibledit-web".</p>
<p><?php echo gettext ("You can initially login through default username 'adminusername' and password 'adminpassword'. These only work if no Administrator's account exist on the system. It is recommended to create an Administrator's account of your own choice. This automatically removes the default adminusername account.") ?></p>
