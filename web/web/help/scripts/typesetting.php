<h1><?php echo gettext ("Typesetting Bibles") ?></h1>

<h2><?php echo gettext ("Introduction") ?></h2>
<p><?php echo gettext ("The Bible has been translated. It is time to publish it.") ?></p>
<p><?php echo gettext ("This article assists with typesetting the Bible.") ?></p>
<p><?php echo gettext ("At the end of all, you will have a PDF file with part of the Bible, or the whole Bible, typeset, ready for publishing.") ?></p>

<h2><?php echo gettext ("Basic assumptions") ?></h2>
<p><?php echo gettext ("The Bible is available in USFM code. The USFM code is clean and adheres to the standards.") ?></p>
<p><?php echo gettext ("You have access to your own copy of Bibledit-Web, or to the Bibledit-Web demo on the internet.") ?></p>
<p><?php echo gettext ("You have access to a copy of LibreOffice.") ?></p>

<h2><?php echo gettext ("Importing the USFM files") ?></h2>
<p><?php echo gettext ("This section is about a specific Bible that I prepare for publication. It describes the steps I take to prepare the USFM code for export.") ?></p>
<p><?php echo gettext ("The Bible is available in USFM files. Each Bible book is stored as one USFM file.") ?></p>
<p><?php echo gettext ("Create a new Bible in Bibledit-Web through menu Management / Bibles. Give the Bible a name. I call it 'Ndebele'.") ?></p>
<p><?php echo gettext ("Click the 'Ndebele' Bible.") ?></p>
<p><?php echo gettext ("Import data into it.") ?></p>
<p><?php echo gettext ("Choose the option to upload a file.") ?></p>
<p><?php echo gettext ("You can upload 66 separate files, or you can put all the 66 files into one zipped file, and then upload that zipped file. I chose the second option, as being easier and quicker than the first. Importing took about a minute. The logbook shows the progress.") ?></p>
<p><?php echo gettext ("Create a new stylesheet in menu Management / Styles. Give it a name. I call it 'Ndebele'.") ?></p>
<p><?php echo gettext ("All the data is now in Bibledit-Web. Everthing is ready for export to OpenDocument files.") ?></p>
<p><?php echo gettext ("The OpenDocument files are used as a starting point for typesetting. Typesetting itself is done in another program.") ?></p>

<h2><?php echo gettext ("Hyphenation") ?></h2>
<p><?php echo gettext ("Some of the word in the OpenDocument files are long. This results in too much space between some words.") ?></p>
<p><?php echo gettext ("The words need to get hyphens so that part of the word goes on the next line.") ?></p>
<p><?php echo gettext ("LibreOffice and Adobe InDesign have hyphenation algorithms for many languages. That would solve the problem.") ?></p>
<p><?php echo gettext ("But the programs do not have a hyphenation algorithm for our language.") ?></p>
<p><?php echo gettext ("Bibledit-Web has a hyphenater that could assist.") ?></p>
<p><?php echo gettext ("In Bibledit-Web, go to menu Management / Hyphenation.") ?></p>
<p><?php echo gettext ("Enter the first set of characters, and the second set, and save it.") ?></p>
<p><?php echo gettext ("Set the Bible where to take the data from. In this case it is 'Ndebele'.") ?></p>
<p><?php echo gettext ("Insert the soft hyphens according to the setting. It takes a while to complete.") ?></p>
<p><?php echo gettext ("The output Bible is called 'Ndebele-hyphenated'.") ?></p>
<p><?php echo gettext ("Use this Bible from now on.") ?></p>

<h2><?php echo gettext ("Exporting to OpenDocument files") ?></h2>
<p><?php echo gettext ("We are going to make the initial export just to see how the text looks.") ?></p>
<p><?php echo gettext ("In Bibledit-Web, go to menu Management / Exports.") ?></p>
<p><?php echo gettext ("Add Bible 'Ndebele' to the ones that will be exported.") ?></p>
<p><?php echo gettext ("Use stylesheet 'Ndebele'.") ?></p>
<p><?php echo gettext ("Output the chapter number in drop caps. Switch that setting on.") ?></p>
<p><?php echo gettext ("Set the page size. I use this: width: 138 mm; height: 210 mm.") ?></p>
<p><?php echo gettext ("Set the margins. I use this: 10 mm for top, bottom, and outer margins, and 12 mm for the inner margins.") ?></p>
<p><?php echo gettext ("Do not display the date in the running headers. Switch that setting off.") ?></p>
<p><?php echo gettext ("Do a fresh export.") ?></p>
<p><?php echo gettext ("The Logbook, in menu Management, shows the progress and when it is ready.") ?></p>
<p><?php echo gettext ("Go to menu Members / Downloads / Exported Bibles, and choose your Bible there. Then go to the OpenDocument folder.") ?></p>
<p><?php echo gettext ("You will see the Bible exported into several format.") ?></p>
<p><?php echo gettext ("Open them in LibreOffice and see how they look.") ?></p>
<br>
<p><?php echo gettext ("I notice that the Divine name is underlined in the OpenDocument files. The Divine name is within \\nd_...\\nd* markup in USFM. I prefer this in small capitals, not underlined.") ?></p>
<p><?php echo gettext ("This can be changed in the stylesheet.") ?></p>
<p><?php echo gettext ("Go to menu Management / Styles, and choose the 'Ndebele' stylesheet.") ?></p>
<p><?php echo gettext ("Click the 'nd' style.") ?></p>
<p><?php echo gettext ("Switch 'Underline' off.") ?></p>
<p><?php echo gettext ("Switch 'Small caps' on.") ?></p>
<p><?php echo gettext ("Do a new export, wait a while, and review the OpenDocument files again.") ?></p>
<br>
<p><?php echo gettext ("I notice that the verse numbers are in superscript. The verse numbers are small and difficult to read.") ?></p>
<p><?php echo gettext ("The intended audience prefers the verse numbers in bigger print.") ?></p>
<p><?php echo gettext ("Go to the relevant stylesheet and choose the 'v' style.") ?></p>
<p><?php echo gettext ("Switch 'Superscript' off.") ?></p>
<p><?php echo gettext ("Do a new export and review the OpenDocument files.") ?></p>
<br>
<p><?php echo gettext ("The text in the OpenDocument files has been divided into paragraphs. New paragraphs start when appropriate in the context. This is how the USFM files have been marked up.") ?></p>
<p><?php echo gettext ("The people who are going to read this Bible prefer each verse to start a new paragraph.") ?></p>
<p><?php echo gettext ("Go to the stylesheet, and choose the 'v' style.") ?></p>
<p><?php echo gettext ("Switch 'Restart paragraph' on.") ?></p>
<p><?php echo gettext ("Export again.") ?></p>
<br>
<p><?php echo gettext ("Since each verse starts a new paragraph, there is no longer any need to indent the first line of that paragraph.") ?></p>
<p><?php echo gettext ("In the stylesheet, edit the 'p' style.") ?></p>
<p><?php echo gettext ("Set the 'First line indent' to 0 mm.") ?></p>
<br>
<p><?php echo gettext ("Some verses are short. Chapters that start with short verses no longer have the chapter number in drop caps in LibreOffice.") ?></p>
<p><?php echo gettext ("This is because this verse is all on one line, and thus there is no space for drop caps, as these need two lines of text.") ?></p>
<p><?php echo gettext ("Setting the text in two columns in LibreOffice solves the problem. The drop caps are back.") ?></p>
<br>
<p><?php echo gettext ("The letters of the Bible text are too big. This takes up a lot of space. The Bible would get too many pages.") ?></p>
<p><?php echo gettext ("Find out which styles start paragraphs. In the Ndebele Bible, these are the styles p, s, nb, d.") ?></p>
<p><?php echo gettext ("Set these styles to have a font size of 10 points.") ?></p>
<br>
<p><?php echo gettext ("The letters of the notes and crossreferences are too big.") ?></p>
<p><?php echo gettext ("Set the fontsize of the styles ft and xt to 6 points.") ?></p>
<br>
<p><?php echo gettext ("Parts of the footnotes are in bold or italics or both. I prefer it all in normal script.") ?></p>
<p><?php echo gettext ("Update the relevant footnote styles and switch bold and italics off. The styles are fk, fl, fq, fqa.") ?></p>

<h2><?php echo gettext ("Typesetting guidelines") ?></h2>
<ol>
<li><?php echo gettext ("The following needs consideration if the Bible includes more than one book. Does the next book follow on the same page on which the previous book finished, or do the books follow on from one another in a different way? Whatever method is chosen, it needs to be applied consistently throughout the Bible.") ?></li>
<li><?php echo gettext ("Line spacing should be consistent throughout and should be placed line on line across all pages. It means that they should all line up with each other all the way down the page and across all pages. Then when the Bible is printed, the printed text will be back to back and not show through from one side of the page to the other. In the typesetting software, this can be implemented by setting up a grid in the document, and then aligning all text to this grid.") ?></li>
<li><?php echo gettext ("The space after the verse numbers would be the same and consistent. Bibledit-Web exports the text to OpenDocument format using 'en spaces' after each verse. This ensures a consistent width of the spaces after the verse numbers. Superscript numerals could also be considered, though larger than any note citations in the text. The size of the superscript would be between 70 and 80% of the size of the main text.") ?></li>
<li><?php echo gettext ("The alignment of the drop caps chapter numbers to the text beside it is to be consistent. There should be some space between the numbers and the text. Drop cap characters should not be too large and not rise too far above the height of the other letters on the first line. The top of the highest letters should reach to about the same height. The drop cap characters in the body of the text should not be indented.") ?></li>
<li><?php echo gettext ("The chapter divisions would also benefit from some more space between them and the text of the preceding chapter. When there is no heading above a chapter, it may be visually better to start the chapter number with a line space before it.") ?></li>
<li><?php echo gettext ("The amount of space given to the Bible book headings should not be too great.") ?></li>
<li><?php echo gettext ("Subheadings should not appear squashed amongst the rest of the text and would benefit from some space.") ?></li>
<li><?php echo gettext ("Subheadings are an addition and not Scripture, so it is appropriate to put these in italics.") ?></li>
<li><?php echo gettext ("The space between the text and footnotes should not be too tight neither unnecessarily large. Keep the gap between the references in the footer and the main bock of text as consistent as possible.") ?></li>
<li><?php echo gettext ("The footnotes and crossreferences should not have a large variation in how far they finish from the foot of the page.") ?></li>
<li><?php echo gettext ("In the crossreferences it is pleasing to have the chapter and verse number kept together with the book name, especially as the small font on long lines gives scope for this.") ?></li>
<li><?php echo gettext ("The columns should start at the same height and should be balanced at the ends.") ?></li>
<li><?php echo gettext ("The gap between the columns should be between 4 and 5 mm.") ?></li>
<li><?php echo gettext ("The centre line dividing the two columns should start about level or very slightly above the top of the highest ascenders on the first line of text in a column and finish about level with the bottom of the lowest descenders on the bottom line. The centre line may also be omitted, as most modern Bibles do. The pages looks cleaner.") ?></li>
<li><?php echo gettext ("The Psalm headers need consistent spaces before and after and should be positioned closer to the Psalm that follows than to the Psalm before.") ?></li>
<li><?php echo gettext ("A 10 mm margin is okay for the outer margin, and for the top and bottom margin. The inner margin needs slightly more, e.g. 12 mm.") ?></li>
<li><?php echo gettext ("For singable Psalms, readability would be helped by distinguishing the verse numbers visually.") ?></li>
<li><?php echo gettext ("It is helpful to distinguish the running head, perhaps by using a semi-bold version of the same font.") ?></li>
<li><?php echo gettext ("One way in which the number of pages in a volume could be reduced without loss of readability would be to choose a font with slightly shorter ascenders and descenders and then tighten the leading (line spacing) slightly.") ?></li>
<li><?php echo gettext ("When producing PDF output, the text needs to be monochrome black, not black consisting of three or four colours.") ?></li>
<li><?php echo gettext ("The last line of a verse or paragraph should not appear on its own at the top of a column. The typesetting software will have a setting for 'orphans' to control this. An elegant way of avoiding orphans is to increase or decrease the tracking of the text. This is the overall spacing of the letters in a paragraph or on a line.") ?></li>
<li><?php echo gettext ("Avoid hyphenations in the headings.") ?></li>
<li><?php echo gettext ("Many book titles consist of a main title and a subtitle, like 'The gospel of Matthew'. 'Matthew' is the main title, and the rest is the subtitle. It is good if there is a clear contrast on size between these two. Say 24 points bold for the main title, and 14 points regular for the subtitle.") ?></li>
<li><?php echo gettext ("The number of hyphens used in a row should be few for enhanced readability. The number of hyphens used in a row should not amount to more than three consecutive hyphenated lines. At times a word that is now hyphenated could be forced down onto the next line without opening up the gaps between the words too much on the line above. Hyphenation should be set to the optimum result for the column width but then it is usually necessary to go over every page and make corrections manually.") ?></li>
<li><?php echo gettext ("The main page numbering should start at the first page of Genesis, or Matthew in case of New Testaments.") ?></li>
<li><?php echo gettext ("To avoid what are sometimes referred to as 'rivers' in the text. This is where large spaces between words cause rivers of white space to be seen running through the text. The use of hyphenation or tracking adjustments are tools that can be used to minimise the problem. But as always they need to be used with discretion.") ?></li>
<li><?php echo gettext ("As a general rule text will be justified on the left and the right.") ?></li>
<li><?php echo gettext ("A word, part of a word, or very short line should not appear by itself at the end of a paragraph. This result in less much white space between paragraphs or at the bottom of a page.") ?></li>
<li><?php echo gettext ("The main page numbering starts at the first page of the text of the Scripture. This first page of Scripture starts on the recto (right side of a two page spread) for left to right reading scripts and the verso (left side of a two page spread) for right to left scripts.") ?></li>
</ol>

<h2><?php echo gettext ("The typesetting") ?></h2>
<p><?php echo gettext ("I can do the typesetting work in LibreOffice. I then work with free software.") ?></p>
<p><?php echo gettext ("My organisation may want me to use professional typesetting software, such as Adobe InDesign.") ?></p>
<p><a href="typesettinglibreoffice.php"><?php echo gettext ("LibreOffice") ?></a></p>
<p><a href="typesettingindesign.php"><?php echo gettext ("Adobe InDesign") ?></a></p>
