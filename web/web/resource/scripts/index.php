<div><?php echo $this->navigationHtml ?></div>
<h4><?php echo gettext ("Resources") ?></h4>
<hr>
<?php foreach ($this->resources as $offset => $resource) { ?>
  <p><?php echo $resource ?></p>
  <div id="bibledit<?php echo $offset ?>"></div>
  <hr>
<?php } ?>
<p><a href="settings.php">[<?php echo gettext ("settings") ?>]</a></p>
<script><?php echo $this->script; ?></script>
<script type="text/javascript" src="index.js"></script>
<?php echo $this->navigationCode; ?>
