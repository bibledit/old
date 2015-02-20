<?php
/*
Copyright (©) 2003-2015 Teus Benschop.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
?>
<h1>translate("Select Notes")</h1>
<p>
translate("Select notes that refer to:")
  <a <?php if ($this->passageselector == 3) {class="active"<?php } href="select?passageselector=3">translate("any passage")</a>
  |
  <a <?php if ($this->passageselector == 0) {class="active"<?php } href="select?passageselector=0">translate("the current verse")</a>
  |
  <a <?php if ($this->passageselector == 1) {class="active"<?php } href="select?passageselector=1">translate("the current chapter")</a>
  |
  <a <?php if ($this->passageselector == 2) {class="active"<?php } href="select?passageselector=2">translate("the current book")</a>
</p>
<p>
translate("Select notes that have been edited:")
  <a <?php if ($this->editselector == 0) {class="active"<?php } href="select?editselector=0">translate("at any time")</a>
  |
  <a <?php if ($this->editselector == 1) {class="active"<?php } href="select?editselector=1">translate("during the last 30 days")</a>
  |
  <a <?php if ($this->editselector == 2) {class="active"<?php } href="select?editselector=2">translate("during the last 7 days")</a>
  |
  <a <?php if ($this->editselector == 3) {class="active"<?php } href="select?editselector=3">translate("since yesterday")</a>
  |
  <a <?php if ($this->editselector == 4) {class="active"<?php } href="select?editselector=4">translate("today")</a>
</p>
<p>
translate("Select notes that have not been edited for:")
  <a <?php if ($this->noneditselector == 0) {class="active"<?php } href="select?noneditselector=0">translate("any time")</a>
  |
  <a <?php if ($this->noneditselector == 1) {class="active"<?php } href="select?noneditselector=1">translate("a day")</a>
  |
  <a <?php if ($this->noneditselector == 2) {class="active"<?php } href="select?noneditselector=2">translate("two days")</a>
  |
  <a <?php if ($this->noneditselector == 3) {class="active"<?php } href="select?noneditselector=3">translate("a week")</a>
  |
  <a <?php if ($this->noneditselector == 4) {class="active"<?php } href="select?noneditselector=4">translate("a month")</a>
  |
  <a <?php if ($this->noneditselector == 5) {class="active"<?php } href="select?noneditselector=5">translate("a year")</a>
</p>
<p>
translate("Select notes that have a certain status:")
  <a <?php if ($this->statusselector == "") {class="active"<?php } href="select?statusselector=">translate("Any")</a>
  <?php for ($this->statusids as $offset => $statusid) {
    |
    <a <?php if ($this->statusselector == $statusid) {class="active"<?php } href="select?statusselector=$this->statusids[$offset]">$this->statuslocs[$offset]</a>
  <?php }
</p>
<p>
translate("Select notes that belong to:")
  <a <?php if ($this->bibleselector == "") {class="active"<?php } href="select?bibleselector=">translate("any Bible")</a>
  <?php for ($this->bibles as $offset => $bible) {
    |
    <a <?php if ($this->bibleselector == $bible) {class="active"<?php } href="select?bibleselector=$this->bibles[$offset]">$this->bibles[$offset]</a>
  <?php }
</p>
<p>
translate("Select notes that have been assigned to somebody:")
  <a <?php if ($this->assignmentselector == "") {class="active"<?php } href="select?assignmentselector=">translate("do not care about this")</a>
  <?php for ($this->assignees as $offset => $assignee) {
    |
    <a <?php if ($this->assignmentselector == $assignee) {class="active"<?php } href="select?assignmentselector=$this->assignees[$offset]">$this->assignees[$offset]</a>
  <?php }
  <?php if ($this->nonexistingassignee) {
    translate("Currently looking for notes assigned to:") $this->assignmentselector
  <?php }
</p>
<p>
translate("Select notes by subscription:")
  <a <?php if ($this->subscriptionselector == 0) {class="active"<?php } href="select?subscriptionselector=0">translate("do not care about this")</a>
  |
  <a <?php if ($this->subscriptionselector == 1) {class="active"<?php } href="select?subscriptionselector=1">translate("notes I am subscribed to")</a>
</p>
translate("Select notes that have a severity of:")
  <a <?php if ($this->severityselector == -1) {class="active"<?php } href="select?severityselector=-1">translate("Any")</a>
  <?php for ($this->severities as $offset => $severity) {
    |
    <a <?php if ($this->severityselector == $severity[0]) {class="active"<?php } href="select?severityselector=$this->severities[$offset][0]">$this->severities[$offset][1]</a>
  <?php }
</p>
</p>
<form action="select?textselector=1" name="text" method="post">
translate("Select notes that contain text:")
  <a <?php if ($this->textselector == 0) {class="active"<?php } href="select?textselector=0">translate("Any")</a>
  |
  <a <?php if ($this->textselector == 1) {class="active"<?php } href="select?textselector=1">translate("Specific text or words:")</a>
  <?php if ($this->textselector == 1) {
      <input type="text" name="text" value="$this->searchtext" />
      <input type="submit" name="save" value=translate("Save") />
  <?php }
</form>
</p>
<br>
<p>translate("Number of notes selected:") $this->count</p>
<br>
translate("Passage inclusion:")
  <a <?php if ($this->passageinclusionselector == 0) {class="active"<?php } href="select?passageinclusionyselector=0">translate("Do not include the passage text")</a>
  <a <?php if ($this->passageinclusionselector == 1) {class="active"<?php } href="select?passageinclusionyselector=1">translate("Include the passage text")</a>
</p>
translate("Text inclusion:")
  <a <?php if ($this->textinclusionselector == 0) {class="active"<?php } href="select?textinclusionyselector=0">translate("Do not include the note text")</a>
  <a <?php if ($this->textinclusionselector == 1) {class="active"<?php } href="select?textinclusionyselector=1">translate("Include the note text")</a>
</p>
<h3><a href="index">translate("Display the notes list")</a></h3>
<script type="text/javascript" src="poll.js?##VERSION##"></script>
