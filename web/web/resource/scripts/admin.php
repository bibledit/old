<h2><?php echo gettext ("External resources") ?></h2>
<?php foreach ($this->resources as $resource) { ?>
  <p><a href="?delete=<?php echo $resource ?>" class="deleteresource"> ✗ </a><a href="edit.php?name=<?php echo $resource ?>"> ✎ </a><?php echo $resource ?></p>
<?php } ?>
<br>
<form action="admin.php" name="form" method="post">
  <input type="text" name="entry" maxlength="300"  />
  <input type="submit" name="submit" value=<?php echo gettext ("Add") ?> />
</form>
<div id="dialog-confirm" title="<?php echo gettext ("Remove the resource?") ?>" style="display:none;">
  <p>
    <span class="ui-icon ui-icon-alert" style="float: left; margin: 0 7px 20px 0;"></span>
    <?php echo gettext ("Are you sure?") ?>
  </p>
</div>
<br>
<p><?php echo gettext ("You can add, edit, and delete resources.") ?></p>
<p>
  <?php echo gettext ("Bibledit-Web comes with a set of default scripts.") ?>
  <?php echo gettext ("Changes in them will be reverted during upgrade of Bibledit-Web.") ?>
</p>
<link rel="stylesheet" href="../jquery/smoothness/jquery-ui-1.10.3.css" />
<script type="text/javascript" src="admin.js"></script>
