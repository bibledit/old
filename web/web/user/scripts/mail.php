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
<h1><?php echo gettext ("Mail") ?></h1>

<p>
  <a href="<?php echo $this->link_inbox ?>" <?php if ($this->active_label == 'inbox') { ?>class="active"<?php } ?>>
  <?php echo gettext ("Inbox") ?></a>
  |
  <a href="<?php echo $this->link_trash ?>"<?php if ($this->active_label == 'trash') { ?>class="active"<?php } ?>>
  <?php echo gettext ("Trash") ?></a>
</p>

<?php if ($this->body != "") { ?>

  <p><?php echo $this->subject ?></p>
  <p>-</p>
  <p><?php echo $this->body ?></p>

<?php } else { ?>

  <table>
    <thead>
      <tr>
        <td><?php echo gettext ("Subject") ?></td>
        <td></td>
        <td><?php echo gettext ("Date") ?></td>
        <td></td>
        <td></td>
      </tr>
    </thead>
    <tbody>
      <?php foreach ($this->ids as $offset => $id) { ?>
      <tr>
        <td><a href="<?php echo $this->views[$offset] ?>"><?php echo $this->subjects[$offset] ?></a></td>
        <td>|</td>
        <td><?php echo $this->timestamps[$offset] ?></td>
        <td>|</td>
        <td><a href="<?php echo $this->deletes[$offset] ?>">[<?php echo gettext ("delete") ?>]</a></td>
      </tr>
      <?php } ?>
    </tbody>
  </table>

<?php } ?>
<p><?php echo gettext ("Mail older than 90 days will automatically be deleted.") ?></p>
