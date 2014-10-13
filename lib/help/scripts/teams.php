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
<h1><?php echo Locale_Translate::_("Teams") ?></h1>
<p>
  <?php echo Locale_Translate::_("Bibledit-Web can host many Bibles.") ?>
  <?php echo Locale_Translate::_("Many users can create an account.") ?>
</p>
<br>
<p>
  <?php echo Locale_Translate::_("A translation team is formed by giving a group of users access to a Bible.") ?>
</p>
<ul>
<li>
  <?php echo Locale_Translate::_("A user may be granted read-only access to a Bible, or write access.") ?>
  <?php echo Locale_Translate::_("The type of access also depends on the role of a user.") ?>
  <a href="../manage/users.php" target="_blank"><?php echo Locale_Translate::_("Manage users.") ?></a>
</li>
<li>
  <?php echo Locale_Translate::_("Bibles can be made viewable by all users.") ?>
  <a href="../bible/manage.php" target="_blank"><?php echo Locale_Translate::_("Manage Bibles.") ?></a>
</li>
</ul>



