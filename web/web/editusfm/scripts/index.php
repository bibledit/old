<div><?php echo $this->navigationHtml ?></div>
<pre id="usfmeditor" contenteditable="true"></pre>
<input type="hidden" name="bible" value="<?php echo $this->bible ?>">
<input type="hidden" name="book" value="<?php echo $this->book ?>">
<input type="hidden" name="chapter" value="<?php echo $this->chapter ?>">
<p><textarea name="data"><?php echo $this->chapter_data ?></textarea></p>
<script type="text/javascript" src="index.js"></script>
<?php echo $this->navigationCode; ?>
