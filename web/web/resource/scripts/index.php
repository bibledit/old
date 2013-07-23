<h3><?php echo gettext ("Resources") ?></h3>
<?php foreach ($this->resources as $offset => $resource) { ?>
  <h4><?php echo $resource ?></h4>
  <div id="bibledit<?php echo $offset ?>"></div>
<?php } ?>
<hr>
<p><a href="settings.php">[<?php echo gettext ("settings") ?>]</a></p>
<script>
<?php echo $this->script; ?>
</script>
<script type="text/javascript" src="index.js"></script>
