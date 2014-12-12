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
<h1><?php echo gettext("Install Bibledit 1.8 on Mac OS X") ?></h1>
<p>
  <?php echo gettext("This assumes a standard installation of the operating system.") ?>
  <?php echo gettext("Bibledit runs on all recent versions of Mac OS X.") ?>
  <?php echo gettext("The steps below have been tested on Mac OS X 10.7 Lion.") ?>
  <?php echo gettext("Open a terminal to enter some of the commands:") ?> Applications / Utilities / Terminal.
</p>

<ol>

<li>
<p><a href="http://guide.macports.org" target="_blank"><?php echo gettext("Install Xcode.") ?></a></p>
</li>

<li>
<p><a href="http://www.macports.org" target="_blank"><?php echo gettext("Install MacPorts.") ?></a></p>
</li>

<li>
<p><?php echo gettext("Install the Apache web server:") ?></p>
<p><code>sudo port install apache2</code></p>
</li>

<li>
<p><?php echo gettext("Activate Apache:") ?></p>
<p><code>sudo port load apache2</code></p>
</li>

<li>
<p>
  <?php echo gettext("To verify Apache is running, point your browser to:") ?>
  <a href="http://localhost/" target="_blank">​http://localhost</a>.
  <?php echo gettext("The page should say:") ?>
  It works!
</p>
</li>

<li>
<p><?php echo gettext("Install PHP plus modules:") ?></p>
<p><code>sudo port install php5 +apache2</code></p>
<p><code>sudo port install php5-mbstring</code></p>
<p><code>sudo port install php5-posix</code></p>
<p><code>sudo port install php5-sqlite</code></p>
<p><code>sudo port install php5-iconv</code></p>
</li>

<li>
<p><?php echo gettext("Register PHP with Apache:") ?></p>
<p><code>cd /opt/local/apache2/modules</code></p>
<p><code>sudo /opt/local/apache2/bin/apxs -a -e -n "php5" libphp5.so</code></p>
</li>

<li>
<p><?php echo gettext("Update the Apache configuration:") ?></p>
<p><code>sudo nano /opt/local/apache2/conf/httpd.conf</code></p>
<p><?php echo gettext("In that file, search for:") ?></p>
<p><code>DirectoryIndex index.html</code></p>
<p><?php echo gettext("and change it to:") ?></p>
<p><code>DirectoryIndex index.php index.html</code></p>
<p><?php echo gettext("Add the following line at the end of that file:") ?></p>
<p><code>Include conf/extra/mod_php.conf</code></p>
<p><?php echo gettext("Save the file.") ?></p>
</li>

<li>
<p><?php echo gettext("Configure PHP:") ?></p>
<p><code>cd /opt/local/etc/php5</code></p>
<p><code>sudo cp php.ini-development php.ini</code></p>
</li>

<li>
<p><?php echo gettext("Restart Apache for the new configuration to take effect:") ?></p>
<p><code>sudo /opt/local/apache2/bin/apachectl restart</code></p>
</li>

<li>
<p>
  <a href="http://download.savannah.nongnu.org/releases/bibledit-web/source/bibledit-web-1.8.zip" target="_blank"><?php echo gettext("Download") ?> Bibledit 1.8</a>.
</p>
</li>

<li>
<p><?php echo gettext("Unzip Bibledit into this folder:") ?> <code>/opt/local/apache2/htdocs</code></p>
</li>

<li>
<p><?php echo gettext("Set correct permissions on folder bibledit-web for the web server:") ?></p>
<p><code>sudo chown -R root /opt/local/apache2/htdocs/bibledit-web</code></p>
<p><code>sudo chmod -R 0777 /opt/local/apache2/htdocs/bibledit-web</code></p>
</li>

<li>
<p>
  <?php echo gettext("To configure Bibledit, point your browser to:") ?>
  <a href="http://localhost/bibledit-web" target="_blank">​http://localhost/bibledit-web</a>.
  <?php echo gettext("Follow all the steps in the browser.") ?>
</p>
</li>

<li>
<p>
  <?php echo gettext("To use Bibledit, point your browser to the same address:") ?>
  <a href="http://localhost/bibledit-web" target="_blank">​http://localhost/bibledit-web</a>.
</p>
</li>

<li>
<p><?php echo gettext("Once everything works, put PHP in production mode:") ?></p>
<p><code>cd /opt/local/etc/php5</code></p>
<p><code>sudo cp php.ini-production php.ini</code></p>
</li>

</ol>
