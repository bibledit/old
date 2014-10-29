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
<p class="error"><?php echo $this->errorr ?></p>
<p class="success"><?php echo $this->success ?></p>
<p><?php echo Locale_Translate::_("The Bible book below needs a matching abbreviation for the target Bible.") ?></p>
<br>
<form action="translate.php" name="pair" method="post">
  <table>
    <tr>
      <td>1. <?php echo Locale_Translate::_("Book name:") ?></td>
      <td><?php echo $this->bookname ?><input type="hidden" name="fullname" maxlength="50" value="<?php echo $this->bookname ?>" /></td>
    </tr>
    <tr>
      <td>2. <?php echo Locale_Translate::_("Enter the abbreviation:") ?></td>
      <td><input type="text" name="abbreviation" maxlength="50" value="" /></td>
    </tr>
    <tr>
      <td>3. <?php echo Locale_Translate::_("Save it:") ?></td>
      <td><input type="submit" name="save" value=<?php echo Locale_Translate::_("Save") ?> /></td>
    </tr>
  </table>
</form>
<br>
<?php if ($this->remaining) { ?>
  <p><?php echo Locale_Translate::_("Remaining book names without an abbreviation:") ?> <?php echo $this->remaining ?>.</p>
<?php } ?>
