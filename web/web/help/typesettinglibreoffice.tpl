<h1>{t}Typesetting in LibreOffice{/t}</h1>

<h2>{t}Introduction{/t}</h2>
<p>{t}See the online help about typesetting for more information about some general principles.{/t}</p>
<p>{t}You have access to a copy of LibreOffice.{/t}</p>

<h2>{t}Page count{/t}</h2>
<p>{t}The publisher may set limits to the number of pages the typeset Bible may contain.{/t}</p>
<p>{t}In this example, we expect an entire Bible, with singable Psalms added as an appendix, to contain no more than 2000 pages. Normal range is 1500 to 1800 pages.{/t}</p>
<br>
<p>{t}We suppose that the singable Psalms take up no more than 200 pages.{/t}</p>
<p>{t}Usually the inclusion of the singable Psalms in a Bible will add about 10% to the number of pages.{/t}</p>
<br>
<p>{t}The Bible will have footnotes and crossreferences, all lumped together into a single textframe at the bottom of each page.{/t}</p>
<p>{t}Let us now measure how many pages the notes are likely to take up.{/t}</p>
<p>{t}Download the file with notes. It was exported above. It is called 00_Bible_notes.odt.{/t}</p>
<p>{t}Open the file in LibreOffice.{/t}</p>
<p>{t}Let it repaginate. A good way of doing this is to press Ctrl-P as if going to print the document. Then it will repaginate. Cancel the print dialog.{/t}</p>
<p>{t}It will now give the page count that the notes are likely to take up.{/t}</p>
<p>{t}In our case, this is about 300 pages.{/t}</p>
<p>{t}Each chapter has headings. These headings do not belong to the notes. Thus the page count is too high.{/t}</p>
<p>{t}Notice that the paragraph style for these headings is called "Heading 1". {/t}</p>
<p>{t}Hide the headings. This can be done like this. Right-click on a heading. Choose "Edit Paragraph Style". A dialog comes up. Choose tab "Font Effects". Tick option "Hidden".{/t}</p>
<p>{t}Repaginate, and notice the page count. In our case it is down to about 125.{/t}</p>
<br>
<p>{t}Let us now find out how many pages the main text body is likely to take up.{/t}</p>
<p>{t}Download the OpenDocument file with text and note citations. It was exported above. It is called 00_Bible_text_and_note_citations.odt.{/t}</p>
<p>{t}The document has only one column.{/t}</p>
<p>{t}Repaginate and notice the page count. In our case this is 1750.{/t}</p>
<p>{t}This figure is not accurate since it applies to a column count of one.{/t}</p>
<p>{t}Set the whole document to have two columns.{/t}</p>
<p>{t}Go to menu Format / Page. Choose the Columns tab. Choose two columns. Set the spacing to 0.2 cm. Set a thin separator line. Click Ok.{/t}</p>
<p>{t}Repaginate and notice the page count. It is down to 1520.{/t}</p>
<br>
<p>{t}The font is of particular influence on the page count. This is not dealt with here.{/t}</p>
<br>
<p>{t}We are now able to estimate the page count for the entire Bible:{/t}</p>
<p>{t}Text body 1520 pages. Notes 125 pages. Singable Psalms 200 pages. Estimated total is 1845 pages.{/t}</p>

<h2>{t}Using LibreOffice for typesetting{/t}</h2>
<p>{t}Having used LibreOffice for typesetting for some time, my experience is that the program is suitable for typesetting Bibles.{/t}</p>
<p>{t}I'd like to give some hints about how to do typographic work in LibreOffice.{/t}</p>
<br>
<p>{t}To meet the requirement that line spacing be consistent throughout and be placed line on line across all pages, do the following:{/t}</p>
<p>{t}Go to menu Format / Page, and enable Register-true. Set the reference style to "Text Body". Press OK.{/t}</p>
<p>{t}Select all the text, and go to menu Format / Paragraph, and activate Register-true. Press OK.{/t}</p>
<p>{t}The line spacing specified by the paragraph style "Text Body" is now used as the reference line spacing for all paragraphs that have the register-true feature enabled.{/t}</p>
<p>{t}The line spacing in the document is going to be consistent. It will be equal to the line spacing of the reference style "Text Body", or a multiple of it.{/t}</p>
<br>
<p>{t}Changing the line spacing.{/t}</p>
<p>{t}Change the line spacing in paragraph style "Text Body".{/t}</p>
<p>{t}Press F11 to get the Styles and Formatting dialog.{/t}</p>
<p>{t}Locate style "Text Body".{/t}</p>
<p>{t}Right-click it with the mouse, and choose menu Modify.{/t}</p>
<p>{t}Go to tab Indents & Spacing.{/t}</p>
<p>{t}Adjust the spacing above the paragraph, the spacing below the paragrap, and the line spacing. Press OK.{/t}</p>
<p>{t}It may happen that some paragraphs in the text used to have a line spacing equal to the line spacing of style "Text Body", but now suddenly take up a multiple of it. This may happen when you have tightened the line spacing a bit.{/t}</p>
<p>{t}To resolve this, make the line spacing of those paragraphs smaller than the line spacing of style "Text body".{/t}</p>
<br>
<p>{t}Updating a paragraph style.{/t}</p>
<p>{t}After exporting USFM text to OpenDocument, the paragraph styles of the USFM document appear in LibreOffice.{/t}</p>
<p>{t}For example, normal paragraphs usually have style "p", and section heading "s".{/t}</p>
<p>{t}If you like to update a paragraph style, you may update each section heading individually, but this is a lot of work. It is faster to update the style for "s", so that this updates all paragraphs that have this style.{/t}</p>
<p>{t}To update a paragraph style, right-click the text of this paragraph, and choose menu Edit Paragraph Style..., and make the desired settings.{/t}</p>
<br>
<p>{t}Exporting to PDF.{/t}</p>
<p>{t}See menu File / Export as PDF.{/t}</p>
<br>
<p>{t}Hiding the soft hyphens and non-breaking spaces.{/t}</p>
<p>{t}The soft hyphens and the non-breaking spaces are visible as grey fields that clutter the page. To hide them, do this:{/t}</p>
<p>{t}Go to menu Tools / Options.{/t}</p>
<p>{t}Go to LibreOffice Writer / Formatting Aids.{/t}</p>
<p>{t}Untick display of custom hyphens.{/t}</p>
<p>{t}Untick display of non-breaking spaces.{/t}</p>
