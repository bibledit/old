<?php
/*
Copyright (©) 2003-2015 Teus Benschop.

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
<p>
  <?php echo gettext("Cross references") ?>
  <a href="help.php">[<?php echo gettext("help") ?>]</a>
  |
  <?php echo gettext("source Bible:") ?> <a href="?source=" title="<?php echo gettext ("Select source Bible") ?>"><?php echo $this->source ?></a>
  <a href="extract.php" title="<?php echo gettext("Copy cross references from source to target for the entire chapter") ?>"> ⇉ <?php echo gettext ("copy cross references entire chapter") ?> ⇉ </a>
  <?php echo gettext("target Bible:") ?> <a href="?target=" title="<?php echo gettext ("Select target Bible") ?>"><?php echo $this->target ?></a>
  |
  <a href="next.php" title="<?php echo gettext("Go to the next chapter without cross references") ?>"><?php echo gettext ("next chapter without cross references") ?></a>
</p>
<p class="error"><?php echo $this->error ?></p>
<p class="success"><?php echo $this->success ?></p>
<table>
<tr>
<td class="topalign"><div id="sourcetext"></div></td>
<td> </td>
<td class="topalign"><div id="targettext"></div></td>
</tr>
</table>
<script type="text/javascript" src="index.js?<?php echo config_logic_version () ?>"></script>
<style type="text/css">
.focus { 
  border-radius: 100px;
  background-color: rgb(255,0,255);
  transition: 500ms linear 0s;
  font-size: larger;
}
</style>
