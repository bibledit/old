<p>
  <a href="edit.php"><?php echo gettext ("Notes") ?>: <?php echo $this->count ?></a>
  <?php if ($this->level >= 5) { ?>
    |
    [<a href="bulk.php"><?php echo gettext ("update") ?></a>]
  <?php } ?>
  |
  <a href="create.php"><?php echo gettext ("Create note") ?></a>  
</p>
<?php foreach ($this->identifiers as $offset => $identifier) { ?>
  <a name="note<?php echo $this->identifiers[$offset] ?>"></a>
  <p><a href="note.php?id=<?php echo $this->identifiers[$offset] ?>"><?php echo $this->summaries[$offset] ?></a></p>
  <p><?php echo $this->versetexts[$offset] ?></p>
  <p><?php echo $this->contents[$offset] ?></p>
<?php } ?>
