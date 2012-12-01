<?php

require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);

$smarty = new Smarty_Bibledit (__FILE__);
$database_styles = Database_Styles::getInstance();
$styles_logic = Styles_Logic::getInstance ();

$sheet = $_GET['sheet'];
$smarty->assign ("sheet", Filter_Html::sanitize ($sheet));

$style = $_GET['style'];
$smarty->assign ("style", Filter_Html::sanitize ($style));

$standard_page_query = array ("sheet"=> $sheet, "style" => $style);

$marker_data = $database_styles->getMarkerData ($sheet, $style);

// The style's name.
$name = $marker_data['name'];
if (isset ($_GET['name'])) {
  $dialog_entry = new Dialog_Entry ($standard_page_query, gettext ("Please enter the name for the style"), $name, "name", "");
  die;
}
if (isset($_POST['name'])) {
  $name = $_POST['entry'];
  $database_styles->updateName ($sheet, $style, $name);
}
$smarty->assign ("name", Filter_Html::sanitize ($name));

// The style's info.
$info = $marker_data['info'];
if (isset ($_GET['info'])) {
  $dialog_entry = new Dialog_Entry ($standard_page_query, gettext ("Please enter the description for the style"), $info, "info", "");
  die;
}
if (isset($_POST['info'])) {
  $info = $_POST['entry'];
  $database_styles->updateInfo ($sheet, $style, $info);
}
$smarty->assign ("info", Filter_Html::sanitize ($info));

// The style's category.
$category = $marker_data['category'];
if (isset ($_GET['category'])) {
  $category = $_GET['category'];
  if ($category == "") {
    $dialog_list = new Dialog_List (array ("sheet", "style"), 
                                    gettext ("Would you like to change the category of this style?"), 
                                    gettext ("Here are the various categories:"), 
                                    gettext ("Please pick one."));
    $styles_logic = Styles_Logic::getInstance ();
    $dialog_list->add_row ($styles_logic->categoryText ("id"),   "&category=id");
    $dialog_list->add_row ($styles_logic->categoryText ("ith"),  "&category=ith");
    $dialog_list->add_row ($styles_logic->categoryText ("ipp"),  "&category=ipp");
    $dialog_list->add_row ($styles_logic->categoryText ("ioe"),  "&category=ioe");
    $dialog_list->add_row ($styles_logic->categoryText ("t"),    "&category=t");
    $dialog_list->add_row ($styles_logic->categoryText ("h"),    "&category=h");
    $dialog_list->add_row ($styles_logic->categoryText ("cv"),   "&category=cv");
    $dialog_list->add_row ($styles_logic->categoryText ("p"),    "&category=p");
    $dialog_list->add_row ($styles_logic->categoryText ("l"),    "&category=l");
    $dialog_list->add_row ($styles_logic->categoryText ("pe"),   "&category=pe");
    $dialog_list->add_row ($styles_logic->categoryText ("te"),   "&category=te");
    $dialog_list->add_row ($styles_logic->categoryText ("f"),    "&category=f");
    $dialog_list->add_row ($styles_logic->categoryText ("x"),    "&category=x");
    $dialog_list->add_row ($styles_logic->categoryText ("xsn"),  "&category=xsn");
    $dialog_list->add_row ($styles_logic->categoryText ("st"),   "&category=st");
    $dialog_list->add_row ($styles_logic->categoryText ("cs"),   "&category=cs");
    $dialog_list->add_row ($styles_logic->categoryText ("sb"),   "&category=sb");
    $dialog_list->add_row ($styles_logic->categoryText ("sf"),   "&category=sf");
    $dialog_list->add_row ($styles_logic->categoryText ("pm"),   "&category=pm");
    $dialog_list->add_row ($styles_logic->categoryText (""),     "&category=");
    $dialog_list->run ();
    die;
  } else {
    $database_styles->updateCategory ($sheet, $style, $category);
  }
}
$smarty->assign ("category", $styles_logic->categoryText($category));

// The style's type.
$type = $marker_data['type'];
if (isset ($_GET['type'])) {
  $type = $_GET['type'];
  if ($type == "") {
    $dialog_list = new Dialog_List (array ("sheet", "style"), 
                                    gettext ("Would you like to change the type of this style?"), 
                                    gettext ("Here are the various types:"), 
                                    gettext ("Please pick one."));
    $styles_logic = Styles_Logic::getInstance ();
    for ($i = 0; $i < 99; $i++) {
      $text = $styles_logic->typeText ($i);
      if (strlen ($text) > 2) {
        $dialog_list->add_row ($text, "&type=$i");
      }
    }
    $dialog_list->run ();
    die;
  } else {
    $database_styles->updateType ($sheet, $style, $type);
  }
}
$smarty->assign ("type", Filter_Html::sanitize ($type));
$smarty->assign ("type_text", $styles_logic->typeText($type));

// The style's subtype.
$subtype = $marker_data['subtype'];
if (isset ($_GET['subtype'])) {
  $subtype = $_GET['subtype'];
  if ($subtype == "") {
    $dialog_list = new Dialog_List (array ("sheet", "style"), gettext ("Would you like to change the sub type of this style?"), "", "");
    $database_styles = Database_Styles::getInstance();
    $styles_logic = Styles_Logic::getInstance ();
    $marker_data = $database_styles->getMarkerData ($sheet, $style);
    $type = $marker_data['type'];
    for ($i = 0; $i < 99; $i++) {
      $text = $styles_logic->subtypeText ($type, $i);
      if (strlen ($text) > 2) {
        $dialog_list->add_row ($text, "&subtype=$i");
      }
    }
    $dialog_list->run ();
    die;
  } else {
    $database_styles->updateSubType ($sheet, $style, $subtype);
  }
}
$smarty->assign ("subtype", Filter_Html::sanitize ($subtype));
$smarty->assign ("subtype_text", $styles_logic->subtypeText($type, $subtype));

// The fontsize.
$smarty->assign ("fontsize_relevant", $styles_logic->fontsizeIsRelevant ($type, $subtype));
$fontsize = $marker_data['fontsize'];
if (isset ($_GET['fontsize'])) {
  $dialog_entry = new Dialog_Entry ($standard_page_query, gettext ("Please enter a fontsize between 5 and 60 points"), $fontsize, "fontsize", gettext ("The value to enter is just a number, e.g. 12."));
  die;
}
if (isset($_POST['fontsize'])) {
  $fs = $_POST['entry'];
  $fs = Filter_Numeric::float_in_string ($fs);
  if (($fs >= 5) && ($fs <= 60)) {
    $fontsize = $fs;
    $database_styles->updateFontsize ($sheet, $style, $fontsize);
  }
}
$smarty->assign ("fontsize", Filter_Html::sanitize ($fontsize));

// Italics, bold, underline, small caps relevance.
$smarty->assign ("ibus_relevant", $styles_logic->italicBoldUnderLineSmallcapsAreRelevant ($type, $subtype));

// Italic.
$italic = $marker_data['italic'];
if (isset ($_GET['italic'])) {
  $italic = $_GET['italic'];
  if ($italic == "") {
    $dialog_list = new Dialog_List (array ("sheet", "style"), gettext ("Would you like to change whether this style is in italics?"), "", "");
    $database_styles = Database_Styles::getInstance();
    $styles_logic = Styles_Logic::getInstance ();
    $marker_data = $database_styles->getMarkerData ($sheet, $style);
    $last_value = ooitOn;
    if ($styles_logic->italicBoldUnderLineSmallcapsAreFull ($marker_data['type'], $marker_data['subtype']))
      $last_value = ooitToggle;
    for ($i = 0; $i <= $last_value; $i++) {
      $dialog_list->add_row ($styles_logic->OffOnInheritToggleText ($i), "&italic=$i");
    }
    $dialog_list->run ();
    die;
  } else {
    $database_styles->updateItalic ($sheet, $style, $italic);
  }
}
$smarty->assign ("italic", $styles_logic->OffOnInheritToggleText ($italic));

// Bold.
$bold   = $marker_data['bold'];
if (isset ($_GET['bold'])) {
  $bold = $_GET['bold'];
  if ($bold == "") {
    $dialog_list = new Dialog_List (array ("sheet", "style"), gettext ("Would you like to change whether this style is in bold?"), "", "");
    $database_styles = Database_Styles::getInstance();
    $styles_logic = Styles_Logic::getInstance ();
    $marker_data = $database_styles->getMarkerData ($sheet, $style);
    $last_value = ooitOn;
    if ($styles_logic->italicBoldUnderLineSmallcapsAreFull ($marker_data['type'], $marker_data['subtype']))
      $last_value = ooitToggle;
    for ($i = 0; $i <= $last_value; $i++) {
      $dialog_list->add_row ($styles_logic->OffOnInheritToggleText ($i), "&bold=$i");
    }
    $dialog_list->run ();
    die;
  } else {
    $database_styles->updateBold ($sheet, $style, $bold);
  }
}
$smarty->assign ("bold", $styles_logic->OffOnInheritToggleText ($bold));

// Underline.
$underline = $marker_data['underline'];
if (isset ($_GET['underline'])) {
  $underline = $_GET['underline'];
  if ($underline == "") {
    $dialog_list = new Dialog_List (array ("sheet", "style"), gettext ("Would you like to change whether this style is underlined?"), "", "");
    $database_styles = Database_Styles::getInstance();
    $styles_logic = Styles_Logic::getInstance ();
    $marker_data = $database_styles->getMarkerData ($sheet, $style);
    $last_value = ooitOn;
    if ($styles_logic->italicBoldUnderLineSmallcapsAreFull ($type, $subtype))
      $last_value = ooitToggle;
    for ($i = 0; $i <= $last_value; $i++) {
      $dialog_list->add_row ($styles_logic->OffOnInheritToggleText ($i), "&underline=$i");
    }
    $dialog_list->run ();
    die;
  } else {
    $database_styles->updateUnderline ($sheet, $style, $underline);
  }
}
$smarty->assign ("underline", $styles_logic->OffOnInheritToggleText ($underline));

// Small caps.
$smallcaps = $marker_data['smallcaps'];
if (isset ($_GET['smallcaps'])) {
  $smallcaps = $_GET['smallcaps'];
  if ($smallcaps == "") {
    $dialog_list = new Dialog_List (array ("sheet", "style"), gettext ("Would you like to change whether this style is in small caps?"), "", "");
    $database_styles = Database_Styles::getInstance();
    $styles_logic = Styles_Logic::getInstance ();
    $marker_data = $database_styles->getMarkerData ($sheet, $style);
    $last_value = ooitOn;
    if ($styles_logic->italicBoldUnderLineSmallcapsAreFull ($marker_data['type'], $marker_data['subtype']))
      $last_value = ooitToggle;
    for ($i = 0; $i <= $last_value; $i++) {
      $dialog_list->add_row ($styles_logic->OffOnInheritToggleText ($i), "&smallcaps=$i");
    }
    $dialog_list->run ();
    die;
  } else {
    $database_styles->updateSmallcaps ($sheet, $style, $smallcaps);
  }
}
$smarty->assign ("smallcaps", $styles_logic->OffOnInheritToggleText ($smallcaps));

// Superscript.
$smarty->assign ("superscript_relevant",  $styles_logic->superscriptIsRelevant ($type, $subtype));
$superscript = $marker_data['superscript'];
if (isset ($_GET['superscript'])) {
  $superscript = Filter_Bool::int ($_GET['superscript']);
  $database_styles->updateSuperscript ($sheet, $style, $superscript);
}
$smarty->assign ("superscript_value",  $styles_logic->OffOnInheritToggleText ($superscript));
$smarty->assign ("superscript_toggle", Filter_Bool::not ($superscript));

// Whether a list of the following paragraph treats are relevant.
$smarty->assign ("paragraph_treats_relevant", $styles_logic->paragraphTreatsAreRelevant ($type, $subtype));

// Text alignment.
$justification = $marker_data['justification'];
if (isset ($_GET['alignment'])) {
  $dialog_list = new Dialog_List (array ("sheet", "style"), gettext ("Would you like to change the text alignment of this style?"), "", "");
  $styles_logic = Styles_Logic::getInstance();
  for ($i = AlignmentLeft; $i <= AlignmentJustify; $i++) {
    $dialog_list->add_row ($styles_logic->alignmentText ($i), "&justification=$i");
  }
  $dialog_list->run ();
  die;  
}
if (isset ($_GET['justification'])) {
  $justification = $_GET['justification'];
  $database_styles->updateJustification ($sheet, $style, $justification);
}
$smarty->assign ("justification", $styles_logic->alignmentText ($justification));

// Space before paragraph.
$spacebefore = $marker_data['spacebefore'];
if (isset ($_GET['spacebefore'])) {
  $dialog_entry = new Dialog_Entry ($standard_page_query, gettext ("Please enter a space of between 0 and 100 mm before the paragraph"), $spacebefore, "spacebefore", gettext ("This is the space before, or in other words, above the paragraph. The value to enter is just a number, e.g. 0."));
  die;
}
if (isset($_POST['spacebefore'])) {
  $value = $_POST['entry'];
  $value = Filter_Numeric::float_in_string ($value);
  if (($value >= 0) && ($value <= 100)) {
    $spacebefore = $value;
    $database_styles->updateSpaceBefore ($sheet, $style, $spacebefore);
  }
}
$smarty->assign ("spacebefore", Filter_Html::sanitize ($spacebefore));

// Space after paragraph.
$spaceafter = $marker_data['spaceafter'];
if (isset ($_GET['spaceafter'])) {
  $dialog_entry = new Dialog_Entry ($standard_page_query, gettext ("Please enter a space of between 0 and 100 mm after the paragraph"), $spaceafter, "spaceafter", gettext ("This is the space after, or in other words, below the paragraph. The value to enter is just a number, e.g. 0."));
  die;
}
if (isset($_POST['spaceafter'])) {
  $value = $_POST['entry'];
  $value = Filter_Numeric::float_in_string ($value);
  if (($value >= 0) && ($value <= 100)) {
    $spaceafter = $value;
    $database_styles->updateSpaceAfter ($sheet, $style, $spaceafter);
  }
}
$smarty->assign ("spaceafter", Filter_Html::sanitize ($spaceafter));

// Left margin.
$leftmargin = $marker_data['leftmargin'];
if (isset ($_GET['leftmargin'])) {
  $dialog_entry = new Dialog_Entry ($standard_page_query, gettext ("Please enter a left margin of between -100 and 100 mm"), $leftmargin, "leftmargin", gettext ("This is the left margin of the paragraph. The value to enter is just a number, e.g. 0."));
  die;
}
if (isset($_POST['leftmargin'])) {
  $value = $_POST['entry'];
  $value = Filter_Numeric::float_in_string ($value);
  if (($value >= -100) && ($value <= 100)) {
    $leftmargin = $value;
    $database_styles->updateLeftMargin ($sheet, $style, $leftmargin);
  }
}
$smarty->assign ("leftmargin", Filter_Html::sanitize ($leftmargin));

// Right margin.
$rightmargin = $marker_data['rightmargin'];
if (isset ($_GET['rightmargin'])) {
  $dialog_entry = new Dialog_Entry ($standard_page_query, gettext ("Please enter a right margin of between -100 and 100 mm"), $rightmargin, "rightmargin", gettext ("This is the right margin of the paragraph. The value to enter is just a number, e.g. 0."));
  die;
}
if (isset($_POST['rightmargin'])) {
  $value = $_POST['entry'];
  $value = Filter_Numeric::float_in_string ($value);
  if (($value >= -100) && ($value <= 100)) {
    $rightmargin = $value;
    $database_styles->updateRightMargin ($sheet, $style, $rightmargin);
  }
}
$smarty->assign ("rightmargin", Filter_Html::sanitize ($rightmargin));

// First line indent.
$firstlineindent = $marker_data['firstlineindent'];
if (isset ($_GET['firstlineindent'])) {
  $dialog_entry = new Dialog_Entry ($standard_page_query, gettext ("Please enter a first line indent of between -100 and 100 mm"), $firstlineindent, "firstlineindent", gettext ("This is the indent of the first line of the the paragraph. The value to enter is just a number, e.g. 0."));
  die;
}
if (isset($_POST['firstlineindent'])) {
  $value = $_POST['entry'];
  $value = Filter_Numeric::float_in_string ($value);
  if (($value >= -100) && ($value <= 100)) {
    $firstlineindent = $value;
    $database_styles->updateFirstLineIndent ($sheet, $style, $firstlineindent);
  }
}
$smarty->assign ("firstlineindent", Filter_Html::sanitize ($firstlineindent));

// Columns spanning.
$smarty->assign ("columns_relevant", $styles_logic->columnsAreRelevant ($type, $subtype));
$spancolumns = $marker_data['spancolumns'];
if (isset ($_GET['spancolumns'])) {
  $spancolumns = Filter_Bool::int ($_GET['spancolumns']);
  $database_styles->updateSpanColumns ($sheet, $style, $spancolumns);
}
$smarty->assign ("spancolumns", $styles_logic->OffOnInheritToggleText ($spancolumns));
$smarty->assign ("spancolumns_toggle", Filter_Bool::not ($spancolumns));

// Color.
$smarty->assign ("color_relevant", $styles_logic->colorIsRelevant ($type, $subtype));
$color = $marker_data['color'];
if (isset ($_GET['color'])) {
  $value = $_GET['color'];
  if ($value == "") {
    $dialog_color = new Dialog_Color ($standard_page_query, gettext ("Please specify a new color"), "color");
    die;
  } else {
    $color = $value;
    $database_styles->updateColor ($sheet, $style, $color);
  }
}
$smarty->assign ("color", Filter_Html::sanitize ($color));

// Whether to print this style.
$smarty->assign ("print_relevant", $styles_logic->printIsRelevant ($type, $subtype));
$print = $marker_data['print'];
if (isset ($_GET['print'])) {
  $print = Filter_Bool::int ($_GET['print']);
  $database_styles->updatePrint ($sheet, $style, $print);
}
$smarty->assign ("print", $styles_logic->OffOnInheritToggleText ($print));
$smarty->assign ("print_toggle", Filter_Bool::not ($print));

// Userbool1
$smarty->assign ("userbool1_function", $styles_logic->getUserBool1Text ($styles_logic->getUserBool1Function ($type, $subtype)));
$userbool1 = $marker_data['userbool1'];
if (isset ($_GET['userbool1'])) {
  $userbool1 = Filter_Bool::int ($_GET['userbool1']);
  $database_styles->updateUserbool1 ($sheet, $style, $userbool1);
}
$smarty->assign ("userbool1_value",    $styles_logic->OffOnInheritToggleText ($userbool1));
$smarty->assign ("userbool1_toggle", Filter_Bool::not ($userbool1));

// Userbool2
$smarty->assign ("userbool2_function", $styles_logic->getUserBool2Text ($styles_logic->getUserBool2Function ($type, $subtype)));
$userbool2 = $marker_data['userbool2'];
if (isset ($_GET['userbool2'])) {
  $userbool2 = Filter_Bool::int ($_GET['userbool2']);
  $database_styles->updateUserbool2 ($sheet, $style, $userbool2);
}
$smarty->assign ("userbool2_value",    $styles_logic->OffOnInheritToggleText ($userbool2));
$smarty->assign ("userbool2_toggle", Filter_Bool::not ($userbool2));

// Userbool3
$smarty->assign ("userbool3_function", $styles_logic->getUserBool3Text ($styles_logic->getUserBool3Function ($type, $subtype)));
$userbool3 = $marker_data['userbool3'];
if (isset ($_GET['userbool3'])) {
  $userbool3 = Filter_Bool::int ($_GET['userbool3']);
  $database_styles->updateUserbool3 ($sheet, $style, $userbool3);
}
$smarty->assign ("userbool3_value",    $styles_logic->OffOnInheritToggleText ($userbool3));
$smarty->assign ("userbool3_toggle", Filter_Bool::not ($userbool3));

// Userint1.
$userint1 = $marker_data['userint1'];
switch ($styles_logic->getUserInt1Function ($type, $subtype)) {
  case UserInt1None : 
    break;
  case UserInt1NoteNumbering :
    $smarty->assign ("userint1_notenumbering", true);
    if (isset ($_GET['notenumbering'])) {
      $dialog_list = new Dialog_List (array ("sheet", "style"), gettext ("Would you like to change the numbering of the note?"), "", "");
      $styles_logic = Styles_Logic::getInstance();
      for ($i = NoteNumbering123; $i <= NoteNumberingUser; $i++) {
        $dialog_list->add_row ($styles_logic->noteNumberingText ($i), "&userint1=$i");
      }
      $dialog_list->run ();
      die;
    }
    if (isset ($_GET['userint1'])) {
      $userint1 = $_GET['userint1'];
      $database_styles->updateUserint1 ($sheet, $style, $userint1);
    }
    $smarty->assign ("userint1", $styles_logic->noteNumberingText ($userint1));
    break;
  case UserInt1TableColumnNumber :
    $smarty->assign ("userint1_columnnumber", true);
    if (isset ($_GET['userint1'])) {
      $dialog_entry = new Dialog_Entry ($standard_page_query, gettext ("Please enter a column number between 1 and 4"), $userint1, "userint1", gettext ("This is the column number for the style. The first columm is number 1."));
      die;
    }
    if (isset($_POST['userint1'])) {
      $value = $_POST['entry'];
      $value = Filter_Numeric::integer_in_string ($value);
      if (($value >= 1) && ($value <= 4)) {
        $userint1 = $value;
        $database_styles->updateUserint1 ($sheet, $style, $userint1);
      }
    }
    $smarty->assign ("userint1", $userint1);
    break;
}

// Userint2.
$userint2 = $marker_data['userint2'];
switch ($styles_logic->getUserInt2Function ($type, $subtype)) {
  case UserInt2None :
    break;
  case UserInt2NoteNumberingRestart :
    $smarty->assign ("userint2_notenumberingrestart", true);
    if (isset ($_GET['notenumberingrestart'])) {
      $dialog_list = new Dialog_List (array ("sheet", "style"), gettext ("Would you like to change when the note numbering restarts?"), "", "");
      $styles_logic = Styles_Logic::getInstance();
      for ($i = NoteRestartNumberingNever; $i <= NoteRestartNumberingEveryChapter; $i++) {
        $dialog_list->add_row ($styles_logic->noteRestartNumberingText ($i), "&userint2=$i");
      }
      $dialog_list->run ();
      die;
    }
    if (isset ($_GET['userint2'])) {
      $userint2 = $_GET['userint2'];
      $database_styles->updateUserint2 ($sheet, $style, $userint2);
    }
    $smarty->assign ("userint2", $styles_logic->noteRestartNumberingText ($userint2));
    break;
  case UserInt2EndnotePosition :
    $smarty->assign ("userint2_endnoteposition", true);
    if (isset ($_GET['endnoteposition'])) {
      $dialog_list = new Dialog_List (array ("sheet", "style"), gettext ("Would you like to change the position where to dump the endnotes?"), "", "");
      $styles_logic = Styles_Logic::getInstance();
      for ($i = EndNotePositionAfterBook; $i <= EndNotePositionAtMarker; $i++) {
        $dialog_list->add_row ($styles_logic->endNotePositionText ($i), "&userint2=$i");
      }
      $dialog_list->run ();
      die;
    }
    if (isset ($_GET['userint2'])) {
      $userint2 = $_GET['userint2'];
      $database_styles->updateUserint2 ($sheet, $style, $userint2);
    }
    $smarty->assign ("userint2", $styles_logic->endNotePositionText ($userint2));
    break;
}

// Userint3 not yet used.

// Userstring1.
$userstring1 = $marker_data['userstring1'];
switch ($styles_logic->getUserString1Function ($type, $subtype)) {
  case UserString1None :
    break;
  case UserString1NoteNumberingSequence :
    if ($userint1 == NoteNumberingUser) {
      $smarty->assign ("userstring1_numberingsequence", true);
      $userstring_question = gettext ("Please enter a new note numbering sequence");
      $userstring1_help = gettext ("This gives a sequence for numbering the notes. When for example § † * is entered, the numbering goes like §, †, *, §, †, *, and so forth. Any sequence of characters can be used. Spaces should separate the characters");
    }
    break;
  case UserString1WordListEntryAddition :
    $smarty->assign ("userstring1_wordlistaddition", true);
    $userstring1_question = gettext ("Please enter a new addition to the word list entry");
    $userstring1_help = gettext ("This given an optional string to be added after each definition in the body of text. In some Bibles the unusual words are marked with an asterisk and then explained in a glossary. If you would enter the asterisk here, or indeed any string, Bibledit-Web would include this in the exported documents.");
    break;
}
if (isset ($_GET['userstring1'])) {
  $dialog_entry = new Dialog_Entry ($standard_page_query, $userstring1_question, $userstring1, "userstring1", $userstring1_help);
  die;
}
if (isset($_POST['userstring1'])) {
  $userstring1 = $_POST['entry'];
  $database_styles->updateUserstring1 ($sheet, $style, $userstring1);
}
if ($userstring1 == "") $userstring1 = "--";
$smarty->assign ("userstring1", Filter_Html::sanitize ($userstring1));

// Userstring2
$userstring2 = $marker_data['userstring2'];
switch ($styles_logic->getUserString2Function ($type, $subtype)) {
  case UserString2None :
    break;
  case UserString2DumpEndnotesHere :
    if ($userint2 == EndNotePositionAtMarker) {
      $smarty->assign ("userstring2_dumpendnotes", true);
      $userstring2_question = gettext ("Please enter a marker at which the endnotes should be dumped");
      $userstring2_info = gettext ("The marker is to be given without the backslash, e.g. \"zendnotes\".");
    }
    break;
}
if (isset ($_GET['userstring2'])) {
  $dialog_entry = new Dialog_Entry ($standard_page_query, $userstring2_question, $userstring2, "userstring2", $userstring2_info);
  die;
}
if (isset($_POST['userstring2'])) {
  $userstring2 = $_POST['entry'];
  $database_styles->updateUserstring2 ($sheet, $style, $userstring2);
}
if ($userstring2 == "") $userstring2 = "--";
$smarty->assign ("userstring2", Filter_Html::sanitize ($userstring2));

// Userstring3 not yet used.

$smarty->display ("view.tpl");

?>