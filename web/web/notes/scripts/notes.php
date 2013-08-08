<?php foreach ($this->identifiers as $offset => $identifier) { ?>
  <a name="note<?php echo $this->identifiers[$offset] ?>"></a>
  <p><a href="note.php?id=<?php echo $this->identifiers[$offset] ?>"><?php echo $this->summaries[$offset] ?></a></p>
  <p><?php echo $this->versetexts[$offset] ?></p>
  <p><?php echo $this->contents[$offset] ?></p>
<?php } ?>
