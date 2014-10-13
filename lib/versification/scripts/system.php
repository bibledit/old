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
<h1><?php echo Locale_Translate::_("Versification system") ?> <?php echo $this->name ?></h1>

<p><?php echo Locale_Translate::_("This shows how many verses each chapter of the Bible has according to this versification system, and how many chapters each book has. The data is given in two formats. First in human readable form, and then in xml format if you scroll further down the page.") ?></p>

<p><a href="index.php?delete=<?php echo $this->name ?>"><?php echo Locale_Translate::_("Delete this versification") ?></a></p>

<table>
  <thead>
    <tr>
      <td><?php echo Locale_Translate::_("Book") ?></td>
      <td><?php echo Locale_Translate::_("Chapter") ?></td>
      <td><?php echo Locale_Translate::_("Verses") ?></td>
    </tr>
  </thead>
  <tbody>
    <?php foreach ($this->books as $offset => $book) { ?>
    <tr>
      <td><?php echo $book ?></td>
      <td><?php echo $this->chapters[$offset] ?></td>
      <td><?php echo $this->verses[$offset] ?></td>
    </tr>
    <?php } ?>
  </tbody>
</table>

<pre>
<?php echo $this->xml ?>
</pre>

<p><?php echo Locale_Translate::_("You can modify this versification system. To do that, copy the existing xml data from this page into an editor, make the changes, and paste the data back into the textbow below, and then submit it. It will overwrite whatever was there before.") ?></p>

<form action="?name=<?php echo $this->name ?>" name="form" method="post">
  <p><textarea name="data"></textarea></p>
  <p><input type="submit" name="submit" value=<?php echo Locale_Translate::_("Submit") ?> /></p>
</form>
