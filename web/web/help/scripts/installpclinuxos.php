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
<h2><?php echo Locale_Translate::_("Install Bibledit-Web 1.9 on PCLinuxOS LXDE 2014") ?></h2>
<p>
  <?php echo Locale_Translate::_("Follow the steps below to install Bibledit-Web.") ?>
</p>
<ol>
<li><?php echo Locale_Translate::_("Open the Synaptic Package Manager.") ?></li>
<li>
  <p><?php echo Locale_Translate::_("Install the following packages:") ?></p>
  <p><code>task-lamp</code></p>
  <p><code>php-iconv</code></p>
</li>
<li>
  <?php echo Locale_Translate::_("To activate the web server, open Configure your Computer, click Sharing, click Configure web server.") ?>
  <?php echo Locale_Translate::_("Follow the steps in the wizard, taking the default settings, and clicking Next, till it finishes.") ?>
</li>
<li>
  <?php echo Locale_Translate::_("Test the web server by opening the site in the browser:") ?>
  http://localhost.
  <?php echo Locale_Translate::_("The browser should say that the server is alive and working.") ?>
</li>
<li>
  <?php echo Locale_Translate::_("Open a Terminal.") ?>
</li>
<li>
  <?php echo Locale_Translate::_("Become the administrative user:") ?>
  <p><code>$ su</code></p>
  <p><?php echo Locale_Translate::_("Provide the administrative password.") ?></p>
</li>
<li>
  <p><?php echo Locale_Translate::_("Download and install Bibledit-Web in folder /var/www/html:") ?></p>
  <p><code>$ cd /var/www/html</code></p>
  <p><code>$ wget http://download.savannah.nongnu.org/releases/bibledit-web/source/bibledit-web-1.9.zip</code></p>
  <p><code>$ unzip bibledit-web-1.9.zip</code></p>
  <p><code>$ chown -R apache:apache bibledit-web</code></p>
</li>
<li><?php echo Locale_Translate::_("Bibledit-Web will be accessible through:") ?> http://localhost/bibledit-web.</li>
<li><?php echo Locale_Translate::_("Open the web address in the browser, and follow the steps on the screen to configure Bibledit-Web and log in.") ?></li>
<li><?php echo Locale_Translate::_("Reboot.") ?></li>
</ol>
