<h1><?php echo gettext ("Edit Notes Display") ?></h1>
<p>
<?php echo gettext ("Select notes that refer to:") ?>
  <a <?php if ($this->passageselector == 3) { ?>class="active"<?php } ?> href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnotespassageselector=3"><?php echo gettext ("any passage") ?></a>
  |
  <a <?php if ($this->passageselector == 0) { ?>class="active"<?php } ?> href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnotespassageselector=0"><?php echo gettext ("the current verse") ?></a>
  |
  <a <?php if ($this->passageselector == 1) { ?>class="active"<?php } ?> href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnotespassageselector=1"><?php echo gettext ("the current chapter") ?></a>
  |
  <a <?php if ($this->passageselector == 2) { ?>class="active"<?php } ?> href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnotespassageselector=2"><?php echo gettext ("the current book") ?></a>
</p>
<p>
<?php echo gettext ("Select notes that have been edited:") ?>
  <a <?php if ($this->editselector == 0) { ?>class="active"<?php } ?> href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnoteseditselector=0"><?php echo gettext ("at any time") ?></a>
  |
  <a <?php if ($this->editselector == 1) { ?>class="active"<?php } ?> href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnoteseditselector=1"><?php echo gettext ("during the last 30 days") ?></a>
  |
  <a <?php if ($this->editselector == 2) { ?>class="active"<?php } ?> href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnoteseditselector=2"><?php echo gettext ("during the last 7 days") ?></a>
  |
  <a <?php if ($this->editselector == 3) { ?>class="active"<?php } ?> href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnoteseditselector=3"><?php echo gettext ("since yesterday") ?></a>
  |
  <a <?php if ($this->editselector == 4) { ?>class="active"<?php } ?> href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnoteseditselector=4"><?php echo gettext ("today") ?></a>
</p>
<p>
<?php echo gettext ("Select notes that have not been edited for:") ?>
  <a <?php if ($this->noneditselector == 0) { ?>class="active"<?php } ?> href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnotesnoneditselector=0"><?php echo gettext ("any time") ?></a>
  |
  <a <?php if ($this->noneditselector == 1) { ?>class="active"<?php } ?> href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnotesnoneditselector=1"><?php echo gettext ("a day") ?></a>
  |
  <a <?php if ($this->noneditselector == 2) { ?>class="active"<?php } ?> href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnotesnoneditselector=2"><?php echo gettext ("two days") ?></a>
  |
  <a <?php if ($this->noneditselector == 3) { ?>class="active"<?php } ?> href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnotesnoneditselector=3"><?php echo gettext ("a week") ?></a>
  |
  <a <?php if ($this->noneditselector == 4) { ?>class="active"<?php } ?> href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnotesnoneditselector=4"><?php echo gettext ("a month") ?></a>
  |
  <a <?php if ($this->noneditselector == 5) { ?>class="active"<?php } ?> href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnotesnoneditselector=5"><?php echo gettext ("a year") ?></a>
</p>
<p>
<?php echo gettext ("Select notes that have a certain status:") ?>
  <a <?php if ($this->statusselector == "") { ?>class="active"<?php } ?> href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnotesstatusselector="><?php echo gettext ("Any") ?></a>
  <?php foreach ($this->statusids as $offset => $statusid) { ?>
    |
    <a <?php if ($this->statusselector == $statusid) { ?>class="active"<?php } ?> href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnotesstatusselector=<?php echo $this->statusids[$offset] ?>"><?php echo $this->statuslocs[$offset] ?></a>
  <?php } ?>
</p>
<p>
<?php echo gettext ("Select notes that belong to:") ?>
  <a <?php if ($this->bibleselector == "") { ?>class="active"<?php } ?> href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnotesbibleselector="><?php echo gettext ("any Bible") ?></a>
  <?php foreach ($this->bibles as $offset => $bible) { ?>
    |
    <a <?php if ($this->bibleselector == $bible) { ?>class="active"<?php } ?> href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnotesbibleselector=<?php echo $this->bibles[$offset] ?>"><?php echo $this->bibles[$offset] ?></a>
  <?php } ?>
</p>
<p>
<?php echo gettext ("Select notes that have been assigned to somebody:") ?>
  <a <?php if ($this->assignmentselector == "") { ?>class="active"<?php } ?> href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnotesassignmentselector="><?php echo gettext ("do not care about this") ?></a>
  <?php foreach ($this->assignees as $offset => $assignee) { ?>
    |
    <a <?php if ($this->assignmentselector == $assignee) { ?>class="active"<?php } ?> href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnotesassignmentselector=<?php echo $this->assignees[$offset] ?>"><?php echo $this->assignees[$offset] ?></a>
  <?php } ?>
  <?php if ($this->nonexistingassignee) { ?>
    <?php echo gettext ("Currently looking for notes assigned to:") ?> <?php echo $this->assignmentselector ?>
  <?php } ?>
</p>
<p>
<?php echo gettext ("Select notes by subscription:") ?>
  <a <?php if ($this->subscriptionselector == 0) { ?>class="active"<?php } ?> href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnotessubscriptionselector=0"><?php echo gettext ("do not care about this") ?></a>
  |
  <a <?php if ($this->subscriptionselector == 1) { ?>class="active"<?php } ?> href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnotessubscriptionselector=1"><?php echo gettext ("notes I am subscribed to") ?></a>
</p>
<?php echo gettext ("Select notes that have a severity of:") ?>
  <a <?php if ($this->severityselector == -1) { ?>class="active"<?php } ?> href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnotesseverityselector=-1"><?php echo gettext ("Any") ?></a>
  <?php foreach ($this->severities as $offset => $severity) { ?>
    |
    <a <?php if ($this->severityselector == $severity[0]) { ?>class="active"<?php } ?> href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnotesseverityselector=<?php echo $this->severities[$offset][0] ?>"><?php echo $this->severities[$offset][1] ?></a>
  <?php } ?>
</p>
</p>
<form action="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnotestextselector=1" name="text" method="post">
<?php echo gettext ("Select notes that contain text:") ?>
  <a <?php if ($this->textselector == 0) { ?>class="active"<?php } ?> href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnotestextselector=0"><?php echo gettext ("Any") ?></a>
  |
  <a <?php if ($this->textselector == 1) { ?>class="active"<?php } ?> href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnotestextselector=1"><?php echo gettext ("Specific text or words:") ?></a>
  <?php if ($this->textselector == 1) { ?>
      <input type="text" name="text" value="<?php echo $this->searchtext ?>" />
      <input type="submit" name="save" value=<?php echo gettext ("Save") ?> />
  <?php } ?>
</form>
</p>
<br>
<p><?php echo gettext ("Number of notes selected:") ?> <?php echo $this->totalcount ?></p>
<br>
<?php echo gettext ("Passage inclusion:") ?>
  <a <?php if ($this->passageinclusionselector == 0) { ?>class="active"<?php } ?> href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnotespassageinclusionyselector=0"><?php echo gettext ("Do not include the passage text") ?></a>
  <a <?php if ($this->passageinclusionselector == 1) { ?>class="active"<?php } ?> href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnotespassageinclusionyselector=1"><?php echo gettext ("Include the passage text") ?></a>
</p>
<?php echo gettext ("Text inclusion:") ?>
  <a <?php if ($this->textinclusionselector == 0) { ?>class="active"<?php } ?> href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnotestextinclusionyselector=0"><?php echo gettext ("Do not include the note text") ?></a>
  <a <?php if ($this->textinclusionselector == 1) { ?>class="active"<?php } ?> href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&consultationnotestextinclusionyselector=1"><?php echo gettext ("Include the note text") ?></a>
</p>
<h3>
<?php echo gettext ("Ok, I have finished") ?>,
<a href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&displaynoteslist="><?php echo gettext ("display the notes list") ?></a>
<?php if ($this->totalcount > 50) { ?>
  | <?php echo gettext ("or") ?> |
  <a href="<?php echo $this->caller ?>?session=<?php echo $this->session ?>&showallconsultationnotes="><?php echo gettext ("display all selected notes for this once") ?></a>
<?php } ?>
</h3>
