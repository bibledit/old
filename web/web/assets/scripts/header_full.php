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
<table width="100%">
<tr>
<td>
<a href="../start/index.php"><?php echo gettext ("Start") ?></a>
</td>
<td align="right">
<?php  
if ($this->user == "") {
  if ($this->logging_in != true) {
?>
    <a href="<?php echo $this->header_path_modifier ?>session/login.php?request=<?php echo $this->request ?>"><?php echo gettext ("Login") ?></a>
<?php  
  }
} else {
  if ($this->level >= 2) {
    echo '<form action="' . $this->header_path_modifier . 'search/search.php" method="get" name="search" id="search">' . "\n";
    echo '  <input name="q" type="text" value="' . $this->query . '"/>' . "\n";
    echo "  <input type=\"submit\" value=\"" . gettext ("Search") . "\" onClick=\"this.value = '" . gettext ("Searching") . "...'; return true;\" />\n";
    echo "</form>\n";
  }
  echo '<a href="' . $this->header_path_modifier . 'user/index.php">' . $this->user . "</a>\n";
}
?>
</td>
</tr>
</table>
