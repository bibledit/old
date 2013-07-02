<h2><?php echo $this->header ?></h2>
<?php if (!$this->horizontal) { ?>
  <p><?php echo $this->info_top ?></p>
<?php } ?>
<?php if (!$this->horizontal) { ?>
  <ul>
<?php } ?>
<?php foreach ($this->text_lines as $offset => $text_line) { ?>
  <?php if (!$this->horizontal) { ?><li><?php } ?><a href="<?php echo $this->caller_url ?><?php echo $this->get_parameters[$offset] ?>"><?php echo $this->text_lines[$offset] ?></a><?php if (!$this->horizontal) { ?></li><?php } ?>
<?php } ?>
<?php if (!$this->horizontal) { ?>
  </ul> 
<?php } ?>
<?php if (!$this->horizontal) { ?>
  <p><?php echo $this->info_bottom ?></p>
<?php } ?>
<h3><a href="<?php echo $this->caller_url ?>"><?php echo gettext ("Cancel") ?></a></h3>
