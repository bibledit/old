<div style="float:left; width:50%;">
<font size="-1" color="grey"><hr /></font>
<p><font size="-1" color="grey"><?php echo $this->noteCount ?> <?php echo gettext ("notes") ?></font></p>
<?php foreach ($this->noteUrls as $offset => $noteUrl) { ?>
  <p style="margin-top: 0.75em; margin-bottom: 0em"><a href="<?php echo $noteUrl ?>"><?php echo $this->noteTitles[$offset] ?></a></p>
  <?php echo $this->noteExcerpts[$offset] ?>
<?php } ?>
</div>
<div style="float:right; width:50%; ">
<font size="-1" color="grey"><hr /></font>
<p><font size="-1" color="grey"><?php echo $this->textCount ?> <?php echo gettext ("chapters") ?></font></p>
<?php foreach ($this->textUrls as $offset => $textUrl) { ?>
  <p style="margin-top: 0.75em; margin-bottom: 0em"><a href="<?php echo $textUrl ?>"><?php echo $this->textTitles[$offset] ?></a></p>
  <?php echo $this->textExcerpts[$offset] ?>
<?php } ?>
</div>
