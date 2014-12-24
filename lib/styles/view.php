<?php

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
