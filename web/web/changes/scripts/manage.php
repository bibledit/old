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
<h3><?php echo Locale_Translate::_("Manage Changes") ?></h3>
<p>
  <?php echo Locale_Translate::_("Bibledit-Web tracks the changes in the text of the Bibles.") ?>
  <?php echo Locale_Translate::_("It generates details about them during the night.") ?>
</p>
<ul>
<li><?php echo Locale_Translate::_("Lists with changes can be downloaded.") ?></li>
<li><?php echo Locale_Translate::_("Bibledit-Web emails lists with the changes to users who have subscribed to them.") ?></li>
<li>
  <?php echo Locale_Translate::_("It generates Change Notifications for users who have enabled that option.") ?>
  <?php echo Locale_Translate::_("Every user makes personal settings.") ?>
  <a href="../user/notifications.php"><?php echo Locale_Translate::_("Your settings are in the notifications.") ?></a>
</li>
</ul>
<p><?php echo Locale_Translate::_("Users who have change notifications awaiting their approval:") ?></p>
<table>
<?php foreach ($this->users as $offset => $user) { ?>
  <tr>
    <td><?php echo $this->users[$offset] ?>:</td>
    <td><?php echo $this->count[$offset] ?></td>
    <td><a href="manage.php?clear=<?php echo $this->users[$offset] ?>">[<?php echo Locale_Translate::_("clear") ?>]</a></td>
  </tr>
<?php } ?>
</table>
