<br>
<p><?php echo $this->timestamp ?></p>
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
      <td><a href="notes.php?consultationnote=<?php echo $this->notes[$offset] ?>" target="_blank"><?php echo $this->summaries[$offset] ?></a></td>
      <td><?php if ($this->subscriptions[$offset]) { ?><a href="change.php?id=<?php echo $this->id ?>&unsubscribe=<?php echo $this->notes[$offset] ?>">[<?php echo gettext ("unsubscribe") ?>]</a><?php } ?></td>
      <td><?php if ($this->assignments[$offset]) { ?><a href="change.php?id=<?php echo $this->id ?>&unassign=<?php echo $this->notes[$offset] ?>">[<?php echo gettext ("I have done my part it") ?>]</a><?php } ?></td>
      <td><?php if ($this->level >= 5) { ?><a href="change.php?id=<?php echo $this->id ?>&delete=<?php echo $this->notes[$offset] ?>">[<?php echo gettext ("delete") ?>]</a><?php } ?></td>
    </tr>
  <?php } ?>
</table>
<p>
  <a href="../desktop/index.php?desktop=edittext" target="_blank"><?php echo gettext ("Edit text") ?></a>
  |
  <a href="change.php?createnote=<?php echo $this->id ?>" target="_blank"><?php echo gettext ("Create note") ?></a>
</p>
<br>
