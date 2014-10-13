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
<h1><?php echo Locale_Translate::_("Test read access") ?></h1>

<p><?php echo Locale_Translate::_("Bible") ?>: <?php echo $this->object ?></p>

<p><?php echo Locale_Translate::_("Repository URL") ?>: <?php echo $this->url ?></p>

<p><?php echo Locale_Translate::_("Bibledit-Web attempted to read data from this repository. This is the result:") ?></p>

<p><code><?php echo $this->read_access_result ?></code></p>

<p class="error"><?php echo $this->error_message ?></p>
<p class="success"><?php echo $this->success_message ?></p>

<p><?php echo Locale_Translate::_("The shell command that was used is:") ?> <code><?php echo $this->command ?></code></p>

<p><?php echo Locale_Translate::_("If you'd like to retry, after updating the URL, press button Submit.") ?></p>
<form action="collaboration_repo_read.php?object=<?php echo $this->object ?>" name="url" method="post">
  <p><input type="text" name="urlvalue" value="<?php echo $this->url ?>" /></p>
  <p><input type="submit" name="url" value=<?php echo Locale_Translate::_("Submit") ?> /></p>
</form>

<h2><a href="collaboration_repo_clone.php?object=<?php echo $this->object ?>"><?php echo Locale_Translate::_("Next page") ?></a></h2>
