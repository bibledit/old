<h2><?php echo gettext ("Settings for Resources") ?></h2>
<p><a href="index.php">[<?php echo gettext ("back") ?></a>]</p>
<h3><?php echo gettext ("Active resources") ?></h3>
<div id="sortable">
<?php foreach ($this->actives as $active) { ?>
  <p><a href="?remove=<?php echo $active ?>"> ✗ </a><?php echo $active ?></p>
<?php } ?>
</div>
<br>
<p>
  <?php echo gettext ("To change the order, drag an item to another location.") ?>
  <?php echo gettext ("Click ✗ to remove.") ?>
</p>
<h3><?php echo gettext ("Available resources") ?></h3>
<p>
<?php echo gettext ("Internal Bibles:") ?>
<?php foreach ($this->bibles as $offset => $bible) { ?>
  <?php if ($offset) echo " | " ?>
  <a href="?add=<?php echo $bible ?>"><?php echo $bible ?></a>
<?php } ?>
</p>
<p>
<?php echo gettext ("External resources:") ?>
<?php foreach ($this->externals as $offset => $external) { ?>
  <?php if ($offset) echo " | " ?>
  <a href="?add=<?php echo $external ?>"><?php echo $external ?></a>
<?php } ?>
</p>
<script type="text/javascript" src="settings.js"></script>
