<h1><?php echo gettext ("Typesetting in LibreOffice") ?></h1>

<h2><?php echo gettext ("Introduction") ?></h2>
<p><?php echo gettext ("See the online help about typesetting for more information about some general principles.") ?></p>
<p><?php echo gettext ("You have access to a copy of LibreOffice.") ?></p>

<h2><?php echo gettext ("Page count") ?></h2>
<p><?php echo gettext ("The publisher may set limits to the number of pages the typeset Bible may contain.") ?></p>
<p><?php echo gettext ("In this example, we expect an entire Bible, with singable Psalms added as an appendix, to contain no more than 2000 pages. Normal range is 1500 to 1800 pages.") ?></p>
<br>
<p><?php echo gettext ("We suppose that the singable Psalms take up no more than 200 pages.") ?></p>
<p><?php echo gettext ("Usually the inclusion of the singable Psalms in a Bible will add about 10% to the number of pages.") ?></p>
<br>
<p><?php echo gettext ("The Bible will have footnotes and crossreferences, all lumped together into a single textframe at the bottom of each page.") ?></p>
<p><?php echo gettext ("Let us now measure how many pages the notes are likely to take up.") ?></p>
<p><?php echo gettext ("Download the file with notes. It was exported above. It is called 00_Bible_notes.odt.") ?></p>
<p><?php echo gettext ("Open the file in LibreOffice.") ?></p>
<p><?php echo gettext ("Let it repaginate. A good way of doing this is to press Ctrl-P as if going to print the document. Then it will repaginate. Cancel the print dialog.") ?></p>
<p><?php echo gettext ("It will now give the page count that the notes are likely to take up.") ?></p>
<p><?php echo gettext ("In our case, this is about 300 pages.") ?></p>
<p><?php echo gettext ("Each chapter has headings. These headings do not belong to the notes. Thus the page count is too high.") ?></p>
<p><?php echo gettext ("Notice that the paragraph style for these headings is called 'Heading 1'. ") ?></p>
<p><?php echo gettext ("Hide the headings. This can be done like this. Right-click on a heading. Choose 'Edit Paragraph Style'. A dialog comes up. Choose tab 'Font Effects'. Tick option 'Hidden'.") ?></p>
<p><?php echo gettext ("Repaginate, and notice the page count. In our case it is down to about 125.") ?></p>
<br>
<p><?php echo gettext ("Let us now find out how many pages the main text body is likely to take up.") ?></p>
<p><?php echo gettext ("Download the OpenDocument file with text and note citations. It was exported above. It is called 00_Bible_text_and_note_citations.odt.") ?></p>
<p><?php echo gettext ("The document has only one column.") ?></p>
<p><?php echo gettext ("Repaginate and notice the page count. In our case this is 1750.") ?></p>
<p><?php echo gettext ("This figure is not accurate since it applies to a column count of one.") ?></p>
<p><?php echo gettext ("Set the whole document to have two columns.") ?></p>
<p><?php echo gettext ("Go to menu Format / Page. Choose the Columns tab. Choose two columns. Set the spacing to 0.2 cm. Set a thin separator line. Click Ok.") ?></p>
<p><?php echo gettext ("Repaginate and notice the page count. It is down to 1520.") ?></p>
<br>
<p><?php echo gettext ("The font is of particular influence on the page count. This is not dealt with here.") ?></p>
<br>
<p><?php echo gettext ("We are now able to estimate the page count for the entire Bible:") ?></p>
<p><?php echo gettext ("Text body 1520 pages. Notes 125 pages. Singable Psalms 200 pages. Estimated total is 1845 pages.") ?></p>

<h2><?php echo gettext ("Using LibreOffice for typesetting") ?></h2>
<p><?php echo gettext ("Having used LibreOffice for typesetting for some time, my experience is that the program is suitable for typesetting Bibles.") ?></p>
<p><?php echo gettext ("I'd like to give some hints about how to do typographic work in LibreOffice.") ?></p>
<br>
<p><?php echo gettext ("To meet the requirement that line spacing be consistent throughout and be placed line on line across all pages, do the following:") ?></p>
<p><?php echo gettext ("Go to menu Format / Page, and enable Register-true. Set the reference style to 'Text Body'. Press OK.") ?></p>
<p><?php echo gettext ("Select all the text, and go to menu Format / Paragraph, and activate Register-true. Press OK.") ?></p>
<p><?php echo gettext ("The line spacing specified by the paragraph style 'Text Body' is now used as the reference line spacing for all paragraphs that have the register-true feature enabled.") ?></p>
<p><?php echo gettext ("The line spacing in the document is going to be consistent. It will be equal to the line spacing of the reference style 'Text Body', or a multiple of it.") ?></p>
<br>
<p><?php echo gettext ("Changing the line spacing.") ?></p>
<p><?php echo gettext ("Change the line spacing in paragraph style 'Text Body'.") ?></p>
<p><?php echo gettext ("Press F11 to get the Styles and Formatting dialog.") ?></p>
<p><?php echo gettext ("Locate style 'Text Body'.") ?></p>
<p><?php echo gettext ("Right-click it with the mouse, and choose menu Modify.") ?></p>
<p><?php echo gettext ("Go to tab Indents & Spacing.") ?></p>
<p><?php echo gettext ("Adjust the spacing above the paragraph, the spacing below the paragrap, and the line spacing. Press OK.") ?></p>
<p><?php echo gettext ("It may happen that some paragraphs in the text used to have a line spacing equal to the line spacing of style 'Text Body', but now suddenly take up a multiple of it. This may happen when you have tightened the line spacing a bit.") ?></p>
<p><?php echo gettext ("To resolve this, make the line spacing of those paragraphs smaller than the line spacing of style 'Text body'.") ?></p>
<br>
<p><?php echo gettext ("Updating a paragraph style.") ?></p>
<p><?php echo gettext ("After exporting USFM text to OpenDocument, the paragraph styles of the USFM document appear in LibreOffice.") ?></p>
<p><?php echo gettext ("For example, normal paragraphs usually have style 'p', and section heading 's'.") ?></p>
<p><?php echo gettext ("If you like to update a paragraph style, you may update each section heading individually, but this is a lot of work. It is faster to update the style for 's', so that this updates all paragraphs that have this style.") ?></p>
<p><?php echo gettext ("To update a paragraph style, right-click the text of this paragraph, and choose menu Edit Paragraph Style..., and make the desired settings.") ?></p>
<br>
<p><?php echo gettext ("Exporting to PDF.") ?></p>
<p><?php echo gettext ("See menu File / Export as PDF.") ?></p>
<br>
<p><?php echo gettext ("Hiding the soft hyphens and non-breaking spaces.") ?></p>
<p><?php echo gettext ("The soft hyphens and the non-breaking spaces are visible as grey fields that clutter the page. To hide them, do this:") ?></p>
<p><?php echo gettext ("Go to menu Tools / Options.") ?></p>
<p><?php echo gettext ("Go to LibreOffice Writer / Formatting Aids.") ?></p>
<p><?php echo gettext ("Untick display of custom hyphens.") ?></p>
<p><?php echo gettext ("Untick display of non-breaking spaces.") ?></p>
