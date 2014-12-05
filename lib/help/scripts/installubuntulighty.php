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
<h2><?php echo gettext("Install Bibledit-Web 1.8 on Ubuntu 14.04 through Lighty") ?></h2>
<p>
  <?php echo gettext("Follow the steps below to install Bibledit-Web.") ?>
  <?php echo gettext("It needs some basic knowledge of working with Linux servers.") ?>
</p>
<ol>
<li><?php echo gettext("Open a terminal.") ?></li>
<li>
  <p><?php echo gettext("Update the software sources:") ?></p>
  <p><code>$ sudo apt-get update</code></p>
</li>
<li>
  <p><?php echo gettext("Install the web server:") ?></p>
  <p><code>$ sudo apt-get install lighttpd</code></p>
</li>
<li>
  <?php echo gettext("Test the web server by opening the site in the browser, for example:") ?>
  http://localhost
  <?php echo gettext("or") ?>
  http://website.org.
  <?php echo gettext("When the web server works, the browser displays the Placeholder page.") ?>
</li>
<li>
  <p><?php echo gettext("Install and enable PHP:") ?></p>
  <p><code>$ sudo apt-get install php5-cgi php5-cli</code></p>
  <p><code>$ sudo lighttpd-enable-mod fastcgi fastcgi-php</code></p>
</li>
<li>
  <p><?php echo gettext("Install the software Bibledit-Web relies on:") ?></p>
  <p><code>$ sudo apt-get install git unzip php5-json php5-sqlite</code></p>
</li>
<li>
  <p><?php echo gettext("Restart the web server so the new modules take effect:") ?></p>
  <p><code>$ sudo service lighttpd restart</code></p>
</li>
<li>
  <p><?php echo gettext("Download Bibledit-Web:") ?></p>
  <p><code>$ sudo wget http://download.savannah.nongnu.org/releases/bibledit-web/source/bibledit-web-1.8.zip</code></p>
</li>
<li>
  <p><?php echo gettext("Install Bibledit-Web in folder /var/www as user root:") ?></p>
  <p><code>$ cd /var/www</code></p>
  <p><code>$ sudo unzip ~/bibledit-web-1.8.zip</code></p>
</li>
<li>
  <p><?php echo gettext("Make everything to be owned by the web server user:") ?></p>
  <p><code>$ sudo chown -R www-data:www-data bibledit-web</code></p>
</li>
<li>
  <p><?php echo gettext("Configure access control:") ?></p>
  <p><code>$ sudo cp /var/www/bibledit-web/config/lighttpd.conf /etc/lighttpd/conf-enabled</code></p>
</li>
<li>
  <p><?php echo gettext("Reload the web server so the new configuration takes effect:") ?></p>
  <p><code>$ sudo service lighttpd restart</code></p>
</li>
<li><?php echo gettext("Bibledit-Web will be accessible through:") ?> http://website.org/bibledit-web.</li>
<li><?php echo gettext("Open the web address in the browser, and follow the steps on the screen to configure Bibledit-Web and log in.") ?></li>
</ol>
