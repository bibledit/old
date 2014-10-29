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
<br>
<table>
  <tr>
    <td><?php echo Locale_Translate::_("Old") ?>:</td>
    <td><?php echo $this->old_text ?></td>
  </tr>
  <tr>
    <td><?php echo Locale_Translate::_("Change") ?>:</td>
    <td><?php echo $this->modification ?></td>
  </tr>
  <tr>
    <td><?php echo Locale_Translate::_("New") ?>:</td>
    <td><?php echo $this->new_text ?></td>
  </tr>
</table>
<br>
<table>
  <?php foreach ($this->notes as $offset => $note) { ?>
    <tr>
      <td>
      <?php if ($this->live_notes_editor) { ?>
        <a class="opennote" href="<?php echo $this->notes[$offset] ?>"><?php echo $this->summaries[$offset] ?></a>
      <?php } else { ?>
        <a href="../notes/note.php?id=<?php echo $this->notes[$offset] ?>" target="_blank"><?php echo $this->summaries[$offset] ?></a>
      <?php } ?>
      </td>
      <td><?php if ($this->subscriptions[$offset]) { ?><a href="unsubscribe" id="unsubscribe<?php echo $this->notes[$offset] ?>">[<?php echo Locale_Translate::_("unsubscribe") ?>]</a><?php } ?></td>
      <td><?php if ($this->assignments[$offset]) { ?><a href="unassign" id="unassign<?php echo $this->notes[$offset] ?>">[<?php echo Locale_Translate::_("I have done my part on it") ?>]</a><?php } ?></td>
      <td><?php if ($this->level >= 5) { ?><a href="delete" id="delete<?php echo $this->notes[$offset] ?>">[<?php echo Locale_Translate::_("mark for deletion") ?>]</a><?php } ?></td>
    </tr>
  <?php } ?>
</table>
<p>
  <?php echo $this->timestamp ?>
  |
  <?php if ($this->live_notes_editor) { ?>
    <a class="newnote" href="<?php echo $this->id ?>"><?php echo Locale_Translate::_("Create note") ?></a>
  <?php } else { ?>
    <a href="../notes/create.php?fromchange=<?php echo $this->id ?>" target="_blank"><?php echo Locale_Translate::_("Create note") ?></a>
  <?php } ?>
</p>
<br>
