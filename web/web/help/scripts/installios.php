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
<h1><?php echo Locale_Translate::_("Install Bibledit-Web 1.9 on iOS 7: iPad and iPhone") ?></h1>
<p>
  <?php echo Locale_Translate::_("This assumes a standard iPad or iPhone running iOS 7.") ?>
  <?php echo Locale_Translate::_("The steps below have been tested on an iPad mini running iOS 7.1.2.") ?>
</p>

<ol>

<li>
<p>
  <?php echo Locale_Translate::_("Do an untethered jailbreak of iOS 7.1.x on the device with Pangu or another tool.") ?>
  <?php echo Locale_Translate::_("A number of guides on the Internet provide assistance.") ?>
</p>
</li>

<li>
<p>
  <?php echo Locale_Translate::_("If the jailbreaking did not install Cydia:") ?>
  <a href="http://www.cydiaios7.com" target="_blank"><?php echo Locale_Translate::_("Install Cydia iOS 7 on the device.") ?></a>
  <?php echo Locale_Translate::_("Open Cydia for the first time and let it install the software and reboot the device.") ?>
</p>
</li>

<li>
<p><?php echo Locale_Translate::_("Open Cydia, go to Sources, and add the following APT URL:") ?></p>
<p><code>http://ios-webstack.tk/cydia</code></p>
</li>

<li>
<p><?php echo Locale_Translate::_("In Cydia, search for and install the following package:") ?></p>
<p><code>ios-lighttpd-php-mysql</code></p>
</li>

<li>
<p>
  <?php echo Locale_Translate::_("Open Safari on the device.") ?>
  <?php echo Locale_Translate::_("Browse to the following address:") ?>
</p>
<p><a href="http://localhost" target="_blank"><code>localhost</code></a></p>
<p>
  <?php echo Locale_Translate::_("It should display a page saying that it works.") ?>
</p>
</li>

<li>
<p>
  <?php echo Locale_Translate::_("In the iOS Settings, there is a new entry:") ?>
  "Lighttpd MySQL".
  <?php echo Locale_Translate::_("Tap it, look for the MySQL Server, and disable it.") ?>
  <?php echo Locale_Translate::_("This saves system resources.") ?>
</p>
</li>

<li>
<p><?php echo Locale_Translate::_("In Cydia, search for and install the following packages:") ?></p>
<p><code>MobileTerminal</code></p>
<p><code>wget</code></p>
<p><code>unzip</code></p>
</li>

<li>
<p><?php echo Locale_Translate::_('Open the "Terminal" app.') ?></p>
</li>

<li>
<p>
  <?php echo Locale_Translate::_("Become root to escalate your privileges.") ?>
  <?php echo Locale_Translate::_("In the terminal, type the command below, and press return:") ?>
</p>
<p><code>su root</code></p>
<p>
  <?php echo Locale_Translate::_("It asks for the password.") ?>
  <?php echo Locale_Translate::_("In the terminal, type the password below, and press return:") ?>
</p>
<p><code>alpine</code></p>
<p>
  <?php echo Locale_Translate::_("The password is invisible in the terminal.") ?>
</p>
</li>

<li>
<p>
  <?php echo Locale_Translate::_("Change to the home directory.") ?>
  <?php echo Locale_Translate::_("In the terminal, type the command below, and press return:") ?>
</p>
<p><code>cd</code></p>
</li>

<li>
<p>
  <?php echo Locale_Translate::_("Download the Bibledit software.") ?>
  <?php echo Locale_Translate::_("In the terminal, type the command below, and press return:") ?>
</p>
<p><code>wget --no-check-certificate https://github.com/teusbenschop/bibledit-ios/archive/master.zip</code></p>
<p>
  <?php echo Locale_Translate::_("The download takes some time.") ?>
</p>
</li>

<li>
<p>
  <?php echo Locale_Translate::_("Unpack the downloaded software.") ?>
  <?php echo Locale_Translate::_("In the terminal, type the command below, and press return:") ?>
</p>
<p><code>unzip master</code></p>
<p>
  <?php echo Locale_Translate::_("It will inflate a lot of files.") ?>
</p>
</li>

<li>
<p>
  <?php echo Locale_Translate::_("Run the Bibledit installation script.") ?>
  <?php echo Locale_Translate::_("In the terminal, type the command below, and press return:") ?>
</p>
<p><code>./bibledit-ios-master/install</code></p>
<p>
  <?php echo Locale_Translate::_("It will output messages but no errors should occur.") ?>
</p>
</li>

<li>
<p>
  <?php echo Locale_Translate::_("Open Safari on the device.") ?>
  <?php echo Locale_Translate::_("Browse to the following address:") ?>
</p>
<p><a href="http://localhost/bibledit-web" target="_blank"><code>localhost/bibledit-web</code></a></p>
<p>
  <?php echo Locale_Translate::_("It should display Bibledit-Web.") ?>
  <?php echo Locale_Translate::_("Follow the steps in Safari to complete the installation and open Bibledit.") ?>
</p>
</li>

</ol>

<p>
  <?php echo Locale_Translate::_("Note:") ?>
  <?php echo Locale_Translate::_("Work on a version of Bibledit on iOS that does not require jailbreaking the device is in progress.") ?>
</p>
