<?php
/*
Copyright (©) 2003-2013 Teus Benschop.

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
<h1><?php echo gettext ("Stylesheet") ?> <?php echo $this->name ?></h1>

<p>
  <?php echo gettext ("This shows the various styles contained in this stylesheet.") ?>
  [<a href="indexm.php?delete=<?php echo $this->name ?>"><?php echo gettext ("delete") ?>]</a></p>

<table>
  <thead>
    <tr>
      <td><?php echo gettext ("Style") ?></td>
      <td><?php echo gettext ("Name") ?></td>
      <td></td>
    </tr>
  </thead>
  <tbody>
    <?php foreach ($this->markers as $offset => $marker) { ?>
    <tr>
      <td><a href="view.php?sheet=<?php echo $this->name ?>&style=<?php echo $marker ?>"><?php echo $marker ?></a></td>
      <td><?php echo $this->names[$offset] ?></td>
      <td>[<a href="sheetm.php?name=<?php echo $this->name ?>&delete=<?php echo $marker ?>"><?php echo gettext ("delete") ?>]</a></td>
    </tr>
    <?php } ?>
  </tbody>
</table>

<p><?php echo gettext ("You can modify this stylesheet. To do that, make changes to the existing xml data in the textbox below, and then submit it. It will overwrite whatever was there before.") ?></p>

<form action="sheetm.php?name=<?php echo $this->name ?>" name="form" method="post">
  <p><textarea name="data"><?php echo $this->xml ?></textarea></p>
  <p><input type="submit" name="submit" value=<?php echo gettext ("Submit") ?> /></p>
</form>
