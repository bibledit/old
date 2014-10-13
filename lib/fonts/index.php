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


require_once ("../bootstrap/bootstrap.php");
page_access_level (Filter_Roles::MANAGER_LEVEL);


// Delete a font if it is not in use.
@$delete = $_GET['delete'];
if (isset ($delete)) {
  $font = basename ($delete);
  $font_in_use = false;
  $database_bibles = Database_Bibles::getInstance ();
  $database_config_bible = Database_Config_Bible::getInstance ();
  $bibles = $database_bibles->getBibles ();
  foreach ($bibles as $bible) {
    if ($font == $database_config_bible->getTextFont ($bible)) $font_in_use = true;
  }  
  if (!$font_in_use) {
    unlink ($font);
  } else {
    Assets_Page::error (Locale_Translate::_("The font could not be deleted because it is in use."));
  }
}


// Upload a font.
if (isset($_POST['upload'])) {
  // Upload may take time in case the file is large or the network is slow.
  ignore_user_abort (true);
  set_time_limit (0);
  $filename = $_FILES['data']['name'];
  $tmpfile = $_FILES['data']['tmp_name'];
  if (move_uploaded_file ($tmpfile, $filename)) {
    Assets_Page::success (Locale_Translate::_("The font has been uploaded."));
  } else {
    Assets_Page::error (Filter_Upload::error2text ($_FILES['data']['error']));
  }
}


$header = new Assets_Header (Locale_Translate::_("Fonts"));
$header->run ();


$view = new Assets_View (__FILE__);


$view->view->upload_max_filesize = ini_get ("upload_max_filesize");


$view->view->fonts = Fonts_Logic::getFonts ();


$view->render ("index.php");


Assets_Page::footer ();


?>
