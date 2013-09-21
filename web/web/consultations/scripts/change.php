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
<br>
<p><?php echo $this->timestamp ?></p>
<?php if ($this->personal_change_proposal) { ?>
<p>
  <?php echo gettext ("This is your change proposal.") ?>
  <?php echo gettext ("You may check if it made its way into the Bible text.") ?>
</p>
<?php } ?>
<table>
  <tr>
    <td><?php echo gettext ("Old") ?>:</td>
    <td><?php echo $this->old_text ?></td>
  </tr>
  <tr>
    <td><?php echo gettext ("Change") ?>:</td>
    <td><?php echo $this->modification ?></td>
  </tr>
  <tr>
    <td><?php echo gettext ("New") ?>:</td>
    <td><?php echo $this->new_text ?></td>
  </tr>
</table>
<br>
<table>
  <?php foreach ($this->notes as $offset => $note) { ?>
    <tr>
      <td><a href="../notes/note.php?id=<?php echo $this->notes[$offset] ?>" target="_blank"><?php echo $this->summaries[$offset] ?></a></td>
      <td><?php if ($this->subscriptions[$offset]) { ?><a href="unsubscribe" id="unsubscribe<?php echo $this->notes[$offset] ?>">[<?php echo gettext ("unsubscribe") ?>]</a><?php } ?></td>
      <td><?php if ($this->assignments[$offset]) { ?><a href="unassign" id="unassign<?php echo $this->notes[$offset] ?>">[<?php echo gettext ("I have done my part on it") ?>]</a><?php } ?></td>
      <td><?php if ($this->level >= 5) { ?><a href="delete" id="delete<?php echo $this->notes[$offset] ?>">[<?php echo gettext ("mark for deletion") ?>]</a><?php } ?></td>
    </tr>
  <?php } ?>
</table>
<p>
  <a href="../editusfm/index.php" target="_blank"><?php echo gettext ("Edit text") ?></a>
  |
  <a href="../notes/create.php?fromchange=<?php echo $this->id ?>" target="_blank"><?php echo gettext ("Create note") ?></a>
</p>
<br>
