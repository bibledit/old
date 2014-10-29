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
<h3><?php echo Locale_Translate::_("Download resource") ?> "<?php echo $this->name ?>"</h3>
<p><a href="admin.php">[<?php echo Locale_Translate::_("back") ?>]</a></p>
<br>
<p>
  <?php echo Locale_Translate::_("Bibledit-Web can display external resources to the user.") ?>
  <?php echo Locale_Translate::_("It fetches the resources from websites on the Internet.") ?>
  <?php echo Locale_Translate::_("Some of those websites may be slow to respond.") ?>
  <?php echo Locale_Translate::_("Bibledit-Web may not always be able to reach them.") ?>
</p>
<p>
  <?php echo Locale_Translate::_("To address these issues, you can download external resources.") ?>
  <?php echo Locale_Translate::_("Bibledit-Web then stores the data in its database.") ?>
  <?php echo Locale_Translate::_("When a downloaded resource is needed, Bibledit-Web can fetch the data from its database rather than from the external website.") ?>
  <?php echo Locale_Translate::_("This works faster.") ?>
  <?php echo Locale_Translate::_("It takes the load off the website.") ?>
</p>
<br>
<p>
  <?php echo Locale_Translate::_("Number of verses available offline for this resource:") ?>
  <?php echo $this->count ?>
  <?php if ($this->count) { ?>
    [<a href="?name=<?php echo $this->name ?>&clear"><?php echo Locale_Translate::_("clear") ?></a>]
  <?php } ?>
</p>
<p><a href="?name=<?php echo $this->name ?>&download"><?php echo Locale_Translate::_("Start downloading this resource, en view the progress in the Journal.") ?></a></p>
<br>
<p>
  <?php echo Locale_Translate::_("It takes partial downloads into account, because it only downloads the missing pieces.") ?>
  <?php echo Locale_Translate::_("Clear any offline verses if a full download is needed.") ?>
</p>
