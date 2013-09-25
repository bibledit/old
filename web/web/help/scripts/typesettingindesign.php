<?php
/*
Copyright (©) 2003-2013 Teus Benschop.

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
<h1><?php echo gettext ("Typesetting in Adobe InDesign CS5.5") ?></h1>

<h2><?php echo gettext ("Introduction") ?></h2>
<p><?php echo gettext ("See the online help about typesetting for more information and general principles.") ?></p>

<h2><?php echo gettext ("Chapter numbers in drop caps") ?></h2>
<p><?php echo gettext ("Bibledit-Web normally exports chapter numbers in bigger font in a text frame.") ?></p>
<p><?php echo gettext ("In InDesign it is helpful to have these in drop caps.") ?></p>
<ul>
<li><?php echo gettext ("Go to Bibledit-Web, menu Export.") ?></li>
<li><?php echo gettext ("There is a setting for using frames in the drop caps. Switch it off.") ?></li>
<li><?php echo gettext ("Do a fresh export.") ?></li>
</ul>

<h2><?php echo gettext ("Creating a new document") ?></h2>
<p><?php echo gettext ("Typesetting in InDesign starts with creating a new document.") ?></p>
<ul>
<li><?php echo gettext ("Choose menu File / New / Document, or press Ctrl-N.") ?></li>
<li><?php echo gettext ("In the New Document dialog, make some settings.") ?></li>
<li><?php echo gettext ("Facing Pages: on.") ?></li>
<li><?php echo gettext ("Page width: 138 mm.") ?></li>
<li><?php echo gettext ("Page height: 210 mm.") ?></li>
<li><?php echo gettext ("Number of columns: 2.") ?></li>
<li><?php echo gettext ("Gutter: 4.2 mm.") ?></li>
<li><?php echo gettext ("Top, Bottom and Outside margins: 10 mm.") ?></li>
<li><?php echo gettext ("Untick the option to set all margins the same.") ?></li>
<li><?php echo gettext ("Inside margin: 12 mm.") ?></li>
<li><?php echo gettext ("Click Save Preset, and save the settings as 'Ndebele'. Next time, use this preset to quickly make another new document with the same settings as this one.") ?></li>
<li><?php echo gettext ("Click OK to create the new document.") ?></li>
</ul>

<h2><?php echo gettext ("Setting up the baseline grid") ?></h2>
<p><?php echo gettext ("All our text will stick to the baseline grid.") ?></p>
<p><?php echo gettext ("This is a key factor to good typography.") ?></p>
<p><?php echo gettext ("My leading is 12 points, and I will set up a grid to reflect this.") ?></p>
<p><?php echo gettext ("Go to menu Edit / Preferences / Grids.") ?></p>
<p><?php echo gettext ("Start the baseline grid at 0 mm relative to the top margin.") ?></p>
<p><?php echo gettext ("Increment it every 12 points.") ?></p>
<p><?php echo gettext ("Press OK. The grid is now set up.") ?></p>
<p><?php echo gettext ("Optionally, to make it visible, go to menu View / Grids & Guides / Show Baseline Grid.") ?></p>


<h2><?php echo gettext ("Placing text") ?></h2>
<p><?php echo gettext ("I am going to show how to place Bible text in InDesign.") ?></p>
<p><?php echo gettext ("The text is available from Bibledit-Web as OpenDocument files.") ?></p>
<p><?php echo gettext ("Open the book of Genesis, the version with text and note citations, in OpenOffice.") ?></p>
<p><?php echo gettext ("Save it as a Microsoft Word 97 / 2000 / XP (.doc) document.") ?></p>
<p><?php echo gettext ("Create a new document in InDesign.") ?></p>
<p><?php echo gettext ("Go to menu File / Place, or press Ctrl-D.") ?></p>
<p><?php echo gettext ("Select the document in MS Word (.doc) format.") ?></p>
<p><?php echo gettext ("Move the cursor through the document in InDesign. Notice that the cursor shows some of the text to be placed.") ?></p>
<p><?php echo gettext ("Move the cursor to the corner of the first column where the text will start. Click there. Notice that the first bit of the text will be placed in the first column.") ?></p>
<p><?php echo gettext ("There is much more text than what fits in just one column. This extra text is not visible. What is visible, though, is a red plus near the end of the first column. This shows that more text is available.") ?></p>
<p><?php echo gettext ("I would like to place this extra text in the second column.") ?></p>
<p><?php echo gettext ("Click the Selection Tool or press V. The cursor changes to a selection arrow.") ?></p>
<p><?php echo gettext ("Select the first column, and then click the red plus near the end of the first column. Notice that the cursor will now show some text.") ?></p>
<p><?php echo gettext ("Place this text in the second column, in the same way as it was placed in the first column.") ?></p>
<p><?php echo gettext ("There is still more text than fits in the second column. I estimate that there is enough text to fill many pages.") ?></p>
<p><?php echo gettext ("I am going to insert an extra page, so that we get more space to place the text.") ?></p>
<p><?php echo gettext ("To add blank pages, I need to have the Pages palette visible. If the Pages palette is not visible, choose Window > Pages to open the palette.") ?></p>
<p><?php echo gettext ("In the Pages palette, click Create new page. A new page will be added. This is page 2. Press F12 to close the palette.") ?></p>
<p><?php echo gettext ("Page 2 shows guides for two columns. I can fill these with text in the manner as described above.") ?></p>
<p><?php echo gettext ("I can then add as many pages as are needed, and fill each page with text.") ?></p>

<h2><?php echo gettext ("Span headlines across columns") ?></h2>
<p><?php echo gettext ("The headline I have in this example is 'The first book of Moses called Genesis'. I'd like it to span across the two columns.") ?></p>
<p><?php echo gettext ("Select the left column, and drag the top to make it shorter, so creating space above it.") ?></p>
<p><?php echo gettext ("Do the same with the right column.") ?></p>
<p><?php echo gettext ("Select the text 'The first book of Moses called Genesis'. Cut it to the clipboard.") ?></p>
<p><?php echo gettext ("Click the Type tool or type Escape T.") ?></p>
<p><?php echo gettext ("Create a new text box above the two columns. Let it span across the two columns. Estimate the size it needs to be.") ?></p>
<p><?php echo gettext ("Paste the text into that text box.") ?></p>
<p><?php echo gettext ("Resize the text box so it looks smart. Then resize both columns so that all looks good.") ?></p>

<h2><?php echo gettext ("Making text align to the grid") ?></h2>
<p><?php echo gettext ("Text aligned to the grid is key to good typesetting.") ?></p>
<p><?php echo gettext ("Select all text you would like to stick to the baseline grid.") ?></p>
<p><?php echo gettext ("Go to menu Type / Paragraph, or press Alt-Ctrl-T.") ?></p>
<p><?php echo gettext ("Press the Align to baseline grid button in the bottom right hand corner.") ?></p>
<p><?php echo gettext ("The leading of the text should be equal to the baseline grid. If the leading of the text is bigger, then it is like as if you have double line spacing.") ?></p>

<h2><?php echo gettext ("Total page count") ?></h2>
<p><?php echo gettext ("The publisher may set limits to the number of pages the typeset Bible may contain.") ?></p>
<p><?php echo gettext ("In this example, we expect an entire Bible, with singable Psalms added as an appendix, to contain no more than 2000 pages. Normal range is 1500 to 1800 pages.") ?></p>
<br>
<p><?php echo gettext ("We suppose that the singable Psalms take up no more than 200 pages.") ?></p>
<p><?php echo gettext ("Usually the inclusion of the singable Psalms in a Bible will add about 10% to the number of pages.") ?></p>
<br>
<p><?php echo gettext ("The Bible will have footnotes and crossreferences, all lumped together into a single textframe at the bottom of each page.") ?></p>
<p><?php echo gettext ("Let us now measure how many pages the notes are likely to take up.") ?></p>
<p><?php echo gettext ("Create a new document in InDesign.") ?></p>
<p><?php echo gettext ("Open the file with all notes in LibreOffice. It is 00_Bible_notes.odt. Save it in the correct format for InDesign.") ?></p>
<p><?php echo gettext ("Place the text as described above. But I am going to use Autoflow this time. Autoflow will automatically create new text boxes and new page till all text has been placed. This allows us quickly to see how many pages the notes will take up.") ?></p>
<p><?php echo gettext ("This is how to use Autoflow.") ?></p>
<p><?php echo gettext ("When you are about the click where you want the text to go, keep the Shift key down.") ?></p>
<p><?php echo gettext ("I can now roughly see the total number of pages that the notes will take up.") ?></p>
<p><?php echo gettext ("This is sufficient for an estimate.") ?></p>
<br>
<p><?php echo gettext ("Let us now find out how many pages the main text body is likely to take up.") ?></p>
<p><?php echo gettext ("The text is in the file called 00_Bible_text_and_note_citations.odt.") ?></p>
<p><?php echo gettext ("Place this text and use Autoflow.") ?></p>
<p><?php echo gettext ("It appears that, on my system, all text cannot be placed at once. It will cut off after some point.") ?></p>
<p><?php echo gettext ("I will place 01_Genesis_text_and_note_citations.odt instead.") ?></p>
<p><?php echo gettext ("I can now roughly see the total number of pages that the text of Genesis will take up.") ?></p>
<p><?php echo gettext ("The size of the whole Bible is about 22 times more than the size of Genesis.") ?></p>
<br>
<p><?php echo gettext ("The font is of particular influence on the page count. This is not dealt with here.") ?></p>

<h2><?php echo gettext ("Editing a paragraph style") ?></h2>
<p><?php echo gettext ("After exporting USFM text to OpenDocument, and placing this text into InDesign, the USFM styles appear in InDesign.") ?></p>
<p><?php echo gettext ("For example, normal paragraphs usually have style 'p', and section heading 's'.") ?></p>
<p><?php echo gettext ("If you like to update a paragraph style, you may update each section heading individually, but this is a lot of work. It is faster to update the style for 's', so that this updates all paragraphs that have this style.") ?></p>
<p><?php echo gettext ("To update a paragraph style, place the cursor in it, and on the palette choose Paragraph Styles. Right-click the style you want and choose Edit.") ?></p>

<h2><?php echo gettext ("Running headers") ?></h2>
<p><?php echo gettext ("The Bible will have running headers. The headers contain the page number near the inside margin. At the right margin it shows book and chapters numbers on that page.") ?></p>
<br>
<p><?php echo gettext ("Enter the page numbers on the master pages.") ?></p>
<p><?php echo gettext ("On the Pages palette, double-click the master pages.") ?></p>
<p><?php echo gettext ("On the left master page, create a text frame, and enter the page number at the inside margin.") ?></p>
<p><?php echo gettext ("Page numbers can be found in menu Type / Insert Special Character / Marker / Current Page Number.") ?></p>
<p><?php echo gettext ("On the right master page, create a text frame, and enter the page number at the inside margin too.") ?></p>
<br>
<p><?php echo gettext ("On page 1, create a new text frame at the top, and put the book and chapter at the outside margin.") ?></p>
<p><?php echo gettext ("This text frame can be copied to each odd page as it is generated.") ?></p>
<p><?php echo gettext ("The same applies for page 2, and any subsequent even pages.") ?></p>
<br>
<p><?php echo gettext ("The page where a book starts normally does not display a page number.") ?></p>
<p><?php echo gettext ("In order to do that, apply master page [None] to that particular page.") ?></p>

<h2><?php echo gettext ("Space between drop caps and text") ?></h2>
<p><?php echo gettext ("It may be visually pleasant to have some space between the drop caps chapter numbers and the text beside it.") ?></p>
<p><?php echo gettext ("All that is needed is a fixed space after the chapter numbers. Pick any one from a hair space to an em space. The number of characters in drop caps is now increased by one. Set this value in the drop cap settings. All lines after the first will automatically line up.") ?></p>

<h2><?php echo gettext ("Managing widows or orphans") ?></h2>
<p><?php echo gettext ("Widows and orphans are words or short lines at the beginning or end of a paragraph, which are left dangling at the top or bottom of a column, separated from the rest of the paragraph.") ?></p>
<p><?php echo gettext ("There is some disagreement about the definitions of widow and orphan; what one calls a widow the other calls an orphan.") ?></p>
<p><?php echo gettext ("There are some ways to avoid widows or orphans.") ?></p>
<p><?php echo gettext ("- Adjusting the spacing between words to produce tighter or looser paragraphs.") ?></p>
<p><?php echo gettext ("- Adjusting the tracking of the text. This is the overall spacing of the letters in a paragraph or on a line. With a good font in justified paragraphs this can usually be adjusted by up to -25 or +15 without it looking too out of place.") ?></p>
<p><?php echo gettext ("- Adjusting the hyphenation of words within the paragraph.") ?></p>
<p><?php echo gettext ("- Forcing a word down onto the next line using a soft return.") ?></p>
