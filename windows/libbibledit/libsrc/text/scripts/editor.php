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
<?php foreach ($this->line_data_before as $offset => $line) { ?>
  <a name="line<?php echo $this->line_numbers_before[$offset] ?>"></a>
  <p><a href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&editusfmline=<?php echo $this->line_numbers_before[$offset] ?>#line<?php echo $this->line_numbers_before[$offset] ?>"><?php echo $this->line_data_before[$offset] ?></a></p>
<?php } ?>
<?php if (isset ($this->line_data_edit)) { ?>
  <a name="line<?php echo $this->line_number_edit ?>"></a>
  <form action="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&bible=<?php echo $this->bible ?>&book=<?php echo $this->book ?>&chapter=<?php echo $this->chapter ?>&textsaveline=<?php echo $this->line_number_edit ?>#line<?php echo $this->goto_line ?>" name="form" method="post">
  <p><textarea name="data"><?php echo $this->line_data_edit ?></textarea></p>
  <p><input type="submit" name="submit" value=<?php echo Locale_Translate::_("Save") ?> /><input type="submit" name="cancel" value=<?php echo Locale_Translate::_ ("Cancel") ?> /></p>
</form>
<?php } ?>
<?php foreach ($this->line_data_after as $offset => $line) { ?>
  <a name="line<?php echo $this->line_numbers_after[$offset] ?>"></a>
  <p><a href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&editusfmline=<?php echo $this->line_numbers_after[$offset] ?>#line<?php echo $this->line_numbers_after[$offset] ?>"><?php echo $this->line_data_after[$offset] ?></a></p>
<?php } ?>
