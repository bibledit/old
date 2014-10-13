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
<h1><?php echo Locale_Translate::_("Typesetting Bibles") ?></h1>

<h2><?php echo Locale_Translate::_("Introduction") ?></h2>
<p><?php echo Locale_Translate::_("The Bible has been translated. It is time to publish it.") ?></p>
<p><?php echo Locale_Translate::_("This article assists with typesetting the Bible.") ?></p>
<p><?php echo Locale_Translate::_("At the end of all, you will have a PDF file with part of the Bible, or the whole Bible, typeset, ready for publishing.") ?></p>

<h2><?php echo Locale_Translate::_("Basic assumptions") ?></h2>
<ul>
<li><?php echo Locale_Translate::_("The Bible is available in USFM code.") ?></li>
<li><?php echo Locale_Translate::_("The USFM code is clean and adheres to the standards.") ?></li>
<li><?php echo Locale_Translate::_("You have access to Bibledit-Web.") ?></li>
<li><?php echo Locale_Translate::_("You have OpenOffice, LibreOffice or an equivalent.") ?></li>
</ul>

<h2><?php echo Locale_Translate::_("Importing the USFM files") ?></h2>
<p><?php echo Locale_Translate::_("This section describes the steps to take to prepare the USFM code for export.") ?></p>
<p><?php echo Locale_Translate::_("Each Bible book is stored as one USFM file.") ?></p>
<ul>
<li><?php echo Locale_Translate::_("Create a new Bible in Bibledit-Web through menu Bibles. Give the Bible a name. The example calls it 'Ndebele'.") ?></li>
<li><?php echo Locale_Translate::_("Click the 'Ndebele' Bible.") ?></li>
<li><?php echo Locale_Translate::_("Import data into it.") ?></li>
<li><?php echo Locale_Translate::_("Choose the option to upload a file.") ?></li>
</ul>
<p><?php echo Locale_Translate::_("You can upload 66 separate files, or you can put all the 66 files into one zipped file, and then upload that zipped file. Import takes a while. The Journal shows the progress.") ?></p>
<ul>
<li><?php echo Locale_Translate::_("Create a new stylesheet in menu Styles. Give it a name. The example calls it 'Ndebele'.") ?></li>
</ul>
<p><?php echo Locale_Translate::_("All the data is now in Bibledit-Web. Everthing is ready for export to OpenDocument files.") ?></p>
<p><?php echo Locale_Translate::_("The OpenDocument files are used as a starting point for typesetting.") ?></p>

<h2><?php echo Locale_Translate::_("Hyphenation") ?></h2>
<p><?php echo Locale_Translate::_("Some of the word in the OpenDocument files are long. This results in too much space between some words.") ?></p>
<p><?php echo Locale_Translate::_("The words need to get hyphens so that part of the word goes on the next line.") ?></p>
<p><?php echo Locale_Translate::_("LibreOffice and Adobe InDesign have hyphenation algorithms for many languages. That would solve the problem.") ?></p>
<p><?php echo Locale_Translate::_("But the programs do not have a hyphenation algorithms for all languages.") ?></p>
<p><?php echo Locale_Translate::_("Bibledit-Web has a hyphenater that could assist.") ?></p>
<ul>
<li><?php echo Locale_Translate::_("In Bibledit-Web, go to menu Hyphenation.") ?></li>
<li><?php echo Locale_Translate::_("Enter the first set of characters, and the second set, and save it.") ?></li>
<li><?php echo Locale_Translate::_("Set the Bible where to take the data from. In this case it is 'Ndebele'.") ?></li>
<li><?php echo Locale_Translate::_("Insert the soft hyphens according to the setting. It takes a while to complete.") ?></li>
</ul>
<p><?php echo Locale_Translate::_("The output Bible is called 'Ndebele-hyphenated'.") ?></p>
<p><?php echo Locale_Translate::_("Use this Bible from now on.") ?></p>

<h2><?php echo Locale_Translate::_("Exporting to OpenDocument files") ?></h2>
<p><?php echo Locale_Translate::_("Make an initial export just to see how the text looks.") ?></p>
<ul>
<li><?php echo Locale_Translate::_("In Bibledit-Web, go to menu Exports.") ?></li>
<li><?php echo Locale_Translate::_("Add Bible 'Ndebele' to the ones that will be exported.") ?></li>
<li><?php echo Locale_Translate::_("Use stylesheet 'Ndebele'.") ?></li>
<li><?php echo Locale_Translate::_("Output the chapter number in drop caps. Switch that setting on.") ?></li>
<li><?php echo Locale_Translate::_("Set the page size. I use this: width: 138 mm; height: 210 mm.") ?></li>
<li><?php echo Locale_Translate::_("Set the margins. I use this: 10 mm for top and bottom margins, 12 mm for the outer margins, and 14 mm for the inner margins.") ?></li>
<li><?php echo Locale_Translate::_("Do not display the date in the running headers. Switch that setting off.") ?></li>
<li><?php echo Locale_Translate::_("Do a fresh export.") ?></li>
</ul>
<p><?php echo Locale_Translate::_("The Logbook shows the progress. Proceed when it is ready.") ?></p>
<ul>
<li><?php echo Locale_Translate::_("Go to menu Exports and choose your Bible there. Then go to the OpenDocument folder.") ?></li>
</ul>
<p><?php echo Locale_Translate::_("You will see the Bible exported into several formats.") ?></p>
<p><?php echo Locale_Translate::_("Open the books in LibreOffice to see how they look.") ?></p>
<br>
<p><?php echo Locale_Translate::_("The divine name will be underlined in the OpenDocument files. The divine name is within \\nd_...\\nd* markup in USFM. Some prefer this in small capitals, not underlined.") ?></p>
<p><?php echo Locale_Translate::_("This can be changed in the stylesheet.") ?></p>
<ul>
<li><?php echo Locale_Translate::_("Go to menu Styles, and choose the 'Ndebele' stylesheet.") ?></li>
<li><?php echo Locale_Translate::_("Click the 'nd' style.") ?></li>
<li><?php echo Locale_Translate::_("Switch 'Underline' off.") ?></li>
<li><?php echo Locale_Translate::_("Switch 'Small caps' on.") ?></li>
<li><?php echo Locale_Translate::_("Do a new export, wait a while, and review the OpenDocument files again.") ?></li>
</ul>
<br>
<p><?php echo Locale_Translate::_("The verse numbers are in superscript. The verse numbers are small and difficult to read.") ?></p>
<p><?php echo Locale_Translate::_("The intended audience prefers the verse numbers in bigger print.") ?></p>
<ul>
<li><?php echo Locale_Translate::_("Go to the relevant stylesheet and choose the 'v' style.") ?></li>
<li><?php echo Locale_Translate::_("Switch 'Superscript' off.") ?></li>
<li><?php echo Locale_Translate::_("Do a new export and review the OpenDocument files.") ?></li>
</ul>
<br>
<p><?php echo Locale_Translate::_("The text in the OpenDocument files has been divided into paragraphs. New paragraphs start when appropriate in the context. This is how the USFM files have been marked up.") ?></p>
<p><?php echo Locale_Translate::_("The people who are going to read this Bible prefer each verse to start a new paragraph.") ?></p>
<ul>
<li><?php echo Locale_Translate::_("Go to the stylesheet, and choose the 'v' style.") ?></li>
<li><?php echo Locale_Translate::_("Switch 'Restart paragraph' on.") ?></li>
<li><?php echo Locale_Translate::_("Export again.") ?></li>
</ul>
<br>
<p><?php echo Locale_Translate::_("Since each verse starts a new paragraph, there is no longer any need to indent the first line of that paragraph.") ?></p>
<ul>
<li><?php echo Locale_Translate::_("In the stylesheet, edit the 'p' style.") ?></li>
<li><?php echo Locale_Translate::_("Set the 'First line indent' to 0 mm.") ?></li>
</ul>
<br>
<p><?php echo Locale_Translate::_("Some verses are short. Chapters that start with short verses no longer have the chapter number in drop caps in LibreOffice.") ?></p>
<p><?php echo Locale_Translate::_("This is because this verse is all on one line, and thus there is no space for drop caps, as these need two lines of text.") ?></p>
<p><?php echo Locale_Translate::_("Setting the text in two columns in LibreOffice solves the problem. The drop caps are back.") ?></p>
<br>
<p><?php echo Locale_Translate::_("The letters of the Bible text are too big. This takes up a lot of space. The Bible would get too many pages.") ?></p>
<p><?php echo Locale_Translate::_("Find out which styles start paragraphs. In the Ndebele Bible, these are the styles p, s, nb, d.") ?></p>
<p><?php echo Locale_Translate::_("Set these styles to have a font size of 10 points.") ?></p>
<br>
<p><?php echo Locale_Translate::_("The letters of the notes and crossreferences are too big.") ?></p>
<p><?php echo Locale_Translate::_("Set the fontsize of the styles ft and xt to 6 points.") ?></p>
<br>
<p><?php echo Locale_Translate::_("Parts of the footnotes are in bold or italics or both. You may prefer it all in normal script.") ?></p>
  <p><?php echo Locale_Translate::_("Update the relevant footnote styles and switch bold and italics off. The styles are fk, fl, fq, fqa.") ?></p>

<h2><?php echo Locale_Translate::_("Typesetting guidelines") ?></h2>
<ol>
<li>
  <?php echo Locale_Translate::_("The following needs consideration if the Bible includes more than one book.") ?>
  <?php echo Locale_Translate::_("Does the next book follow on the same page on which the previous book finished, or do the books follow on from one another in a different way?") ?>
  <?php echo Locale_Translate::_("Whatever method is chosen, it needs to be applied consistently throughout the Bible.") ?>
</li>
<li>
  <?php echo Locale_Translate::_("Line spacing should be consistent throughout and should be placed line on line across all pages.") ?>
  <?php echo Locale_Translate::_("It means that they should all line up with each other all the way down the page and across all pages.") ?>
  <?php echo Locale_Translate::_("Then when the Bible is printed, the printed text will be back to back and not show through from one side of the page to the other.") ?>
  <?php echo Locale_Translate::_("In the typesetting software, this can be implemented by setting up a grid in the document, and then aligning all text to this grid.") ?>
</li>
<li>
  <?php echo Locale_Translate::_("The space after the verse numbers would be the same and consistent.") ?>
  <?php echo Locale_Translate::_("Bibledit-Web exports the text to OpenDocument format using 'en spaces' after each verse.") ?>
  <?php echo Locale_Translate::_("This ensures a consistent width of the spaces after the verse numbers.") ?>
</li>
<li>
  <?php echo Locale_Translate::_("The verse numbers should be set closer to the text that follows rather than half-way between.") ?>
  <?php echo Locale_Translate::_("Many fonts have half spaces and hair spaces that can be used for this.") ?>
  <?php echo Locale_Translate::_("Or set the kerning to a specific amount for the verse numbers.") ?>
</li>
<li>
  <?php echo Locale_Translate::_("Superscript numerals could also be considered for verse numbers.") ?>
  <?php echo Locale_Translate::_("They are less intrusive while at the same time just as readable and easily found if they are not made too small.") ?>
  <?php echo Locale_Translate::_("They should be larger than any note citations in the text.") ?>
  <?php echo Locale_Translate::_("The size of the superscript would be between 70 and 80% of the size of the main text.") ?>
</li>
<li>
  <?php echo Locale_Translate::_("The alignment of the drop caps chapter numbers to the text beside it is to be consistent.") ?>
  <?php echo Locale_Translate::_("There should be some space between the numbers and the text.") ?>
  <?php echo Locale_Translate::_("Drop cap characters should not be too large and not rise too far above the height of the other letters on the first line.") ?>
  <?php echo Locale_Translate::_("The top of the highest letters should reach to about the same height.") ?>
  <?php echo Locale_Translate::_("The drop cap characters in the body of the text should not be indented.") ?>
</li>
<li>
  <?php echo Locale_Translate::_("The chapter divisions would also benefit from some more space between them and the text of the preceding chapter.") ?>
  <?php echo Locale_Translate::_("When there is no heading above a chapter, it may be visually better to start the chapter number with a line space before it.") ?>
</li>
<li>
  <?php echo Locale_Translate::_("The amount of space given to the Bible book headings should not be too much.") ?>
  <?php echo Locale_Translate::_("But they can do with some space above and below them.") ?>
</li>
<li>
  <?php echo Locale_Translate::_("Subheadings should not appear squashed amongst the rest of the text and would benefit from some space.") ?>
</li>
<li>
  <?php echo Locale_Translate::_("Subheadings do not need to be set on the grid.") ?>
  <?php echo Locale_Translate::_("They are much better set a little closer to the text that follows.") ?>
</li>
<li>
  <?php echo Locale_Translate::_("Subheadings are an addition and not Scripture, so it is appropriate to put these in italics.") ?>
</li>
<li>
  <?php echo Locale_Translate::_("The space between the text and footnotes should not be too tight neither unnecessarily large.") ?>
  <?php echo Locale_Translate::_("Keep the gap between the references in the footer and the main bock of text as consistent as possible.") ?>
</li>
<li>
  <?php echo Locale_Translate::_("The footnotes and crossreferences should not have a large variation in how far they finish from the foot of the page.") ?>
</li>
<li>
  <?php echo Locale_Translate::_("In the crossreferences it is pleasing to have the chapter and verse number kept together with the book name, especially as the small font on long lines gives scope for this.") ?>
</li>
<li>
  <?php echo Locale_Translate::_("The columns should start at the same height and should be balanced at the ends.") ?>
</li>
<li>
  <?php echo Locale_Translate::_("The gap between the columns should be between 4 and 5 mm.") ?>
</li>
<li>
  <?php echo Locale_Translate::_("The centre line dividing the two columns should start about level or very slightly above the top of the highest ascenders on the first line of text in a column and finish about level with the bottom of the lowest descenders on the bottom line.") ?>
  <?php echo Locale_Translate::_("The centre line may also be omitted, as most modern Bibles do.") ?>
  <?php echo Locale_Translate::_("The pages looks cleaner then.") ?>
</li>
<li>
  <?php echo Locale_Translate::_("The Psalm headers need consistent spaces before and after and should be positioned closer to the Psalm that follows than to the Psalm before.") ?>
</li>
<li>
  <?php echo Locale_Translate::_("A 10 mm margin is okay for the top and bottom margins.") ?>
  <?php echo Locale_Translate::_("A 12 mm margin is good for the outer margin.") ?>
  <?php echo Locale_Translate::_("The inner margin needs slightly more, for example 14 mm.") ?>
</li>
<li>
  <?php echo Locale_Translate::_("For singable Psalms, readability would be helped by distinguishing the verse numbers visually.") ?>
</li>
<li>
  <?php echo Locale_Translate::_("It is helpful to distinguish the running head, perhaps by using a semi-bold version of the same font.") ?>
</li>
<li>
  <?php echo Locale_Translate::_("One way in which the number of pages in a volume could be reduced without loss of readability would be to choose a font with slightly shorter ascenders and descenders and then tighten the leading (line spacing) slightly.") ?>
</li>
<li>
  <?php echo Locale_Translate::_("When producing PDF output, the text needs to be monochrome black, not black consisting of three or four colours.") ?>
</li>
<li>
  <?php echo Locale_Translate::_("The last line of a verse or paragraph should not appear on its own at the top of a column.") ?>
  <?php echo Locale_Translate::_("The typesetting software will have a setting for 'orphans' to control this.") ?>
  <?php echo Locale_Translate::_("An elegant way of avoiding orphans is to increase or decrease the tracking of the text.") ?>
  <?php echo Locale_Translate::_("This is the overall spacing of the letters in a paragraph or on a line.") ?>
</li>
<li>
  <?php echo Locale_Translate::_("Avoid hyphenations in the headings.") ?>
</li>
<li>
  <?php echo Locale_Translate::_("Many book titles consist of a main title and a subtitle, like 'The gospel of Matthew'.") ?>
  <?php echo Locale_Translate::_("'Matthew' is the main title, and the rest is the subtitle.") ?>
  <?php echo Locale_Translate::_("It is good if there is a clear contrast on size between these two.") ?>
  <?php echo Locale_Translate::_("Say 24 points bold for the main title, and 14 points regular for the subtitle.") ?>
</li>
<li>
  <?php echo Locale_Translate::_("The number of hyphens used in a row should be few for enhanced readability.") ?>
  <?php echo Locale_Translate::_("The number of hyphens used in a row should not amount to more than three consecutive hyphenated lines.") ?>
  <?php echo Locale_Translate::_("At times a word that is now hyphenated could be forced down onto the next line without opening up the gaps between the words too much on the line above.") ?>
  <?php echo Locale_Translate::_("Hyphenation should be set to the optimum result for the column width but then it is usually necessary to go over every page and make corrections manually.") ?>
</li>
<li>
  <?php echo Locale_Translate::_("The main page numbering should start at the first page of Genesis, or Matthew in case of New Testaments.") ?>
</li>
<li>
  <?php echo Locale_Translate::_("To avoid what are sometimes referred to as 'rivers' in the text.") ?>
  <?php echo Locale_Translate::_("This is where large spaces between words cause rivers of white space to be seen running through the text.") ?>
  <?php echo Locale_Translate::_("The use of hyphenation or tracking adjustments are tools that can be used to minimise the problem.") ?>
  <?php echo Locale_Translate::_("But as always they need to be used with discretion.") ?>
</li>
<li>
  <?php echo Locale_Translate::_("As a general rule text will be justified on the left and the right.") ?>
</li>
<li>
  <?php echo Locale_Translate::_("A word, part of a word, or very short line should not appear by itself at the end of a paragraph.") ?>
  <?php echo Locale_Translate::_("This result in less much white space between paragraphs or at the bottom of a page.") ?>
</li>
<li>
  <?php echo Locale_Translate::_("The main page numbering starts at the first page of the text of the Scripture.") ?>
  <?php echo Locale_Translate::_("This first page of Scripture starts on the recto (right side of a two page spread) for left to right reading scripts and the verso (left side of a two page spread) for right to left scripts.") ?>
</li>
<li>
  <?php echo Locale_Translate::_("Due to subheadings starting in a new column, there may be spaces at the foot of pages.") ?>
  <?php echo Locale_Translate::_("They can be filled by carefully adjusting tracking in one of the paragraphs.") ?>
  <?php echo Locale_Translate::_("The paragraph would then take up an extra line to fill that space.") ?>
  <?php echo Locale_Translate::_("It may not always be possible to fill gaps completely without creating unsightly spacing within paragraphs.") ?>
  <?php echo Locale_Translate::_("It needs to be handled with common sense.") ?>
</li>
</ol>

<h2><?php echo Locale_Translate::_("The typesetting") ?></h2>
<p><?php echo Locale_Translate::_("The typesetting can be done in LibreOffice. This is free software.") ?></p>
<p><?php echo Locale_Translate::_("The organisation may want to use specialized typesetting software like Adobe InDesign.") ?></p>
<p><a href="typesettinglibreoffice.php"><?php echo Locale_Translate::_("LibreOffice") ?></a></p>
<p><a href="typesettingindesign.php"><?php echo Locale_Translate::_("Adobe InDesign") ?></a></p>
