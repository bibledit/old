<h2><?php echo $this->header ?></h2>
<p><?php echo $this->info_top ?></p>
<p>
<?php foreach ($this->book_ids as $offset => $book_id) { ?>
<a href="<?php echo $this->caller ?>&<?php echo $this->action ?>=<?php echo $this->book_ids[$offset] ?>"><?php echo $this->book_names[$offset] ?></a>
<?php } ?>
</p>
<p><?php echo $this->info_bottom ?></p>
<h3><a href="<?php echo $this->caller ?>"><?php echo gettext ("Cancel") ?></a></h3>
