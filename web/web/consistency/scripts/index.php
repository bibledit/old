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
<style type="text/css">
  *.searchResult {
      font-weight: bold;
      background-color: yellow;
  }
</style>
<h3><?php echo gettext ("Consistency") ?></h3>
<p><a href="#help">[<?php echo gettext ("help") ?>]</a></p>
<br>
<table>
<tr style="vertical-align: top;">
<td style="width:10%;">
  <p><?php echo gettext ("Passages:") ?></p>
  <textarea id="passages"></textarea>
  <p><?php echo gettext ("Translations:") ?></p>
  <textarea id="translations"></textarea>
</td>
<td style="width:1%;"></td>
<td><div id="texts"></div></td>
</tr>
</table>
<br>
<a id="help"></a>
<p>
  <?php echo gettext ("This assists with checking how consistent words have been translated in the Bible.") ?>
</p>
<p>
  <?php echo gettext ("Enter the passages to check.") ?>
  <?php echo gettext ("Example:") ?>
</p>
<p>Exod. 37:4-5, 14-15, 27-28</p>
<p>Exod. 38:5-7</p>
<p>Exod. 39:35, 39</p>
<p>
  <?php echo gettext ("The web page will load the verse text for the passages.") ?>
</p>
<p>
  <?php echo gettext ("Enter the translations for the key words.") ?>
  <?php echo gettext ("Enter one word per line.") ?>
</p>
<p>
  <?php echo gettext ("The web page will mark the translations in the verse text.") ?>
</p>
<p>
  <?php echo gettext ("") ?>
  <?php echo gettext ("") ?>
</p>

<p><?php echo gettext ("Add text from the following Bibles or USFM Resources as extra aids for comparison") ?>:
<?php foreach ($this->bibles as $bible) { ?>
  <?php echo $bible ?>
  <a href="?removebible=<?php echo $bible ?>">[<?php echo gettext ("remove") ?>]</a>
  |
<?php } ?>
<a href="?addbible=">[<?php echo gettext ("add Bible") ?>]</a>
<script type="text/javascript" src="index.js"></script>
