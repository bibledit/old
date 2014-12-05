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
<h1><?php echo gettext("Stylesheet") ?> <?php echo $this->name ?></h1>

<table>
  <thead>
    <tr>
      <td><?php echo gettext("Style") ?></td>
      <td><?php echo gettext("Name") ?></td>
      <td></td>
    </tr>
  </thead>
  <tbody>
    <?php foreach ($this->markers as $offset => $marker) { ?>
    <tr>
      <td><a href="view.php?sheet=<?php echo $this->name ?>&style=<?php echo $marker ?>"><?php echo $marker ?></a></td>
      <td><?php echo $this->names[$offset] ?></td>
      <td>[<a href="sheetm.php?name=<?php echo $this->name ?>&delete=<?php echo $marker ?>"><?php echo gettext("delete") ?>]</a></td>
    </tr>
    <?php } ?>
  </tbody>
</table>

<br>

<p><a href="sheetm.php?name=<?php echo $this->name ?>&new="><?php echo gettext("Add a style to the stylesheet.") ?></a></p>
<p><a href="indexm.php?delete=<?php echo $this->name ?>"><?php echo gettext("Delete the entire stylesheet.") ?></a></p>
<br>

<p>
  <?php echo gettext("Below is the stylesheet in XML format.") ?>
  <?php echo gettext("You can edit the XML, then submit it.") ?>
  <?php echo gettext("It will then update the stylesheet.") ?>
</p>
<br>

<form action="sheetm.php?name=<?php echo $this->name ?>" name="form" method="post">
  <p><textarea name="data"><?php echo $this->xml ?></textarea></p>
  <p><input type="submit" name="submit" value=<?php echo gettext("Submit") ?> /></p>
</form>
