<?php
/*
Copyright (©) 2003-2014 Teus Benschop.

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
<h3><?php echo gettext("Print Resources") ?></h3>
<ol>
<li>
<p>
  <?php echo gettext("Resources to print:") ?>
  <a href="?add=">➕</a>
</p>
<div id="sortable">
<?php for ($this->resources as $resource) { ?>
  <p><a href="?remove=<?php echo $resource ?>"> ✗ </a> ⇕ <span class="drag"><?php echo $resource ?></span> ⇕ </p>
<?php } ?>
</div>
</li>
<br>  
<li>
<p>
  <?php echo gettext("Set range: ") ?>
  <?php echo gettext("From") ?>
  <a href="?frombook="><?php echo $this->from_book ?></a>
  <a href="?fromchapter="><?php echo $this->from_chapter ?></a>:<a href="?fromverse="><?php echo $this->from_verse ?></a>
  <?php echo gettext("to") ?>
  <a href="?tobook="><?php echo $this->to_book ?></a>
  <a href="?tochapter="><?php echo $this->to_chapter ?></a>:<a href="?toverse="><?php echo $this->to_verse ?></a>
  </p>
</li>
<br>  
<li>
<p><a href="?generate=" target="_blank"><?php echo gettext("Generate the document.") ?></a></p>
</li>
</ol>
<script type="text/javascript" src="print.js?<?php echo config_logic_version () ?>"></script>
