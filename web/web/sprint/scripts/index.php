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
<h3><?php echo gettext ("Sprint") ?></h3>
<p class="error"><?php echo $this->error ?></p>
<p class="success"><?php echo $this->success ?></p>
<p>
  <a href="?previoussprint=">«</a>
  <a href="?currentprint="><?php echo $this->sprint ?></a>
  <a href="?nextsprint=">»</a>
  <a href="#help">[<?php echo gettext ("help") ?>]</a>
</p>
<br>
<table>
  <tr>
    <td></td>
    <td></td>
    <?php foreach ($this->categories as $offset => $category) { ?>
      <td style="text-align:center"><?php echo $category ?></td>
    <?php } ?>
  </tr>
<?php foreach ($this->tasks as $offset => $id) { ?>
  <tr>
    <td><a href="?id=<?php echo $id ?>&moveback="> « </a></td>
    <td><?php echo $this->titles [$offset] ?></td>
    <?php
    $threshold = intval (100 / count ($this->categories));
    foreach ($this->categories as $offset2 => $category) {
      $low = $offset2 * $threshold;
      $high = ($offset2 + 1) * $threshold;
      $background = "";
      if ($this->percentages [$offset] >= $high) {
        $background = "background-color:yellow;"; 
      }
      echo "<td style=\"text-align:center; $background\">";
      if ($this->percentages [$offset] >= $high) {
        echo "<a href=\"?id=$id&complete=$low\"> ☑ </a>";
      } else {
        echo "<a href=\"?id=$id&complete=$high\"> ☐ </a>";
      }
      echo "</td>";
    }
    ?>
    <td><a href="?id=<?php echo $id ?>&moveforward="> » </a></td>
  </tr>
<?php } ?>
</table>
<br>
<p><?php echo gettext ('Sprint Burndown Chart - Remaining Tasks') ?></p>
<?php if ($this->chart) { ?>
  <img src="data:image/png;base64,<?php echo $this->chart ?>" />
<?php } ?>
<p><?php echo $this->chart2 ?></p>
<br>
<p><a href="?mail="><?php echo gettext ("Mail information to subscribers") ?></a></p>
<br>
<form action="" name="addtask" method="post">
  <p>
    <?php echo gettext ("Add task") ?>
    <input type="text" name="add" maxlength="256" />
    <input type="submit" name="submit" value="<?php echo gettext ("Add") ?>" />
  </p>
</form>
<br>
<form action="" name="categories" method="post">
  <p>
    <?php echo gettext ("Enter task completion categories.") ?>
    <?php echo gettext ("One per line.") ?>
  </p>
  <p><textarea name="categories" style="width:100px"><?php echo $this->categorytext ?></textarea></p>  
  <p><input type="submit" name="save" value="<?php echo gettext ("Save") ?>" /></p>
</form>
<br>
<a id="help"></a>
<p>
    <?php echo gettext ("") ?>
</p>
