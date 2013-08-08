<p>
  <span><?php echo $this->navigationHtml ?></span>
  |
  <a href="select.php">[<?php echo gettext ("select") ?>]</a>
  <?php if ($this->level >= 5) { ?>
    |
    [<a href="bulk.php"><?php echo gettext ("update") ?></a>]
  <?php } ?>
  |
  <a href="create.php"><?php echo gettext ("Create note") ?></a>  
</p>
<br>
<div id="noteslist"><?php echo gettext ("Loading...") ?></div>
<script type="text/javascript" src="index.js"></script>
<?php echo $this->navigationCode; ?>
