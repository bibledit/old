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
<h1><?php echo Locale_Translate::_("Web collaboration") ?></h1>
<p><?php echo Locale_Translate::_("With Bibledit-Web several translators can work on one Bible at the same time.") ?></p>
<p><?php echo Locale_Translate::_("There is nothing special needed to make this happen.") ?></p>
<p><?php echo Locale_Translate::_("The people log into the site that runs Bibledit-Web, and translate there.") ?></p>
<p><?php echo Locale_Translate::_("All the data related to the Bible is saved on that website, and is available to the relevant people.") ?></p>
<p><?php echo Locale_Translate::_("When collaborating in this way, the information given below on the rest of the page is not relevant.") ?></p>

<h1><?php echo Locale_Translate::_("Distributed collaboration") ?></h1>
<p><?php echo Locale_Translate::_("With distributed collaboration there are several copies of the Bible distributed all over the place or the globe.") ?></p>
<p><?php echo Locale_Translate::_("Translators and other people that collaborate in this way do not need to be connected to the Internet all the time, or even not at all.") ?></p>
<p><?php echo Locale_Translate::_("They collaborate through the network or a USB drive.") ?></p>
<p><?php echo Locale_Translate::_("Once everything has been set up, it all goes automatic. The person working on the Bible does not notice the mechanics of the collaboration. It all happens under the hood.") ?></p>
<p><?php echo Locale_Translate::_("There are a variety of methods for collaboration. The administrator who will set it all up needs to have sufficient knowledge of information technology.") ?></p>
<p><?php echo Locale_Translate::_("Bibledit-Web stores all data locally. When collaboration is enabled, it also connects to a central repository. Through that repository the data gets shared between the various translators and other users.") ?></p>
<p><?php echo Locale_Translate::_("Under the hood, Bibledit uses Git for collaboration. Git is a content manager. The user does not notice that Git is used. He does not operate it directly. The only thing that the manager needs to do at regular times is to 'Send/Receive' the Bible. After having done that, the changes in the Bible, made by the other translators or users, appear on his screen too. And the changes made by himself get stored in the repository, and if the other users do Send/Receive also, then these changes will reflect on their screens also.") ?></p>
<p><?php echo Locale_Translate::_("If distributed collaboration is used, it needs to be set up per Bible.") ?> <a href="collaboration.php?object=<?php echo $this->object ?>&select"><?php echo Locale_Translate::_ ("Select which Bible to use.") ?></a></p>
<?php if ($this->object != "") { ?>
  <p><?php echo Locale_Translate::_("Setup for Bible") ?>: <?php echo $this->object ?></p>
  <?php if ($this->url != "") { ?>
  <p><?php echo Locale_Translate::_("Distributed collaboration for this is enabled.") ?> <a href="collaboration.php?object=<?php echo $this->object ?>&disable="><?php echo Locale_Translate::_ ("Disable it.") ?></a></p>
  <?php } ?>
  <p><?php echo Locale_Translate::_("There is a variety of repositories that can be used. They have their own use, advantages and disadvantages.") ?></p>
  <ul>
    <li>
      <em><?php echo Locale_Translate::_("USB Flash Drive.") ?></em>
      <?php echo Locale_Translate::_("First of all there is the repository that resides on a file system. A USB flash drive can be used in this case. This flash drive can be physically moved from one translator to the other, and so on, updating each translator, and getting updated by each translator.") ?>
      <a href="collaboration_flash_drive_setup.php?object=<?php echo $this->object ?>"><?php echo Locale_Translate::_("Set it up.") ?></a>
    </li>
    <li>
      <em><?php echo Locale_Translate::_("Open Network Repository.") ?></em>
      <?php echo Locale_Translate::_("Secondly there is the repository that resides on the network. For internal networks a simple open network repository can be used. Whoever knows where the repository resides can write to it.") ?>
      <a href="collaboration_open_network_setup.php?object=<?php echo $this->object ?>"><?php echo Locale_Translate::_("Set it up.") ?></a>
    </li>
    <li>
      <em><?php echo Locale_Translate::_("Password Protected Network Repository.") ?></em>
      <?php echo Locale_Translate::_("For better security is is helpful to use a repository that needs a username and password for write access.") ?>
      <a href="collaboration_password_protected_network_setup.php?object=<?php echo $this->object ?>"><?php echo Locale_Translate::_("Set it up.") ?></a>
    </li>
    <li>
      <em><?php echo Locale_Translate::_("Secured Network Repository.") ?></em>
      <?php echo Locale_Translate::_("For high security it is recommended to use this type of repository. Only those who have the secure keys to such a repository can write to it. Bibledit-Web supports this in case the GNU/Linux server has been set up to access this repository through passphrase-less private and public keys.") ?>
      <a href="collaboration_secure_network_setup.php?object=<?php echo $this->object ?>"><?php echo Locale_Translate::_("Set it up.") ?></a>
    </li>
  </ul>
<?php } ?>

<?php if (!$this->git_available) { ?>
  <div class="error">
    <?php echo Locale_Translate::_("Program 'git' cannot be found.") ?>
    <?php echo Locale_Translate::_("It is necessary for distributed collaboration.") ?>
    <?php echo Locale_Translate::_("Install it, then proceed.") ?>
  </div>
<?php } ?>
