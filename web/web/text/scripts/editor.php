<?php foreach ($this->line_data_before as $offset => $line) { ?>
  <a name="line<?php echo $this->line_numbers_before[$offset] ?>"></a>
  <p><a href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&editusfmline=<?php echo $this->line_numbers_before[$offset] ?>#line<?php echo $this->line_numbers_before[$offset] ?>"><?php echo $this->line_data_before[$offset] ?></a></p>
<?php } ?>
<?php if (isset ($this->line_data_edit)) { ?>
  <a name="line<?php echo $this->line_number_edit ?>"></a>
  <form action="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&bible=<?php echo $this->bible ?>&book=<?php echo $this->book ?>&chapter=<?php echo $this->chapter ?>&textsaveline=<?php echo $this->line_number_edit ?>#line<?php echo $this->goto_line ?>" name="form" method="post">
  <p><textarea name="data"><?php echo $this->line_data_edit ?></textarea></p>
  <p><input type="submit" name="submit" value=<?php echo gettext ("Save") ?> /><input type="submit" name="cancel" value=<?php echo gettext ("Cancel") ?> /></p>
</form>
<?php } ?>
<?php foreach ($this->line_data_after as $offset => $line) { ?>
  <a name="line<?php echo $this->line_numbers_after[$offset] ?>"></a>
  <p><a href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&editusfmline=<?php echo $this->line_numbers_after[$offset] ?>#line<?php echo $this->line_numbers_after[$offset] ?>"><?php echo $this->line_data_after[$offset] ?></a></p>
<?php } ?>
