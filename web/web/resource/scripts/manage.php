<h2><?php echo gettext ("USFM Resources") ?></h2>
<?php foreach ($this->resources as $resource) { ?>
  <p>
    <a href="?delete=<?php echo $resource ?>" class="deleteresource" title="<?php echo gettext ("Remove") ?>">
    ✗ 
    </a>
    <a href="?convert=<?php echo $resource ?>" class="convertresource" title="<?php echo gettext ("Convert") ?>">
    ♻
    </a>
    <?php echo $resource ?>
  </p>
<?php } ?>
<div id="confirm-delete" title="<?php echo gettext ("Remove the resource?") ?>" style="display:none;">
  <p>
    <span class="ui-icon ui-icon-alert" style="float: left; margin: 0 7px 20px 0;"></span>
    <?php echo gettext ("Are you sure?") ?>
  </p>
</div>
<div id="confirm-convert" title="<?php echo gettext ("Convert the resource?") ?>" style="display:none;">
  <p>
    <span class="ui-icon ui-icon-alert" style="float: left; margin: 0 7px 20px 0;"></span>
    <?php echo gettext ("Are you sure to convert the resource to a Bible?") ?>
  </p>
</div>
<br>
<p><?php echo gettext ("To create a USFM resource, go to the Bibles, and convert it to a USFM resource.") ?></p>
<p><?php echo gettext ("You can delete a USFM resource.") ?></p>
<p>
  <?php echo gettext ("You can convert a USFM resource back to a Bible.") ?>
  <?php echo gettext ("Progress will be visible in the logbook.") ?>
</p>
<link rel="stylesheet" href="../jquery/smoothness/jquery-ui-1.10.3.css" />
<script type="text/javascript" src="manage.js"></script>
