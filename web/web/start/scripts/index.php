<?php  
// 6: Administration.
// 5: Management.
// 4: Translation.
// 3: Consultation.
// 2: Membership.
?>
<?php if ($this->level >= 6) { ?>
  <p><a href="../administration/index.php"><?php echo gettext ("Administration") ?></a></p>
<?php } ?>
<?php if ($this->level >= 5) { ?>
  <p><a href="../manage/index.php"><?php echo gettext ("Management") ?></a></p>
<?php } ?>
<?php if ($this->level >= 4) { ?>
  <p><a href="../translate/index.php"><?php echo gettext ("Translation") ?></a></p>
<?php } ?>
<?php if ($this->level >= 3) { ?>
  <p><a href="../notes/index.php"><?php echo gettext ("Notes") ?></a></p>
  <p><a href="../resource/index.php"><?php echo gettext ("Resources") ?></a></p>
  <p><a href="../consultations/changes.php"><?php echo gettext ("Changes") ?></a></p>
<?php } ?>
<?php if ($this->level >= 2) { ?>
  <p><a href="../members/index.php"><?php echo gettext ("Members") ?></a></p>
<?php } ?>
<p><a href="../help/index.php"><?php echo gettext ("Help") ?></a></p>


