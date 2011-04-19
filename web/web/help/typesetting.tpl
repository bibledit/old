<h1>{t}Typesetting Bibles{/t}</h1>

<h2>{t}Introduction{/t}</h2>
<p>{t}The Bible has been translated. It is time to publish it.{/t}</p>
<p>{t}This article gives some hints about how to typeset a Bible.{/t}</p>
<p>{t}At the end of all, you will have a PDF file with the whole Bible, typeset, ready for publishing.{/t}</p>

<h2>{t}Basic assumptions{/t}</h2>
<p>{t}The Bible is available in USFM code. The USFM code is clean and adheres to the standards.{/t}</p>
<p>{t}You have access to your own copy of Bibledit-Web, or to the Bibledit-Web demo on the internet.{/t}</p>
<p>{t}You have access to a copy of LibreOffice or OpenOffice.{/t}</p>

<h2>{t}Exporting the USFM files to OpenDocument files{/t}</h2>
<p>{t}This section writes about a specific Bible that was to be prepared for publication. It gives the steps I took to convert the USFM code to OpenDocument files.{/t}</p>
<p>{t}The OpenDocument files are used as a starting point for typesetting. Typesetting itself is done in LibreOffice.{/t}</p>
<br>
<p>{t}The Bible is available in USFM files. Each Bible book is stored as one USFM file.{/t}</p>
<p>{t}Create a new Bible in Bibledit-Web through menu Management / Bibles. Give the Bible a name. I call it "Ndebele".{/t}</p>
<p>{t}Click the "Ndebele" Bible.{/t}</p>
<p>{t}Import data into it.{/t}</p>
<p>{t}Choose the option to upload a file.{/t}</p>
<p>{t}You can upload 66 separate files, or you can put all the 66 files into one zipped file, and then upload that zipped file. I chose the second option, as being easier and quicker than the first. Importing took about a minute. The logbook shows the progress.{/t}</p>
<p>{t}Create a new stylesheet in menu Management / Styles. Give it a name. I call it "Ndebele".{/t}</p>
<p>{t}All the data is now in Bibledit-Web. Everthing is ready for export to OpenDocument files.{/t}</p>
<br>
<p>{t}Let us now make the first export and see how the files look.{/t}</p>
<p>{t}Go to menu Management / Exports.{/t}</p>
<p>{t}Add Bible "Ndebele" to the ones that will be exported.{/t}</p>
<p>{t}Use stylesheet "Ndebele".{/t}</p>
<p>{t}Output the chapter number in drop caps. Switch that setting on.{/t}</p>
<p>{t}Set the page size. I've used a width of 138 mm and a height of 210 mm.{/t}</p>
<p>{t}Set the margins. I've used 10 mm for top, bottom, and outer margins, and 12 mm for the inner margins.{/t}</p>
<p>{t}Do not display the date in the running headers. Switch that setting off.{/t}</p>
<p>{t}Do a fresh export. Give it some time to finish.{/t}</p>
<p>{t}Go to menu Members / Downloads / Exported Bibles, and choose your Bible there. Then go to the OpenDocument folder.{/t}</p>
<p>{t}You will see the Bible exported into several format.{/t}</p>
<p>{t}View them and see if you like them.{/t}</p>
<br>
<p>{t}I noticed that the Divine name was underlined in the OpenDocument files. The Divine name is within \nd_...\nd* markup in USFM. I prefer this in small capitals, not underlined.{/t}</p>
<p>{t}An update in the stylesheet is needed.{/t}</p>
<p>{t}Go to menu Management / Styles, and choose the "Ndebele" stylesheet.{/t}</p>
<p>{t}Click the "nd" style.{/t}</p>
<p>{t}Switch "Underline" off.{/t}</p>
<p>{t}Switch "Small caps" on.{/t}</p>
<p>{t}Do a new export, wait a while, and review the OpenDocument files again.{/t}</p>
<br>
<p>{t}I notice that the verse numbers are in superscript. The verse numbers are small and difficult to read.{/t}</p>
<p>{t}The intended audience prefers the verse numbers in bigger print.{/t}</p>
<p>{t}Go to the relevant stylesheet and choose the "v" style.{/t}</p>
<p>{t}Switch "Superscript" off.{/t}</p>
<p>{t}Do a new export and review the OpenDocument files.{/t}</p>
<br>
<p>{t}The text in the OpenDocument files has been divided into paragraphs. New paragraphs start when appropriate in the context. This is how the USFM files have been marked up.{/t}</p>
<p>{t}The people who are going to read this Bible prefer each verse to start a new paragraph.{/t}</p>
<p>{t}Go to the stylesheet, and choose the "v" style.{/t}</p>
<p>{t}Switch "Restart paragraph" on.{/t}</p>
<p>{t}Export again.{/t}</p>
<br>
<p>{t}Since each verse starts a new paragraph, there is no longer any need to indent the first line of that paragraph.{/t}</p>
<p>{t}In the stylesheet, edit the "p" style.{/t}</p>
<p>{t}Set the "First line indent" to 0 mm.{/t}</p>
<br>
<p>{t}Some verses are short. Chapters that start with short verses no longer have the chapter number in drop caps in LibreOffice.{/t}</p>
<p>{t}This is because this verse is all on one line, and thus there is no space for drop caps, as these need two lines of text.{/t}</p>
<p>{t}Setting the text in two columns in LibreOffice solves the problem. The drop caps are back.{/t}</p>
<br>
<p>{t}The letters of the Bible text are too big. This would take up too much space. The Bible would get too many pages.{/t}</p>
<p>{t}Find out which styles start paragraphs. In the Ndebele Bible, these are the styles p, s, nb, d.{/t}</p>
<p>{t}Set these styles to have a font size of 10 points.{/t}</p>
<br>
<p>{t}The letters of the notes and crossreferences are too big.{/t}</p>
<p>{t}Set the fontsize of the styles ft and xt to 6 points.{/t}</p>
<br>
<p>{t}Parts of the footnotes are in bold or italics or both. I prefer it all in normal script.{/t}</p>
<p>{t}Update the relevant footnote styles and switch bold and italics off. The styles are fk, fl, fq, fqa.{/t}</p>
<br>
<p>{t}Some of the word in the OpenDocument files are long. This results in too much space between some words.{/t}</p>
<p>{t}The words need to get hyphens so that part of the word goes on the next line.{/t}</p>
<p>{t}LibreOffice has hyphenation algorithms for many languages. That would solve the problem.{/t}</p>
<p>{t}LibreOffice does not have a hyphenation algorithm for our language.{/t}</p>
<p>{t}Bibledit-Web has a hyphenater that could assist.{/t}</p>
<p>{t}In Bibledit-Web, go to menu Management / Hyphenation.{/t}</p>
<p>{t}Enter the first set of characters, and the second set, and save it.{/t}</p>
<p>{t}Set the Bible where to take the data from. In this case it is "Ndebele".{/t}</p>
<p>{t}Insert the soft hyphens according to the setting. It takes a while to complete.{/t}</p>
<p>{t}The output Bible is called "Ndebele-hyphenated".{/t}</p>
<p>{t}Use this Bible in the Exports options. Export it.{/t}</p>

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
<h2>{t}Typesetting guidelines{/t}</h2>
<ol>
<li>{t}Line spacing should be consistent throughout and should be placed line on line across all pages. It means that they should all line up with each other all the way down the page and across all pages. Then when the Bible is printed, the printed text will be back to back and not show through from one side of the page to the other.{/t}</li>
<li>{t}The space after the verse numbers would be nearly the same and consistent.{/t}</li>
<li>{t}The alignment of the drop caps chapter numbers to the text is to be consistent. The chapter divisions would also benefit from some more space between them and the text of the preceding chapter.{/t}</li>
<li>{t}The amount of space given to the Bible book headings should not be too great.{/t}</li>
<li>{t}Subheadings should not appear squashed amongst the rest of the text and would benefit from some space.{/t}</li>
<li>{t}The space between the text and footnotes should not be too tight neither unnecessarily large.{/t}</li>
<li>{t}In the crossreferences it is pleasing to have the chapter and verse number kept together with the book name, especially as the small font on long lines gives scope for this.{/t}</li>
<li>{t}The columns should start at the same height and should be balanced at the ends.{/t}</li>
<li>{t}The Psalm headers need consistent spaces before and after and should be positioned closer to the Psalm that follows than to the Psalm before.{/t}</li>
<li>{t}A 10 mm margin is okay for the outer margin. The inner margin needs slightly more, e.g. 12 mm.{/t}</li>
<li>{t}Readability would be helped by distinguishing the verse numbers visually. Others may prefer these to to be less visible.{/t}</li>
<li>{t}It would be helpful to distinguish the running head, perhaps by using a semi-bold version of the same font.{/t}</li>
<li>{t}One way in which the number of pages in a volume could be reduced without loss of readability would be to choose a font with slightly shorter ascenders and descenders and then tighten the leading (line spacing) slightly.{/t}</li>
</ol>
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
