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
<h1><?php echo Locale_Translate::_("Typesetting in LibreOffice") ?></h1>

<h2><?php echo Locale_Translate::_("Introduction") ?></h2>
<p><?php echo Locale_Translate::_("See the online help about typesetting for more information about some general principles.") ?></p>
<p><?php echo Locale_Translate::_("You have access to a copy of LibreOffice.") ?></p>

<h2><?php echo Locale_Translate::_("Page count") ?></h2>
<p><?php echo Locale_Translate::_("The publisher may set limits to the number of pages the typeset Bible may contain.") ?></p>
<p><?php echo Locale_Translate::_("In this example, we expect an entire Bible, with singable Psalms added as an appendix, to contain no more than 2000 pages. Normal range is 1500 to 1800 pages.") ?></p>
<br>
<p><?php echo Locale_Translate::_("We suppose that the singable Psalms take up no more than 200 pages.") ?></p>
<p><?php echo Locale_Translate::_("Usually the inclusion of the singable Psalms in a Bible will add about 10% to the number of pages.") ?></p>
<br>
<p><?php echo Locale_Translate::_("The Bible will have footnotes and crossreferences, all lumped together into a single textframe at the bottom of each page.") ?></p>
<p><?php echo Locale_Translate::_("Let us now measure how many pages the notes are likely to take up.") ?></p>
<p><?php echo Locale_Translate::_("Download the file with notes. It was exported above. It is called 00_Bible_notes.odt.") ?></p>
<p><?php echo Locale_Translate::_("Open the file in LibreOffice.") ?></p>
<p><?php echo Locale_Translate::_("Let it repaginate. A good way of doing this is to press Ctrl-P as if going to print the document. Then it will repaginate. Cancel the print dialog.") ?></p>
<p><?php echo Locale_Translate::_("It will now give the page count that the notes are likely to take up.") ?></p>
<p><?php echo Locale_Translate::_("In our case, this is about 300 pages.") ?></p>
<p><?php echo Locale_Translate::_("Each chapter has headings. These headings do not belong to the notes. Thus the page count is too high.") ?></p>
<p><?php echo Locale_Translate::_("Notice that the paragraph style for these headings is called 'Heading 1'. ") ?></p>
<p><?php echo Locale_Translate::_("Hide the headings. This can be done like this. Right-click on a heading. Choose 'Edit Paragraph Style'. A dialog comes up. Choose tab 'Font Effects'. Tick option 'Hidden'.") ?></p>
<p><?php echo Locale_Translate::_("Repaginate, and notice the page count. In our case it is down to about 125.") ?></p>
<br>
<p><?php echo Locale_Translate::_("Let us now find out how many pages the main text body is likely to take up.") ?></p>
<p><?php echo Locale_Translate::_("Download the OpenDocument file with text and note citations. It was exported above. It is called 00_Bible_text_and_note_citations.odt.") ?></p>
<p><?php echo Locale_Translate::_("The document has only one column.") ?></p>
<p><?php echo Locale_Translate::_("Repaginate and notice the page count. In our case this is 1750.") ?></p>
<p><?php echo Locale_Translate::_("This figure is not accurate since it applies to a column count of one.") ?></p>
<p><?php echo Locale_Translate::_("Set the whole document to have two columns.") ?></p>
<p><?php echo Locale_Translate::_("Go to menu Format / Page. Choose the Columns tab. Choose two columns. Set the spacing to 0.2 cm. Set a thin separator line. Click Ok.") ?></p>
<p><?php echo Locale_Translate::_("Repaginate and notice the page count. It is down to 1520.") ?></p>
<br>
<p><?php echo Locale_Translate::_("The font is of particular influence on the page count. This is not dealt with here.") ?></p>
<br>
<p><?php echo Locale_Translate::_("We are now able to estimate the page count for the entire Bible:") ?></p>
<p><?php echo Locale_Translate::_("Text body 1520 pages. Notes 125 pages. Singable Psalms 200 pages. Estimated total is 1845 pages.") ?></p>

<h2><?php echo Locale_Translate::_("Using LibreOffice for typesetting") ?></h2>
<p><?php echo Locale_Translate::_("Having used LibreOffice for typesetting for some time, my experience is that the program is suitable for typesetting Bibles.") ?></p>
<p><?php echo Locale_Translate::_("I'd like to give some hints about how to do typographic work in LibreOffice.") ?></p>
<br>
<p><?php echo Locale_Translate::_("To meet the requirement that line spacing be consistent throughout and be placed line on line across all pages, do the following:") ?></p>
<p><?php echo Locale_Translate::_("Go to menu Format / Page, and enable Register-true. Set the reference style to 'Text Body'. Press OK.") ?></p>
<p><?php echo Locale_Translate::_("Select all the text, and go to menu Format / Paragraph, and activate Register-true. Press OK.") ?></p>
<p><?php echo Locale_Translate::_("The line spacing specified by the paragraph style 'Text Body' is now used as the reference line spacing for all paragraphs that have the register-true feature enabled.") ?></p>
<p><?php echo Locale_Translate::_("The line spacing in the document is going to be consistent. It will be equal to the line spacing of the reference style 'Text Body', or a multiple of it.") ?></p>
<br>
<p><?php echo Locale_Translate::_("Changing the line spacing.") ?></p>
<p><?php echo Locale_Translate::_("Change the line spacing in paragraph style 'Text Body'.") ?></p>
<p><?php echo Locale_Translate::_("Press F11 to get the Styles and Formatting dialog.") ?></p>
<p><?php echo Locale_Translate::_("Locate style 'Text Body'.") ?></p>
<p><?php echo Locale_Translate::_("Right-click it with the mouse, and choose menu Modify.") ?></p>
<p><?php echo Locale_Translate::_("Go to tab Indents & Spacing.") ?></p>
<p><?php echo Locale_Translate::_("Adjust the spacing above the paragraph, the spacing below the paragrap, and the line spacing. Press OK.") ?></p>
<p><?php echo Locale_Translate::_("It may happen that some paragraphs in the text used to have a line spacing equal to the line spacing of style 'Text Body', but now suddenly take up a multiple of it. This may happen when you have tightened the line spacing a bit.") ?></p>
<p><?php echo Locale_Translate::_("To resolve this, make the line spacing of those paragraphs smaller than the line spacing of style 'Text body'.") ?></p>
<br>
<p><?php echo Locale_Translate::_("Updating a paragraph style.") ?></p>
<p><?php echo Locale_Translate::_("After exporting USFM text to OpenDocument, the paragraph styles of the USFM document appear in LibreOffice.") ?></p>
<p><?php echo Locale_Translate::_("For example, normal paragraphs usually have style 'p', and section heading 's'.") ?></p>
<p><?php echo Locale_Translate::_("If you like to update a paragraph style, you may update each section heading individually, but this is a lot of work. It is faster to update the style for 's', so that this updates all paragraphs that have this style.") ?></p>
<p><?php echo Locale_Translate::_("To update a paragraph style, right-click the text of this paragraph, and choose menu Edit Paragraph Style..., and make the desired settings.") ?></p>
<br>
<p><?php echo Locale_Translate::_("Exporting to PDF.") ?></p>
<p><?php echo Locale_Translate::_("See menu File / Export as PDF.") ?></p>
<br>
<p><?php echo Locale_Translate::_("Hiding the soft hyphens and non-breaking spaces.") ?></p>
<p><?php echo Locale_Translate::_("The soft hyphens and the non-breaking spaces are visible as grey fields that clutter the page. To hide them, do this:") ?></p>
<p><?php echo Locale_Translate::_("Go to menu Tools / Options.") ?></p>
<p><?php echo Locale_Translate::_("Go to LibreOffice Writer / Formatting Aids.") ?></p>
<p><?php echo Locale_Translate::_("Untick display of custom hyphens.") ?></p>
<p><?php echo Locale_Translate::_("Untick display of non-breaking spaces.") ?></p>
