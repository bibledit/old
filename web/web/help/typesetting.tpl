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

<h2>{t}The typesetting{/t}</h2>
<p>{t}I can do the typesetting work in LibreOffice. I then work with free software.{/t}</p>
<p>{t}My organisation may want me to use professional typesetting software, such as Adobe InDesign.{/t}</p>
<p><a href="typesettinglibreoffice.php">{t}LibreOffice{/t}</a></p>
<p><a href="typesettingindesign.php">{t}Adobe InDesign{/t}</a></p>
