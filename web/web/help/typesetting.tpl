<h1>{t}Typesetting Bibles{/t}</h1>

<h2>{t}Introduction{/t}</h2>
<p>{t}The Bible has been translated. It is time to publish it.{/t}</p>
<p>{t}This article assists with typesetting the Bible.{/t}</p>
<p>{t}At the end of all, you will have a PDF file with part of the Bible, or the whole Bible, typeset, ready for publishing.{/t}</p>

<h2>{t}Basic assumptions{/t}</h2>
<p>{t}The Bible is available in USFM code. The USFM code is clean and adheres to the standards.{/t}</p>
<p>{t}You have access to your own copy of Bibledit-Web, or to the Bibledit-Web demo on the internet.{/t}</p>
<p>{t}You have access to a copy of LibreOffice.{/t}</p>

<h2>{t}Importing the USFM files{/t}</h2>
<p>{t}This section is about a specific Bible that I prepare for publication. It describes the steps I take to prepare the USFM code for export.{/t}</p>
<p>{t}The Bible is available in USFM files. Each Bible book is stored as one USFM file.{/t}</p>
<p>{t}Create a new Bible in Bibledit-Web through menu Management / Bibles. Give the Bible a name. I call it "Ndebele".{/t}</p>
<p>{t}Click the "Ndebele" Bible.{/t}</p>
<p>{t}Import data into it.{/t}</p>
<p>{t}Choose the option to upload a file.{/t}</p>
<p>{t}You can upload 66 separate files, or you can put all the 66 files into one zipped file, and then upload that zipped file. I chose the second option, as being easier and quicker than the first. Importing took about a minute. The logbook shows the progress.{/t}</p>
<p>{t}Create a new stylesheet in menu Management / Styles. Give it a name. I call it "Ndebele".{/t}</p>
<p>{t}All the data is now in Bibledit-Web. Everthing is ready for export to OpenDocument files.{/t}</p>
<p>{t}The OpenDocument files are used as a starting point for typesetting. Typesetting itself is done in another program.{/t}</p>

<h2>{t}Hyphenation{/t}</h2>
<p>{t}Some of the word in the OpenDocument files are long. This results in too much space between some words.{/t}</p>
<p>{t}The words need to get hyphens so that part of the word goes on the next line.{/t}</p>
<p>{t}LibreOffice and Adobe InDesign have hyphenation algorithms for many languages. That would solve the problem.{/t}</p>
<p>{t}But the programs do not have a hyphenation algorithm for our language.{/t}</p>
<p>{t}Bibledit-Web has a hyphenater that could assist.{/t}</p>
<p>{t}In Bibledit-Web, go to menu Management / Hyphenation.{/t}</p>
<p>{t}Enter the first set of characters, and the second set, and save it.{/t}</p>
<p>{t}Set the Bible where to take the data from. In this case it is "Ndebele".{/t}</p>
<p>{t}Insert the soft hyphens according to the setting. It takes a while to complete.{/t}</p>
<p>{t}The output Bible is called "Ndebele-hyphenated".{/t}</p>
<p>{t}Use this Bible from now on.{/t}</p>

<h2>{t}Exporting to OpenDocument files{/t}</h2>
<p>{t}We are going to make the initial export just to see how the text looks.{/t}</p>
<p>{t}In Bibledit-Web, go to menu Management / Exports.{/t}</p>
<p>{t}Add Bible "Ndebele" to the ones that will be exported.{/t}</p>
<p>{t}Use stylesheet "Ndebele".{/t}</p>
<p>{t}Output the chapter number in drop caps. Switch that setting on.{/t}</p>
<p>{t}Set the page size. I use this: width: 138 mm; height: 210 mm.{/t}</p>
<p>{t}Set the margins. I use this: 10 mm for top, bottom, and outer margins, and 12 mm for the inner margins.{/t}</p>
<p>{t}Do not display the date in the running headers. Switch that setting off.{/t}</p>
<p>{t}Do a fresh export.{/t}</p>
<p>{t}The Logbook, in menu Management, shows the progress and when it is ready.{/t}</p>
<p>{t}Go to menu Members / Downloads / Exported Bibles, and choose your Bible there. Then go to the OpenDocument folder.{/t}</p>
<p>{t}You will see the Bible exported into several format.{/t}</p>
<p>{t}Open them in LibreOffice and see how they look.{/t}</p>
<br>
<p>{t}I notice that the Divine name is underlined in the OpenDocument files. The Divine name is within \\nd_...\\nd* markup in USFM. I prefer this in small capitals, not underlined.{/t}</p>
<p>{t}This can be changed in the stylesheet.{/t}</p>
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
<p>{t}The letters of the Bible text are too big. This takes up a lot of space. The Bible would get too many pages.{/t}</p>
<p>{t}Find out which styles start paragraphs. In the Ndebele Bible, these are the styles p, s, nb, d.{/t}</p>
<p>{t}Set these styles to have a font size of 10 points.{/t}</p>
<br>
<p>{t}The letters of the notes and crossreferences are too big.{/t}</p>
<p>{t}Set the fontsize of the styles ft and xt to 6 points.{/t}</p>
<br>
<p>{t}Parts of the footnotes are in bold or italics or both. I prefer it all in normal script.{/t}</p>
<p>{t}Update the relevant footnote styles and switch bold and italics off. The styles are fk, fl, fq, fqa.{/t}</p>

<h2>{t}Typesetting guidelines{/t}</h2>
<ol>
<li>{t}Line spacing should be consistent throughout and should be placed line on line across all pages. It means that they should all line up with each other all the way down the page and across all pages. Then when the Bible is printed, the printed text will be back to back and not show through from one side of the page to the other. In the typesetting software, this can be implemented by setting up a grid in the document, and then aligning all text to this grid.{/t}</li>
<li>{t}The space after the verse numbers would be the same and consistent. Bibledit-Web exports the text to OpenDocument format using 'en spaces' after each verse. This ensures a consistent width of the spaces after the verse numbers. Superscript numerals could also be considered, though larger than any note citations in the text. The size of the superscript would be between 70 and 80% of the size of the main text.{/t}</li>
<li>{t}The alignment of the drop caps chapter numbers to the text beside it is to be consistent. There should be some space between the numbers and the text. Drop cap characters should not be too large and not rise too far above the height of the other letters on the first line. The top of the highest letters should reach to about the same height.{/t}</li>
<li>{t}The chapter divisions would also benefit from some more space between them and the text of the preceding chapter. When there is no heading above a chapter, it may be visually better to start the chapter number with a line space before it.{/t}</li>
<li>{t}The amount of space given to the Bible book headings should not be too great.{/t}</li>
<li>{t}Subheadings should not appear squashed amongst the rest of the text and would benefit from some space.{/t}</li>
<li>{t}Subheadings are an addition and not Scripture, so it is appropriate to put these in italics.{/t}</li>
<li>{t}The space between the text and footnotes should not be too tight neither unnecessarily large. Keep the gap between the references in the footer and the main bock of text as consistent as possible.{/t}</li>
<li>{t}In the crossreferences it is pleasing to have the chapter and verse number kept together with the book name, especially as the small font on long lines gives scope for this.{/t}</li>
<li>{t}The columns should start at the same height and should be balanced at the ends.{/t}</li>
<li>{t}The gap between the columns should be between 4 and 5 mm.{/t}</li>
<li>{t}The centre line dividing the two columns should start about level or very slightly above the top of the highest ascenders on the first line of text in a column and finish about level with the bottom of the lowest descenders on the bottom line. The centre line may also be omitted, as most modern Bibles do. The pages looks cleaner.{/t}</li>
<li>{t}The Psalm headers need consistent spaces before and after and should be positioned closer to the Psalm that follows than to the Psalm before.{/t}</li>
<li>{t}A 10 mm margin is okay for the outer margin, and for the top and bottom margin. The inner margin needs slightly more, e.g. 12 mm.{/t}</li>
<li>{t}For singable Psalms, readability would be helped by distinguishing the verse numbers visually.{/t}</li>
<li>{t}It would be helpful to distinguish the running head, perhaps by using a semi-bold version of the same font.{/t}</li>
<li>{t}One way in which the number of pages in a volume could be reduced without loss of readability would be to choose a font with slightly shorter ascenders and descenders and then tighten the leading (line spacing) slightly.{/t}</li>
<li>{t}When producing PDF output, the text needs to be monochrome black, not black consisting of three or four colours.{/t}</li>
<li>{t}The last line of a verse or paragraph should not appear on its own at the top of a column.{/t}</li>
<li>{t}Avoid hyphenations in the headings.{/t}</li>
<li>{t}Many book titles consist of a main title and a subtitle, like "The gospel of Matthew". "Matthew" is the main title, and the rest is the subtitle. It is good if there is a clear contrast on size between these two. Say 24 points bold for the main title, and 14 points regular for the subtitle.{/t}</li>
<li>{t}The number of hyphens used in a row should be few for enhanced readability. At times a word that is now hyphenated could be forced down onto the next line without opening up the gaps between the words too much on the line above. Hyphenation should be set to the optimum result for the column width but then it is usually necessary to go over every page and make corrections manually.{/t}</li>
</ol>

<h2>{t}The typesetting{/t}</h2>
<p>{t}I can do the typesetting work in LibreOffice. I then work with free software.{/t}</p>
<p>{t}My organisation may want me to use professional typesetting software, such as Adobe InDesign.{/t}</p>
<p><a href="typesettinglibreoffice.php">{t}LibreOffice{/t}</a></p>
<p><a href="typesettingindesign.php">{t}Adobe InDesign{/t}</a></p>
