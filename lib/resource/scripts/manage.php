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
<h2><?php echo Locale_Translate::_("USFM Resources") ?></h2>
<?php foreach ($this->resources as $resource) { ?>
  <p>
    <a href="?delete=<?php echo $resource ?>" class="deleteresource" title="<?php echo Locale_Translate::_("Remove") ?>">
    ✗
    </a>
    <a href="?convert=<?php echo $resource ?>" class="convertresource" title="<?php echo Locale_Translate::_("Convert") ?>">
    ♻
    </a>
    <?php echo $resource ?>
  </p>
<?php } ?>
<div id="confirm-delete" title="<?php echo Locale_Translate::_("Remove the resource?") ?>" style="display:none;">
  <p>
    <span class="ui-icon ui-icon-alert" style="float: left; margin: 0 7px 20px 0;"></span>
    <?php echo Locale_Translate::_("Are you sure?") ?>
  </p>
</div>
<div id="confirm-convert" title="<?php echo Locale_Translate::_("Convert the resource?") ?>" style="display:none;">
  <p>
    <span class="ui-icon ui-icon-alert" style="float: left; margin: 0 7px 20px 0;"></span>
    <?php echo Locale_Translate::_("Are you sure to convert the resource to a Bible?") ?>
  </p>
</div>
<br>
<p><?php echo Locale_Translate::_("To create a USFM resource, go to the Bibles, and convert it to a USFM resource.") ?></p>
<p><?php echo Locale_Translate::_("You can delete a USFM resource.") ?></p>
<p>
  <?php echo Locale_Translate::_("You can convert a USFM resource back to a Bible.") ?>
  <?php echo Locale_Translate::_("Progress will be visible in the Journal.") ?>
</p>
<link rel="stylesheet" href="../jquery/smoothness/jquery-ui-1.10.3.css" />
<script type="text/javascript" src="manage.js?<?php echo Config_Logic::version () ?>"></script>
