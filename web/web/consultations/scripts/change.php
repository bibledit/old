<h1><?php echo gettext ("Change") ?></h1>
<p><a href="changes.php">[<?php echo gettext ("back") ?>]</a> <a href="#help">[<?php echo gettext ("help") ?>]</a></p>
<p class="error"><?php echo $this->error ?></p>
<p class="success"><?php echo $this->success ?></p>
<br>
<p><?php echo $this->passage ?></p>
<table>
  <tr>
    <td></td>
    <td><?php echo gettext ("Old") ?>:</td>
    <td><?php echo $this->old_text ?></td>
  </tr>
  <tr>
    <td><a href="changes.php?approve=<?php echo $this->id ?>"> ✔ </a></td>
    <td><?php echo gettext ("Change") ?>:</td>
    <td><?php echo $this->modification ?></td>
  </tr>
  <tr>
    <td></td>
    <td><?php echo gettext ("New") ?>:</td>
    <td><?php echo $this->new_text ?></td>
  </tr>
</table>
<br>
<table>
  <?php foreach ($this->notes as $offset => $note) { ?>
    <tr>
      <td><a href="notes.php?consultationnote=<?php echo $this->notes[$offset] ?>" target="_blank"><?php echo $this->summaries[$offset] ?></a></td>
      <td>{if $subscriptions[offset]}<a href="change.php?id=<?php echo $this->id ?>&unsubscribe=<?php echo $this->notes[$offset] ?>">[<?php echo gettext ("unsubscribe") ?>]</a>{/if}</td>
      <td>{if $assignments[offset]}<a href="change.php?id=<?php echo $this->id ?>&unassign=<?php echo $this->notes[$offset] ?>">[<?php echo gettext ("I have done my part it") ?>]</a>{/if}</td>
      <td>{if $level >= 5}<a href="change.php?id=<?php echo $this->id ?>&delete=<?php echo $this->notes[$offset] ?>">[<?php echo gettext ("delete") ?>]</a>{/if}</td>
    </tr>
  <?php } ?>
</table>
<p><a href="change.php?id=<?php echo $this->id ?>&create=<?php echo $this->id ?>" target="_blank"><?php echo gettext ("Create note") ?></a></p>
<br>
<p><?php echo $this->timestamp ?></p>
<br>
<a id="help"></a>
<p>
  <?php echo gettext ("This shows the details of the change in the text of the Bible.") ?>
</p>
<p>
  <?php echo gettext ("It shows the old text, the change, and the new text.") ?>
</p>
<p>
  <?php echo gettext ("Click ✔ to approve a change, and then to go back to the list of changes.") ?>
</p>
<p>
  <?php echo gettext ("It shows all Consultation Notes about the passage.") ?>
</p>
<p>
  <?php echo gettext ("If you are subscribed to any of the notes, you can unsubscribe.") ?>
</p>
<p>
  <?php echo gettext ("If any of the notes is assigned to you, you can unassign it.") ?>
</p>
<p>
  {if $level >= 5}
  <?php echo gettext ("You can delete any of the notes.") ?>
  {/if}
</p>
<p>
  <?php echo gettext ("You can create a new Consultation Notes with a query about the change in the text of the Bible.") ?>
</p>
<p>
  <?php echo gettext ("It displays the date when the modification was processed.") ?>
</p>
