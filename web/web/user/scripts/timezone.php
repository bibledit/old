<?php
/*
Copyright (©) 2003-2013 Teus Benschop.

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
<h1><?php echo gettext ("Timezone") ?></h1>
<p><?php echo gettext ("Here you can set up your own timezone.") ?></p>
<p><?php echo gettext ("All dates and times will then be shown as relevant to your timezone.") ?></p>
<?php if ($this->timezone != "") { ?>
  <p><?php echo gettext ("Current timezone:") ?> <?php echo $this->timezone ?>.</p>
<?php } else { ?>
  <p><?php echo gettext ("Your timezone has not yet been set.") ?></p>
<?php } ?>
<p><a href="timezone.php?timezone="><?php echo gettext ("Change the timezone.") ?></a></p>
