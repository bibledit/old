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
<h2><?php echo gettext("Install Bibledit 1.8 on CentOS 6.4") ?></h2>
<p>
  <?php echo gettext("Follow the steps below to install Bibledit.") ?>
  <?php echo gettext("It needs some basic knowledge of working with Linux servers.") ?>
</p>
<ol>
<li><?php echo gettext("Open a terminal.") ?></li>
<li><a href="http://www.howtoforge.com/installing-apache2-with-php5-and-mysql-support-on-centos-6.4-lamp" target="_blank"><?php echo gettext("Install Apache, PHP5 and MySQL.") ?></a></li>
<li>Set up the firewall to enable incoming web traffic.</li>
<li>
  <p><?php echo gettext("In the terminal issue the following commands:") ?></p>
  <p><code>$ yum install git unzip rcs php-mysql php-mbstring php-xml php-posix php-sqlite</code></p>
  <p><code>$ service httpd restart</code></p>
</li>
<li><?php echo gettext("Select the default settings for anything to be set up.") ?></li>
<li>
  <p><?php echo gettext("Download Bibledit:") ?></p>
  <p><code>$ wget http://download.savannah.nongnu.org/releases/bibledit-web/source/bibledit-web-1.8.zip</code></p>
</li>
<li>
  <p><?php echo gettext("Install Bibledit in folder /var/www/html as user root:") ?></p>
  <p><code>$ cd /var/www/html</code></p>
  <p><code>$ unzip ~/bibledit-web-1.8.zip</code></p>
</li>
<li>
  <p><?php echo gettext("Make everything readable and writable to the web server user through the following commands") ?></p>
  <p><code>$ chown -R apache:apache bibledit-web</code></p>
</li>
<li><?php echo gettext("Bibledit will be accessible through:") ?> http://website.org/bibledit-web.</li>
<li><?php echo gettext("Open the web address in the browser, and follow the steps on the screen to configure Bibledit and log in.") ?></li>
<li>
  <a href="http://stackoverflow.com/questions/8551740/centos-htaccess-not-being-read" target="_blank"><?php echo gettext("Enable the use of .htaccess files in Apache to secure some Bibledit folders.") ?></a>
  <?php echo gettext("Set") ?>
  <code>AllowOverride All</code>
  <?php echo gettext("in file") ?>
  <code>httpd.conf</code>.
</li>
</ol>
