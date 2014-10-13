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
<h1><?php echo Locale_Translate::_("Repository data") ?></h1>
<p><?php echo Locale_Translate::_("You are now at a stage that the data in Bibledit-Web, and the data in the repository, needs to be synchronized. This means that some action needs to be taken to make these two sets of data the same. Not all the data in Bibledit-Web needs to be synchronized with the repository. It is only the data in the one Bible or the Consultation Notes you are now dealing with.") ?></p>
<p><?php echo Locale_Translate::_("Bible") ?>: <?php echo $this->object ?></p>

<p><?php echo Locale_Translate::_("There are two options:") ?></p>
<ol>
  <li><em><?php echo Locale_Translate::_("Take the data in the repository.") ?></em> <?php echo Locale_Translate::_ ("Taking the data in the repository means that the data in the repository is left as it is, and that a copy of that data overwrites your own data in Bibledit-Web. This option is used most of the time.") ?> <a href="collaboration_take_repo.php?object=<?php echo $this->object ?>"><?php echo Locale_Translate::_("Take the data in the repository.") ?></a></li>
  <li><em><?php echo Locale_Translate::_("Take your own data.") ?></em> <?php echo Locale_Translate::_ ("Taking your own data means that your data is taken as it is, and that this is copied to the repository, overwriting whatever is there in the repository now.") ?> <a href="collaboration_take_yourself.php?object=<?php echo $this->object ?>"><?php echo Locale_Translate::_("Take your data.") ?></a></li>
</ol>
