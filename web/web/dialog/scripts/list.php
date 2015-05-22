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
<h2><?php echo $this->header ?></h2>
<?php if (!$this->horizontal) { ?>
  <p><?php echo $this->info_top ?></p>
<?php } ?>
<?php if (!$this->horizontal) { ?>
  <ul>
<?php } ?>
<?php foreach ($this->text_lines as $offset => $text_line) { ?>
  <?php if (!$this->horizontal) { ?><li><?php } ?><a href="<?php echo $this->caller_url ?><?php echo $this->get_parameters[$offset] ?>"><?php echo $this->text_lines[$offset] ?></a><?php if (!$this->horizontal) { ?></li><?php } ?>
<?php } ?>
<?php if (!$this->horizontal) { ?>
  </ul>
<?php } ?>
<?php if (!$this->horizontal) { ?>
  <p><?php echo $this->info_bottom ?></p>
<?php } ?>
<h3><a href="<?php echo $this->caller_url ?>"><?php echo Locale_Translate::_("Cancel") ?></a></h3>
