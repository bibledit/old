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
<p><?php echo Locale_Translate::_("The abbreviation below needs clarification as to which Bible book it refers to.") ?></p>
<br>
<form action="interpret.php" name="pair" method="post">
  <table>
    <tr>
      <td>1. <?php echo Locale_Translate::_("Optionally update the abbreviation:") ?></td>
      <td><input type="text" name="abbreviation" maxlength="50" value="<?php echo $this->abbreviation ?>" /></td>
    </tr>
    <tr>
      <td>2. <?php echo Locale_Translate::_("Select the Bible book it represents:") ?></td>
      <td>
        <select name="fullname">
        <?php foreach ($this->books as $book) { ?>
          <option value="<?php echo $book ?>"><?php echo $book ?></option>
        <?php } ?>
        </select>
      </td>
    </tr>
    <tr>
      <td>3. <?php echo Locale_Translate::_("Save it:") ?></td>
      <td><input type="submit" name="save" value=<?php echo Locale_Translate::_("Save") ?> /></td>
    </tr>
  </table>
</form>
<br>
<?php if ($this->remaining) { ?>
  <p><?php echo Locale_Translate::_("Remaining abbreviations to clarify:") ?> <?php echo $this->remaining ?>.</p>
<?php } ?>
