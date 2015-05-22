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
page_access_level (Filter_Roles::TRANSLATOR_LEVEL);


Assets_Page::header (Locale_Translate::_("Font and text direction"));
$view = new Assets_View (__FILE__);


$database_config_bible = Database_Config_Bible::getInstance();


// The name of the Bible.
$bible = Access_Bible::clamp ($_GET['bible']);
$view->view->bible = Filter_Html::sanitize ($bible);


// Data submission.
if (isset($_POST['submit'])) {

  $font = $_POST ["font"];
  $font = trim ($font);
  $database_config_bible->setTextFont ($bible, $font);
  
  $direction = $_POST ["direction"];
  $direction = Filter_CustomCSS::directionValue ($direction);

  $mode = $_POST ["mode"];
  $mode = Filter_CustomCSS::writingModeValue ($mode);

  $database_config_bible->setTextDirection ($bible, $mode * 10 + $direction);

  Assets_Page::error ("The information was saved.");

}


$font = $database_config_bible->getTextFont ($bible);
$view->view->font = $font;


$direction = $database_config_bible->getTextDirection ($bible);


$view->view->direction_none = Filter_CustomCSS::directionUnspecified ($direction);
$view->view->direction_ltr = Filter_CustomCSS::directionLeftToRight ($direction);
$view->view->direction_rtl = Filter_CustomCSS::directionRightToLeft ($direction);


$view->view->mode_none = Filter_CustomCSS::writingModeUnspecified ($direction);
$view->view->mode_tblr = Filter_CustomCSS::writingModeTopBottomLeftRight ($direction);
$view->view->mode_tbrl = Filter_CustomCSS::writingModeTopBottomRightLeft ($direction);
$view->view->mode_btlr = Filter_CustomCSS::writingModeBottomTopLeftRight ($direction);
$view->view->mode_btrl = Filter_CustomCSS::writingModeBottomTopRightLeft ($direction);


$class = Filter_CustomCSS::getClass ($bible);
$view->view->custom_class = $class;
$view->view->custom_css = Filter_CustomCSS::getCss ($class, Fonts_Logic::getFontPath ($font), $direction);

$view->render ("css.php");


Assets_Page::footer ();


?>
