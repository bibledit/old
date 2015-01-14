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
<p><a href="note.php?id=$this->id">gettext("Go back to the note contents")</a></p>
<p>gettext("Summary"): $this->summary</p>
<p>
<?php if ($this->subscribed) {
gettext("You are subscribed to this note") <a href="?id=$this->id&unsubscribe=">[gettext ("unsubscribe")]</a>
<?php } else {
<a href="?id=$this->id&subscribe=">gettext("Subscribe to this note")</a>
<?php }
<p>gettext("The note has been assigned to"):
<?php for ($this->assignees as $offset => $assignee) {
  $this->assignees[$offset]
  <?php if ($this->level >= 5) {
    <a href="?id=$this->id&unassign=$this->assignees[$offset]">[gettext("unassign")]</a>
    |
  <?php }
<?php }
<?php if ($this->level >= 5) {
  <a href="assign-1.php?id=$this->id">[gettext("add assignee")]</a>
<?php }
</p>
<?php if ($this->assignee) {
  <p>
  gettext("This note has been assigned to you for you to take action on.")
  <a href="?id=$this->id&done=">gettext("I have done my part on it.")</a>
  </p>
<?php }
<p>
gettext("Status"):
<?php if ($this->level >= 4) {
  <a href="status-1.php?id=$this->id">$this->status</a>
<?php } else {
  $this->status
<?php }
</p>
<p>
  gettext("Verses"): $this->verses |
  <a href="verses.php?id=$this->id">[gettext("edit")]</a>
</p>
  <p>gettext("Severity"):
  <a href="severity-1.php?id=$this->id">$this->severity</a></p>
<p>
</p>
<p>
gettext("Bible"):
<?php if ($this->bible != "") {
  $this->bible
<?php } else {
  gettext("This is a general note, it does not apply to any specific Bible")
<?php }
  <a href="bible-1.php?id=$this->id">[gettext("change")]</a>
</p>
<p>gettext("Identifier"): $this->id</p>
</p>
<?php if ($this->level >= 5) {
  <p>
  <?php if ($this->marked) {
    gettext("The note will be deleted after it expires
    ")
    <a href="?id=$this->id&unmarkdel=">[gettext("cancel")]</a>
  <?php } else {
    <a href="?id=$this->id&markdel=">gettext("Mark the note for deletion after a week")</a>
  <?php }
  </p>
  <p><a href="?id=$this->id&delete=">gettext("Delete the note now")</a></p>
<?php }
<p class="error">$this->error</p>
<p class="success">$this->success</p>
<script type="text/javascript" src="poll.js?##VERSION##"></script>
