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
<h3>gettext("Sprint")</h3>
<p class="error">$this->error</p>
<p class="success">$this->success</p>
<p>
  gettext("Bible"):
  <a href="?bible=">$this->bible</a>
  |
  <a href="?previoussprint=">«</a>
  <a href="?currentprint=">$this->sprint</a>
  <a href="?nextsprint=">»</a>
</p>
<br>
<table>
  <tr>
    <td></td>
    <td></td>
    <?php for ($this->categories as $offset => $category) {
      <td style="text-align:center">$category</td>
    <?php }
  </tr>
<?php for ($this->tasks as $offset => $id) {
  <tr>
    <td><a href="?id=$id&moveback="> « </a></td>
    <td>$this->titles [$offset]</td>
    <?php
    $category_count = count ($this->categories);
    $category_percentage = 100 / $category_count;
    for ($this->categories as $offset2 => $category) {
      $low = intval ($offset2 * $category_percentage);
      $high = intval (($offset2 + 1) * $category_percentage);
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
   
    <td><a href="?id=$id&moveforward="> » </a></td>
  </tr>
<?php }
</table>
<br>
<p>gettext('Sprint Burndown Chart - Remaining Tasks')</p>
<?php if ($this->chart) {
  <img src="data:image/png;base64,$this->chart" />
<?php }
<p>$this->chart2</p>
<br>
<p><a href="?mail=">gettext("Mail information to subscribers")</a></p>
<br>
<form action="index.php" name="addtask" method="post">
  <p>
    gettext("Add task")
    <input type="text" name="add" maxlength="256" />
    <input type="submit" name="submit" value="gettext("Add")" />
  </p>
</form>
<br>
<form action="" name="categories" method="post">
  <p>
    gettext("Enter task completion categories.")
    gettext("One per line.")
  </p>
  <p><textarea name="categories" style="width:100px">$this->categorytext</textarea></p>
  <p><input type="submit" name="save" value="gettext("Save")" /></p>
</form>
<br>
<a id="help"></a>
