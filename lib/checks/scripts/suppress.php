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
<h1><?php echo Locale_Translate::_("Suppressed check results") ?></h1>
<p><a href="index.php">[<?php echo Locale_Translate::_("back") ?>] <a href="#help">[<?php echo Locale_Translate::_ ("help") ?>]</a></p>
<br>
<p class="error"><?php echo $this->error ?></p>
<p class="success"><?php echo $this->success ?></p>
<?php foreach ($this->ids as $offset => $id) { ?>
  <p style="color:grey;">
  <a href="suppress.php?release=<?php echo $id ?>">
  ✗
  </a>
  <?php echo $this->data[$offset] ?>
  </p>
<?php } ?>
<br>
<a id="help"></a>
<p>
  <?php echo Locale_Translate::_("The page lists the suppressed results of the checks.") ?>
</p>
<p>
  <?php echo Locale_Translate::_("Click ✗ to remove the items from the suppressed list.") ?>
  <?php echo Locale_Translate::_("It will be visible again in the list of check results.") ?>
</p>
