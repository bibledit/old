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
<h3><?php echo gettext ("Consistency") ?></h3>
<p><a href="#help">[<?php echo gettext ("help") ?>]</a></p>
<br>
<table>
<tr>
<td style="width:10%;">
  <p><?php echo gettext ("Paste passages:") ?></p>
  <textarea id="passages"></textarea>
  <p><?php echo gettext ("Paste translations:") ?></p>
  <textarea id="translations"></textarea>
</td>
<td style="width:1%;"></td>
<td style="vertical-align: top;"><div id="texts"></div></td>
</tr>
</table>
<br>
<a id="help"></a>
<p>
  <?php echo gettext ("This assists with checking how consistent words have been translated in the Bible.") ?>
</p>
<p>
  <?php echo gettext ("Enter or paste the passages to check.") ?>
  <?php echo gettext ("") ?>
</p>
<p>
  <?php echo gettext ("") ?>
  <?php echo gettext ("") ?>
</p>
<script type="text/javascript" src="index.js"></script>
