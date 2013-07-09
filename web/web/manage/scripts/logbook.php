<h1><?php echo gettext ("Logbook") ?></h1>
<p>
  <?php echo $this->date; ?>
  |
  <a href="?delete="><?php echo gettext ("clear"); ?></a>
  |
  <a href="?"><?php echo gettext ("today"); ?></a>
  |
  <a href="?day=1"><?php echo gettext ("yesterday"); ?></a>
  |
  <a href="?day=2"><?php echo gettext ("two days ago"); ?></a>
  |
  <a href="?day=3"><?php echo gettext ("three days ago"); ?></a>
</p>
<div>
<?php  
foreach ($this->lines as $line) {
  echo "<p>$line</p>\n";
}
?>
</div>
<script>
<?php echo $this->script; ?>
</script>
<script type="text/javascript" src="logbook.js"></script>
