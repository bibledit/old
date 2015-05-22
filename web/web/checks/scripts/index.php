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
<h1><?php echo Locale_Translate::_("Checks") ?></h1>
<p><a href="#help">[<?php echo Locale_Translate::_("help") ?>]</a></p>
<br>
<p class="error"><?php echo $this->error ?></p>
<p class="success"><?php echo $this->success ?></p>
<?php foreach ($this->ids as $offset => $id) { ?>
  <p>
    <a href="index.php?approve=<?php echo $id ?>"> ✔ </a>
    <a href="index.php?delete=<?php echo $id ?>"> ✗ </a>
    <?php echo $this->bibles[$offset] ?>
    <?php echo $this->links[$offset] ?>
    <?php echo $this->information[$offset] ?>
  </p>
<?php } ?>
<br>
<a id="help"></a>
<p><?php echo Locale_Translate::_("The page displays a list of results of running the checks.") ?></p>
<p><?php echo Locale_Translate::_("Click ✔ to approve and suppress the entry.") ?></p>
<p>
  <?php echo Locale_Translate::_("Click ✗ to remove the entry for just now.") ?>
  <?php echo Locale_Translate::_("It will be back next day, unless the text of the Bible is corrected.") ?>
</p>
<p><?php echo Locale_Translate::_("Click an entry to open the chapter in the text editor.") ?></p>
<p><?php echo Locale_Translate::_("The Manager should enable the checks.") ?></p>
<p><a href="suppress.php"><?php echo Locale_Translate::_("Suppressed entries") ?></a></p>
<script type="text/javascript" src="index.js?<?php echo Config_Logic::version () ?>"></script>
<script type="text/javascript" src="../navigation/nav.js?<?php echo Config_Logic::version () ?>"></script>
