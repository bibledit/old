<?php
/*
Copyright (©) 2003-2015 Teus Benschop.

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
<h1>gettext("Web collaboration")</h1>
<p>gettext("With Bibledit several translators can work on one Bible at the same time.")</p>
<p>gettext("There is nothing special needed to make this happen.")</p>
<p>gettext("The people log into the site that runs Bibledit, and translate there.")</p>
<p>gettext("All the data related to the Bible is saved on that website, and is available to the relevant people.")</p>
<p>gettext("When collaborating in this way, the information given below on the rest of the page is not relevant.")</p>

<h1>gettext("Distributed collaboration")</h1>
<p>gettext("With distributed collaboration there are several copies of the Bible distributed all over the place or the globe.")</p>
<p>gettext("Translators and other people that collaborate in this way do not need to be connected to the Internet all the time, or even not at all.")</p>
<p>gettext("They collaborate through the network or a USB drive.")</p>
<p>gettext("Once everything has been set up, it all goes automatic. The person working on the Bible does not notice the mechanics of the collaboration. It all happens under the hood.")</p>
<p>gettext("There are a variety of methods for collaboration. The administrator who will set it all up needs to have sufficient knowledge of information technology.")</p>
<p>gettext("Bibledit stores all data locally. When collaboration is enabled, it also connects to a central repository. Through that repository the data gets shared between the various translators and other users.")</p>
<p>gettext("Under the hood, Bibledit uses Git for collaboration. Git is a content manager. The user does not notice that Git is used. He does not operate it directly. The only thing that the manager needs to do at regular times is to 'Send/Receive' the Bible. After having done that, the changes in the Bible, made by the other translators or users, appear on his screen too. And the changes made by himself get stored in the repository, and if the other users do Send/Receive also, then these changes will reflect on their screens also.")</p>
<p>gettext("If distributed collaboration is used, it needs to be set up per Bible.") <a href="collaboration?object=$this->object&select">gettext ("Select which Bible to use.")</a></p>
<?php if ($this->object != "") {
  <p>gettext("Setup for Bible"): $this->object</p>
  <?php if ($this->url != "") {
  <p>gettext("Distributed collaboration for this is enabled.") <a href="collaboration?object=$this->object&disable=">gettext ("Disable it.")</a></p>
  <?php }
  <p>gettext("There is a variety of repositories that can be used. They have their own use, advantages and disadvantages.")</p>
  <ul>
    <li>
      <em>gettext("USB Flash Drive.")</em>
      gettext("First of all there is the repository that resides on a file system. A USB flash drive can be used in this case. This flash drive can be physically moved from one translator to the other, and so on, updating each translator, and getting updated by each translator.")
      <a href="collaboration_flash_drive_setup?object=$this->object">gettext("Set it up.")</a>
    </li>
    <li>
      <em>gettext("Open Network Repository.")</em>
      gettext("Secondly there is the repository that resides on the network. For internal networks a simple open network repository can be used. Whoever knows where the repository resides can write to it.")
      <a href="collaboration_open_network_setup?object=$this->object">gettext("Set it up.")</a>
    </li>
    <li>
      <em>gettext("Password Protected Network Repository.")</em>
      gettext("For better security is is helpful to use a repository that needs a username and password for write access.")
      <a href="collaboration_password_protected_network_setup?object=$this->object">gettext("Set it up.")</a>
    </li>
    <li>
      <em>gettext("Secured Network Repository.")</em>
      gettext("For high security it is recommended to use this type of repository. Only those who have the secure keys to such a repository can write to it. Bibledit supports this in case the GNU/Linux server has been set up to access this repository through passphrase-less private and public keys.")
      <a href="collaboration_secure_network_setup?object=$this->object">gettext("Set it up.")</a>
    </li>
  </ul>
<?php }

<?php if (!$this->git_available) {
  <div class="error">
    gettext("Program 'git' cannot be found.")
    gettext("It is necessary for distributed collaboration.")
    gettext("Install it, then proceed.")
  </div>
<?php }
