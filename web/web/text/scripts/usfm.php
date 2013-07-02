<form action="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&bible=<?php echo $this->bible ?>&book=<?php echo $this->book ?>&chapter=<?php echo $this->chapter ?>&usfmsavetext=" name="form" method="post">
<p><textarea name="data"><?php echo $this->chapter_data ?></textarea></p>
<p><input type="submit" name="submit" value=<?php echo gettext ("Save") ?> /></p>
</form>
