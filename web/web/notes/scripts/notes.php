<p><a href="../notes/create.php"><?php echo gettext ("Create note") ?></a></p>
<p>
<?php if ($this->firstnote > 1) { ?>
  <a href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&showpreviousconsultationnotes=">««</a>
<?php } ?>
<?php echo gettext ("Displaying notes") ?> <?php echo $this->firstnote ?> - <?php echo $this->lastnote ?> / <?php echo $this->totalcount ?>
  [<a href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnoteseditview="><?php echo gettext ("edit") ?></a>]
<?php if ($this->lastnote < $this->totalcount) { ?>
  <a href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&shownextconsultationnotes=">»»</a>
<?php } ?>
<?php if ($this->level >= 5) { ?>
  |
  [<a href="../notes/bulk.php"><?php echo gettext ("update") ?></a>]
<?php } ?>
</p>
<?php foreach ($this->identifiers as $offset => $identifier) { ?>
  <a name="note<?php echo $this->identifiers[$offset] ?>"></a>
  <p><a href="../notes/note.php?id=<?php echo $this->identifiers[$offset] ?>"><?php echo $this->summaries[$offset] ?></a></p>
  <p><?php echo $this->versetexts[$offset] ?></p>
  <p><?php echo $this->contents[$offset] ?></p>
<?php } ?>
