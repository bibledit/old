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
<?php foreach ($this->identifiers as $offset => $identifier) { ?>
  <a name="note<?php echo $this->identifiers[$offset] ?>"></a>
  <p><a href="note.php?id=<?php echo $this->identifiers[$offset] ?>"><?php echo $this->summaries[$offset] ?></a></p>
  <p><?php echo $this->versetexts[$offset] ?></p>
  <p><?php echo $this->contents[$offset] ?></p>
<?php } ?>
