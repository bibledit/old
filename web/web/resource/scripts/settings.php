<h2><?php echo gettext ("Settings for Resources") ?></h2>
<p><a href="index.php">[<?php echo gettext ("back") ?></a>]</p>
<h3><?php echo gettext ("Active resources") ?></h3>
<p>
  <?php echo gettext ("To change the order, drag an item to another location.") ?>
  <?php echo gettext ("Click ✗ to remove.") ?>
</p>
<br>
<div id="sortable">
<?php foreach ($this->actives as $active) { ?>
  <p><a href="?remove=<?php echo $active ?>"> ✗ </a><?php echo $active ?></p>
<?php } ?>
</div>
<h3><?php echo gettext ("Available internal Bible resources") ?></h3>
<p>
<?php echo gettext ("Click to activate:") ?>
<?php foreach ($this->bibles as $offset => $bible) { ?>
  <?php if ($offset) echo " | " ?>
  <a href="?add=<?php echo $bible ?>"><?php echo $bible ?></a>
<?php } ?>
</p>
<script type="text/javascript" src="settings.js"></script>
