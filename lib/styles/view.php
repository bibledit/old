<?php
// The style's type.
type = marker_data["type"];
if (isset (request->query["type"])) {
  type = request->query["type"];
  if (type == "") {
    dialog_list = new Dialog_List (array ("sheet", "style"),
                                    gettext("Would you like to change the type of this style?"),
                                    gettext("Here are the various types:"),
                                    gettext("Please pick one."));
    styles_logic = Styles_Logic::getInstance ();
    for (i = 0; i < 99; i++) {
      text = styles_logic.typeText (i);
      if (strlen (text) > 2) {
        dialog_list.add_row (text, "&type=i");
      }
    }
    dialog_list.run ();
    die;
  } else {
    if (write) database_styles.updateType (sheet, style, type);
  }
}
view.view.type = filter_string_sanitize_html (type);
view.view.type_text = styles_logic.typeText(type);

// The style's subtype.
subtype = marker_data["subtype"];
if (isset (request->query["subtype"])) {
  subtype = request->query["subtype"];
  if (subtype == "") {
    dialog_list = new Dialog_List (array ("sheet", "style"), gettext("Would you like to change the sub type of this style?"), "", "");
    database_styles = Database_Styles::getInstance();
    styles_logic = Styles_Logic::getInstance ();
    marker_data = database_styles.getMarkerData (sheet, style);
    type = marker_data["type"];
    for (i = 0; i < 99; i++) {
      text = styles_logic.subtypeText (type, i);
      if (strlen (text) > 2) {
        dialog_list.add_row (text, "&subtype=i");
      }
    }
    dialog_list.run ();
    die;
  } else {
    if (write) database_styles.updateSubType (sheet, style, subtype);
  }
}
view.view.subtype = filter_string_sanitize_html (subtype);
view.view.subtype_text = styles_logic.subtypeText(type, subtype);

// The fontsize.
view.view.fontsize_relevant = styles_logic.fontsizeIsRelevant (type, subtype);
fontsize = marker_data["fontsize"];
if (isset (request->query["fontsize"])) {
  dialog_entry = new Dialog_Entry (standard_page_query, gettext("Please enter a fontsize between 5 and 60 points"), fontsize, "fontsize", gettext ("The value to enter is just a number, e.g. 12."));
  die;
}
if (isset(_POST["fontsize"])) {
  fs = _POST["entry"];
  fs = Filter_Numeric::float_in_string (fs);
  if ((fs >= 5) && (fs <= 60)) {
    fontsize = fs;
    if (write) database_styles.updateFontsize (sheet, style, fontsize);
  }
}
view.view.fontsize = filter_string_sanitize_html (fontsize);

// Italics, bold, underline, small caps relevance.
view.view.ibus_relevant = styles_logic.italicBoldUnderLineSmallcapsAreRelevant (type, subtype);

// Italic.
italic = marker_data["italic"];
if (isset (request->query["italic"])) {
  italic = request->query["italic"];
  if (italic == "") {
    dialog_list = new Dialog_List (array ("sheet", "style"), gettext("Would you like to change whether this style is in italics?"), "", "");
    database_styles = Database_Styles::getInstance();
    styles_logic = Styles_Logic::getInstance ();
    marker_data = database_styles.getMarkerData (sheet, style);
    last_value = ooitOn;
    if (styles_logic.italicBoldUnderLineSmallcapsAreFull (marker_data["type"], marker_data["subtype"]))
      last_value = ooitToggle;
    for (i = 0; i <= last_value; i++) {
      dialog_list.add_row (styles_logic.OffOnInheritToggleText (i), "&italic=i");
    }
    dialog_list.run ();
    die;
  } else {
    if (write) database_styles.updateItalic (sheet, style, italic);
  }
}
view.view.italic =styles_logic.OffOnInheritToggleText (italic);

// Bold.
bold   = marker_data["bold"];
if (isset (request->query["bold"])) {
  bold = request->query["bold"];
  if (bold == "") {
    dialog_list = new Dialog_List (array ("sheet", "style"), gettext("Would you like to change whether this style is in bold?"), "", "");
    database_styles = Database_Styles::getInstance();
    styles_logic = Styles_Logic::getInstance ();
    marker_data = database_styles.getMarkerData (sheet, style);
    last_value = ooitOn;
    if (styles_logic.italicBoldUnderLineSmallcapsAreFull (marker_data["type"], marker_data["subtype"]))
      last_value = ooitToggle;
    for (i = 0; i <= last_value; i++) {
      dialog_list.add_row (styles_logic.OffOnInheritToggleText (i), "&bold=i");
    }
    dialog_list.run ();
    die;
  } else {
    if (write) database_styles.updateBold (sheet, style, bold);
  }
}
view.view.bold = styles_logic.OffOnInheritToggleText (bold);

// Underline.
underline = marker_data["underline"];
if (isset (request->query["underline"])) {
  underline = request->query["underline"];
  if (underline == "") {
    dialog_list = new Dialog_List (array ("sheet", "style"), gettext("Would you like to change whether this style is underlined?"), "", "");
    database_styles = Database_Styles::getInstance();
    styles_logic = Styles_Logic::getInstance ();
    marker_data = database_styles.getMarkerData (sheet, style);
    last_value = ooitOn;
    if (styles_logic.italicBoldUnderLineSmallcapsAreFull (type, subtype))
      last_value = ooitToggle;
    for (i = 0; i <= last_value; i++) {
      dialog_list.add_row (styles_logic.OffOnInheritToggleText (i), "&underline=i");
    }
    dialog_list.run ();
    die;
  } else {
    if (write) database_styles.updateUnderline (sheet, style, underline);
  }
}
view.view.underline = styles_logic.OffOnInheritToggleText (underline);

// Small caps.
smallcaps = marker_data["smallcaps"];
if (isset (request->query["smallcaps"])) {
  smallcaps = request->query["smallcaps"];
  if (smallcaps == "") {
    dialog_list = new Dialog_List (array ("sheet", "style"), gettext("Would you like to change whether this style is in small caps?"), "", "");
    database_styles = Database_Styles::getInstance();
    styles_logic = Styles_Logic::getInstance ();
    marker_data = database_styles.getMarkerData (sheet, style);
    last_value = ooitOn;
    if (styles_logic.italicBoldUnderLineSmallcapsAreFull (marker_data["type"], marker_data["subtype"]))
      last_value = ooitToggle;
    for (i = 0; i <= last_value; i++) {
      dialog_list.add_row (styles_logic.OffOnInheritToggleText (i), "&smallcaps=i");
    }
    dialog_list.run ();
    die;
  } else {
    if (write) database_styles.updateSmallcaps (sheet, style, smallcaps);
  }
}
view.view.smallcaps = styles_logic.OffOnInheritToggleText (smallcaps);

// Superscript.
view.view.superscript_relevant = styles_logic.superscriptIsRelevant (type, subtype);
superscript = marker_data["superscript"];
if (isset (request->query["superscript"])) {
  superscript = Filter_Bool::int (request->query["superscript"]);
  if (write) database_styles.updateSuperscript (sheet, style, superscript);
}
view.view.superscript_value = styles_logic.OffOnInheritToggleText (superscript);
view.view.superscript_toggle = Filter_Bool::not (superscript);

// Whether a list of the following paragraph treats are relevant.
view.view.paragraph_treats_relevant = styles_logic.paragraphTreatsAreRelevant (type, subtype);

// Text alignment.
justification = marker_data["justification"];
if (isset (request->query["alignment"])) {
  dialog_list = new Dialog_List (array ("sheet", "style"), gettext("Would you like to change the text alignment of this style?"), "", "");
  styles_logic = Styles_Logic::getInstance();
  for (i = AlignmentLeft; i <= AlignmentJustify; i++) {
    dialog_list.add_row (styles_logic.alignmentText (i), "&justification=i");
  }
  dialog_list.run ();
  die;
}
if (isset (request->query["justification"])) {
  justification = request->query["justification"];
  if (write) database_styles.updateJustification (sheet, style, justification);
}
view.view.justification = styles_logic.alignmentText (justification);

// Space before paragraph.
spacebefore = marker_data["spacebefore"];
if (isset (request->query["spacebefore"])) {
  dialog_entry = new Dialog_Entry (standard_page_query, gettext("Please enter a space of between 0 and 100 mm before the paragraph"), spacebefore, "spacebefore", gettext ("This is the space before, or in other words, above the paragraph. The value to enter is just a number, e.g. 0."));
  die;
}
if (isset(_POST["spacebefore"])) {
  value = _POST["entry"];
  value = Filter_Numeric::float_in_string (value);
  if ((value >= 0) && (value <= 100)) {
    spacebefore = value;
    if (write) database_styles.updateSpaceBefore (sheet, style, spacebefore);
  }
}
view.view.spacebefore = filter_string_sanitize_html (spacebefore);

// Space after paragraph.
spaceafter = marker_data["spaceafter"];
if (isset (request->query["spaceafter"])) {
  dialog_entry = new Dialog_Entry (standard_page_query, gettext("Please enter a space of between 0 and 100 mm after the paragraph"), spaceafter, "spaceafter", gettext ("This is the space after, or in other words, below the paragraph. The value to enter is just a number, e.g. 0."));
  die;
}
if (isset(_POST["spaceafter"])) {
  value = _POST["entry"];
  value = Filter_Numeric::float_in_string (value);
  if ((value >= 0) && (value <= 100)) {
    spaceafter = value;
    if (write) database_styles.updateSpaceAfter (sheet, style, spaceafter);
  }
}
view.view.spaceafter = filter_string_sanitize_html (spaceafter);

// Left margin.
leftmargin = marker_data["leftmargin"];
if (isset (request->query["leftmargin"])) {
  dialog_entry = new Dialog_Entry (standard_page_query, gettext("Please enter a left margin of between -100 and 100 mm"), leftmargin, "leftmargin", gettext ("This is the left margin of the paragraph. The value to enter is just a number, e.g. 0."));
  die;
}
if (isset(_POST["leftmargin"])) {
  value = _POST["entry"];
  value = Filter_Numeric::float_in_string (value);
  if ((value >= -100) && (value <= 100)) {
    leftmargin = value;
    if (write) database_styles.updateLeftMargin (sheet, style, leftmargin);
  }
}
view.view.leftmargin = filter_string_sanitize_html (leftmargin);

// Right margin.
rightmargin = marker_data["rightmargin"];
if (isset (request->query["rightmargin"])) {
  dialog_entry = new Dialog_Entry (standard_page_query, gettext("Please enter a right margin of between -100 and 100 mm"), rightmargin, "rightmargin", gettext ("This is the right margin of the paragraph. The value to enter is just a number, e.g. 0."));
  die;
}
if (isset(_POST["rightmargin"])) {
  value = _POST["entry"];
  value = Filter_Numeric::float_in_string (value);
  if ((value >= -100) && (value <= 100)) {
    rightmargin = value;
    if (write) database_styles.updateRightMargin (sheet, style, rightmargin);
  }
}
view.view.rightmargin = filter_string_sanitize_html (rightmargin);

// First line indent.
firstlineindent = marker_data["firstlineindent"];
if (isset (request->query["firstlineindent"])) {
  dialog_entry = new Dialog_Entry (standard_page_query, gettext("Please enter a first line indent of between -100 and 100 mm"), firstlineindent, "firstlineindent", gettext ("This is the indent of the first line of the the paragraph. The value to enter is just a number, e.g. 0."));
  die;
}
if (isset(_POST["firstlineindent"])) {
  value = _POST["entry"];
  value = Filter_Numeric::float_in_string (value);
  if ((value >= -100) && (value <= 100)) {
    firstlineindent = value;
    if (write) database_styles.updateFirstLineIndent (sheet, style, firstlineindent);
  }
}
view.view.firstlineindent = filter_string_sanitize_html (firstlineindent);

// Columns spanning.
view.view.columns_relevant = styles_logic.columnsAreRelevant (type, subtype);
spancolumns = marker_data["spancolumns"];
if (isset (request->query["spancolumns"])) {
  spancolumns = Filter_Bool::int (request->query["spancolumns"]);
  if (write) database_styles.updateSpanColumns (sheet, style, spancolumns);
}
view.view.spancolumns = styles_logic.OffOnInheritToggleText (spancolumns);
view.view.spancolumns_toggle = Filter_Bool::not (spancolumns);

// Color.
view.view.color_relevant = styles_logic.colorIsRelevant (type, subtype);
color = marker_data["color"];
if (isset (request->query["color"])) {
  value = request->query["color"];
  if (value == "") {
    dialog_color = new Dialog_Color (standard_page_query, gettext("Please specify a new color"), "color");
    die;
  } else {
    color = value;
    if (write) database_styles.updateColor (sheet, style, color);
  }
}
view.view.color = filter_string_sanitize_html (color);

// Whether to print this style.
view.view.print_relevant = styles_logic.printIsRelevant (type, subtype);
print = marker_data["print"];
if (isset (request->query["print"])) {
  print = Filter_Bool::int (request->query["print"]);
  if (write) database_styles.updatePrint (sheet, style, print);
}
view.view.print = styles_logic.OffOnInheritToggleText (print);
view.view.print_toggle = Filter_Bool::not (print);

// Userbool1
view.view.userbool1_function = styles_logic.getUserBool1Text (styles_logic.getUserBool1Function (type, subtype));
userbool1 = marker_data["userbool1"];
if (isset (request->query["userbool1"])) {
  userbool1 = Filter_Bool::int (request->query["userbool1"]);
  if (write) database_styles.updateUserbool1 (sheet, style, userbool1);
}
view.view.userbool1_value = styles_logic.OffOnInheritToggleText (userbool1);
view.view.userbool1_toggle = Filter_Bool::not (userbool1);

// Userbool2
view.view.userbool2_function = styles_logic.getUserBool2Text (styles_logic.getUserBool2Function (type, subtype));
userbool2 = marker_data["userbool2"];
if (isset (request->query["userbool2"])) {
  userbool2 = Filter_Bool::int (request->query["userbool2"]);
  if (write) database_styles.updateUserbool2 (sheet, style, userbool2);
}
view.view.userbool2_value = styles_logic.OffOnInheritToggleText (userbool2);
view.view.userbool2_toggle = Filter_Bool::not (userbool2);

// Userbool3
view.view.userbool3_function = styles_logic.getUserBool3Text (styles_logic.getUserBool3Function (type, subtype));
userbool3 = marker_data["userbool3"];
if (isset (request->query["userbool3"])) {
  userbool3 = Filter_Bool::int (request->query["userbool3"]);
  if (write) database_styles.updateUserbool3 (sheet, style, userbool3);
}
view.view.userbool3_value = styles_logic.OffOnInheritToggleText (userbool3);
view.view.userbool3_toggle = Filter_Bool::not (userbool3);

// Userint1.
userint1 = marker_data["userint1"];
switch (styles_logic.getUserInt1Function (type, subtype)) {
  case UserInt1None :
    break;
  case UserInt1NoteNumbering :
    view.view.userint1_notenumbering = true;
    if (isset (request->query["notenumbering"])) {
      dialog_list = new Dialog_List (array ("sheet", "style"), gettext("Would you like to change the numbering of the note?"), "", "");
      styles_logic = Styles_Logic::getInstance();
      for (i = NoteNumbering123; i <= NoteNumberingUser; i++) {
        dialog_list.add_row (styles_logic.noteNumberingText (i), "&userint1=i");
      }
      dialog_list.run ();
      die;
    }
    if (isset (request->query["userint1"])) {
      userint1 = request->query["userint1"];
      if (write) database_styles.updateUserint1 (sheet, style, userint1);
    }
    view.view.userint1 = styles_logic.noteNumberingText (userint1);
    break;
  case UserInt1TableColumnNumber :
    view.view.userint1_columnnumber = true;
    if (isset (request->query["userint1"])) {
      dialog_entry = new Dialog_Entry (standard_page_query, gettext("Please enter a column number between 1 and 4"), userint1, "userint1", gettext ("This is the column number for the style. The first columm is number 1."));
      die;
    }
    if (isset(_POST["userint1"])) {
      value = _POST["entry"];
      value = Filter_Numeric::integer_in_string (value);
      if ((value >= 1) && (value <= 4)) {
        userint1 = value;
        if (write) database_styles.updateUserint1 (sheet, style, userint1);
      }
    }
    view.view.userint1 = userint1;
    break;
}

// Userint2.
userint2 = marker_data["userint2"];
switch (styles_logic.getUserInt2Function (type, subtype)) {
  case UserInt2None :
    break;
  case UserInt2NoteNumberingRestart :
    view.view.userint2_notenumberingrestart = true;
    if (isset (request->query["notenumberingrestart"])) {
      dialog_list = new Dialog_List (array ("sheet", "style"), gettext("Would you like to change when the note numbering restarts?"), "", "");
      styles_logic = Styles_Logic::getInstance();
      for (i = NoteRestartNumberingNever; i <= NoteRestartNumberingEveryChapter; i++) {
        dialog_list.add_row (styles_logic.noteRestartNumberingText (i), "&userint2=i");
      }
      dialog_list.run ();
      die;
    }
    if (isset (request->query["userint2"])) {
      userint2 = request->query["userint2"];
      if (write) database_styles.updateUserint2 (sheet, style, userint2);
    }
    view.view.userint2 = styles_logic.noteRestartNumberingText (userint2);
    break;
  case UserInt2EndnotePosition :
    view.view.userint2_endnoteposition = true;
    if (isset (request->query["endnoteposition"])) {
      dialog_list = new Dialog_List (array ("sheet", "style"), gettext("Would you like to change the position where to dump the endnotes?"), "", "");
      styles_logic = Styles_Logic::getInstance();
      for (i = EndNotePositionAfterBook; i <= EndNotePositionAtMarker; i++) {
        dialog_list.add_row (styles_logic.endNotePositionText (i), "&userint2=i");
      }
      dialog_list.run ();
      die;
    }
    if (isset (request->query["userint2"])) {
      userint2 = request->query["userint2"];
      if (write) database_styles.updateUserint2 (sheet, style, userint2);
    }
    view.view.userint2 = styles_logic.endNotePositionText (userint2);
    break;
}

// Userint3 not yet used.

// Userstring1.
userstring1 = marker_data["userstring1"];
switch (styles_logic.getUserString1Function (type, subtype)) {
  case UserString1None :
    break;
  case UserString1NoteNumberingSequence :
    if (userint1 == NoteNumberingUser) {
      view.view.userstring1_numberingsequence = true;
      userstring_question = gettext("Please enter a new note numbering sequence");
      userstring1_help = gettext("This gives a sequence for numbering the notes. When for example § † * is entered, the numbering goes like §, †, *, §, †, *, and so forth. Any sequence of characters can be used. Spaces should separate the characters");
    }
    break;
  case UserString1WordListEntryAddition :
    view.view.userstring1_wordlistaddition = true;
    userstring1_question = gettext("Please enter a new addition to the word list entry");
    userstring1_help = gettext("This given an optional string to be added after each definition in the body of text. In some Bibles the unusual words are marked with an asterisk and then explained in a glossary. If you would enter the asterisk here, or indeed any string, Bibledit-Web would include this in the exported documents.");
    break;
}
if (isset (request->query["userstring1"])) {
  dialog_entry = new Dialog_Entry (standard_page_query, userstring1_question, userstring1, "userstring1", userstring1_help);
  die;
}
if (isset(_POST["userstring1"])) {
  userstring1 = _POST["entry"];
  if (write) database_styles.updateUserstring1 (sheet, style, userstring1);
}
if (userstring1 == "") userstring1 = "--";
view.view.userstring1 = filter_string_sanitize_html (userstring1);

// Userstring2
userstring2 = marker_data["userstring2"];
switch (styles_logic.getUserString2Function (type, subtype)) {
  case UserString2None :
    break;
  case UserString2DumpEndnotesHere :
    if (userint2 == EndNotePositionAtMarker) {
      view.view.userstring2_dumpendnotes = true;
      userstring2_question = gettext("Please enter a marker at which the endnotes should be dumped");
      userstring2_info = gettext("The marker is to be given without the backslash, e.g. \"zendnotes\".");
    }
    break;
}
if (isset (request->query["userstring2"])) {
  dialog_entry = new Dialog_Entry (standard_page_query, userstring2_question, userstring2, "userstring2", userstring2_info);
  die;
}
if (isset(_POST["userstring2"])) {
  userstring2 = _POST["entry"];
  if (write) database_styles.updateUserstring2 (sheet, style, userstring2);
}
if (userstring2 == "") userstring2 = "--";
view.view.userstring2 = filter_string_sanitize_html (userstring2);

// Recreate stylesheets after editing a style.
if ((count (request->query) != 2) || (count (_POST) != 0)) {
  Styles_Sheets::create_all ();
}

// Userstring3 not yet used.

view.render ("view.php");

Assets_Page::footer ();

?>
