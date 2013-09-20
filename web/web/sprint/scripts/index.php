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
<?php foreach ($this->tasks as $offset => $id) { ?>
  <tr>
    <td><a href="?id=<?php echo $id ?>&moveback="> « </a></td>
    <td><?php echo $this->titles [$offset] ?></td>
    <td>
      <a href="?id=<?php echo $id ?>&complete=0">
        <?php echo ($this->percentages[$offset] >= 0) ? "<mark>" : "" ?> 
        0 
        <?php echo ($this->percentages[$offset] >= 0) ? "</mark>" : "" ?>
      </a>
      |
      <a href="?id=<?php echo $id ?>&complete=25"> 
        <?php echo ($this->percentages[$offset] >= 25) ? "<mark>" : "" ?> 
        25 
        <?php echo ($this->percentages[$offset] >= 25) ? "</mark>" : "" ?>
      </a>
      |
      <a href="?id=<?php echo $id ?>&complete=50"> 
        <?php echo ($this->percentages[$offset] >= 50) ? "<mark>" : "" ?> 
        50 
        <?php echo ($this->percentages[$offset] >= 50) ? "</mark>" : "" ?>
      </a>
      |
      <a href="?id=<?php echo $id ?>&complete=75"> 
        <?php echo ($this->percentages[$offset] >= 75) ? "<mark>" : "" ?> 
        75 
        <?php echo ($this->percentages[$offset] >= 75) ? "</mark>" : "" ?>
      </a>
      |
      <a href="?id=<?php echo $id ?>&complete=100"> 
        <?php echo ($this->percentages[$offset] >= 100) ? "<mark>" : "" ?> 
        100 
        <?php echo ($this->percentages[$offset] >= 100) ? "</mark>" : "" ?>
      </a>
      %
    </td>
    <td><a href="?id=<?php echo $id ?>&moveforward="> » </a></td>
  </tr>
<?php } ?>
</table>
<br>
<img src="data:image/png;base64,<?php echo $this->chart ?>" />
<br>
<form action="" name="addtask" method="post">
  <p>
    <?php echo gettext ("Add task") ?>
    <input type="text" name="add" maxlength="256" />
    <input type="submit" name="submit" value="<?php echo gettext ("Add") ?>" />
  </p>
</form>
<p><a href="?mail="><?php echo gettext ("Mail information to subscribers") ?></a></p>
<br>
<a id="help"></a>
<p>
    <?php echo gettext ("") ?>
</p>
