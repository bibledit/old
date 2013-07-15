<?php foreach ($this->names as $offset => $name) { ?>
  <h3>
    <?php echo $name ?>
  </h3>
  <div>
    <?php echo $this->fragments [$offset] ?>
  </div>
<?php } ?>
