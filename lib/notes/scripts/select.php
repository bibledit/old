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
<h1>gettext("Select Notes")</h1>
<p>
gettext("Select notes that refer to:")
  <a <?php if ($this->passageselector == 3) {class="active"<?php } href="select?passageselector=3">gettext("any passage")</a>
  |
  <a <?php if ($this->passageselector == 0) {class="active"<?php } href="select?passageselector=0">gettext("the current verse")</a>
  |
  <a <?php if ($this->passageselector == 1) {class="active"<?php } href="select?passageselector=1">gettext("the current chapter")</a>
  |
  <a <?php if ($this->passageselector == 2) {class="active"<?php } href="select?passageselector=2">gettext("the current book")</a>
</p>
<p>
gettext("Select notes that have been edited:")
  <a <?php if ($this->editselector == 0) {class="active"<?php } href="select?editselector=0">gettext("at any time")</a>
  |
  <a <?php if ($this->editselector == 1) {class="active"<?php } href="select?editselector=1">gettext("during the last 30 days")</a>
  |
  <a <?php if ($this->editselector == 2) {class="active"<?php } href="select?editselector=2">gettext("during the last 7 days")</a>
  |
  <a <?php if ($this->editselector == 3) {class="active"<?php } href="select?editselector=3">gettext("since yesterday")</a>
  |
  <a <?php if ($this->editselector == 4) {class="active"<?php } href="select?editselector=4">gettext("today")</a>
</p>
<p>
gettext("Select notes that have not been edited for:")
  <a <?php if ($this->noneditselector == 0) {class="active"<?php } href="select?noneditselector=0">gettext("any time")</a>
  |
  <a <?php if ($this->noneditselector == 1) {class="active"<?php } href="select?noneditselector=1">gettext("a day")</a>
  |
  <a <?php if ($this->noneditselector == 2) {class="active"<?php } href="select?noneditselector=2">gettext("two days")</a>
  |
  <a <?php if ($this->noneditselector == 3) {class="active"<?php } href="select?noneditselector=3">gettext("a week")</a>
  |
  <a <?php if ($this->noneditselector == 4) {class="active"<?php } href="select?noneditselector=4">gettext("a month")</a>
  |
  <a <?php if ($this->noneditselector == 5) {class="active"<?php } href="select?noneditselector=5">gettext("a year")</a>
</p>
<p>
gettext("Select notes that have a certain status:")
  <a <?php if ($this->statusselector == "") {class="active"<?php } href="select?statusselector=">gettext("Any")</a>
  <?php for ($this->statusids as $offset => $statusid) {
    |
    <a <?php if ($this->statusselector == $statusid) {class="active"<?php } href="select?statusselector=$this->statusids[$offset]">$this->statuslocs[$offset]</a>
  <?php }
</p>
<p>
gettext("Select notes that belong to:")
  <a <?php if ($this->bibleselector == "") {class="active"<?php } href="select?bibleselector=">gettext("any Bible")</a>
  <?php for ($this->bibles as $offset => $bible) {
    |
    <a <?php if ($this->bibleselector == $bible) {class="active"<?php } href="select?bibleselector=$this->bibles[$offset]">$this->bibles[$offset]</a>
  <?php }
</p>
<p>
gettext("Select notes that have been assigned to somebody:")
  <a <?php if ($this->assignmentselector == "") {class="active"<?php } href="select?assignmentselector=">gettext("do not care about this")</a>
  <?php for ($this->assignees as $offset => $assignee) {
    |
    <a <?php if ($this->assignmentselector == $assignee) {class="active"<?php } href="select?assignmentselector=$this->assignees[$offset]">$this->assignees[$offset]</a>
  <?php }
  <?php if ($this->nonexistingassignee) {
    gettext("Currently looking for notes assigned to:") $this->assignmentselector
  <?php }
</p>
<p>
gettext("Select notes by subscription:")
  <a <?php if ($this->subscriptionselector == 0) {class="active"<?php } href="select?subscriptionselector=0">gettext("do not care about this")</a>
  |
  <a <?php if ($this->subscriptionselector == 1) {class="active"<?php } href="select?subscriptionselector=1">gettext("notes I am subscribed to")</a>
</p>
gettext("Select notes that have a severity of:")
  <a <?php if ($this->severityselector == -1) {class="active"<?php } href="select?severityselector=-1">gettext("Any")</a>
  <?php for ($this->severities as $offset => $severity) {
    |
    <a <?php if ($this->severityselector == $severity[0]) {class="active"<?php } href="select?severityselector=$this->severities[$offset][0]">$this->severities[$offset][1]</a>
  <?php }
</p>
</p>
<form action="select?textselector=1" name="text" method="post">
gettext("Select notes that contain text:")
  <a <?php if ($this->textselector == 0) {class="active"<?php } href="select?textselector=0">gettext("Any")</a>
  |
  <a <?php if ($this->textselector == 1) {class="active"<?php } href="select?textselector=1">gettext("Specific text or words:")</a>
  <?php if ($this->textselector == 1) {
      <input type="text" name="text" value="$this->searchtext" />
      <input type="submit" name="save" value=gettext("Save") />
  <?php }
</form>
</p>
<br>
<p>gettext("Number of notes selected:") $this->count</p>
<br>
gettext("Passage inclusion:")
  <a <?php if ($this->passageinclusionselector == 0) {class="active"<?php } href="select?passageinclusionyselector=0">gettext("Do not include the passage text")</a>
  <a <?php if ($this->passageinclusionselector == 1) {class="active"<?php } href="select?passageinclusionyselector=1">gettext("Include the passage text")</a>
</p>
gettext("Text inclusion:")
  <a <?php if ($this->textinclusionselector == 0) {class="active"<?php } href="select?textinclusionyselector=0">gettext("Do not include the note text")</a>
  <a <?php if ($this->textinclusionselector == 1) {class="active"<?php } href="select?textinclusionyselector=1">gettext("Include the note text")</a>
</p>
<h3><a href="index">gettext("Display the notes list")</a></h3>
<script type="text/javascript" src="poll.js?##VERSION##"></script>
