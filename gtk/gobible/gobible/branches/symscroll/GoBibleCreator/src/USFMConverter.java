/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

import java.io.*;
import java.util.*;
import java.util.regex.*;

/**
 *
 * @author Daniel
 */
public class USFMConverter implements Converter {


    private File oCollectionsFile;
    private String oBaseSourceDirectory;
    private String oSourceTextPath;

    public USFMConverter
        (File collectionsFile,
        String baseSourceDirectory,
        String sourceTextPath)
    {
        this.oCollectionsFile = collectionsFile;
        this.oBaseSourceDirectory = baseSourceDirectory;
        this.oSourceTextPath = sourceTextPath;
    }
    public HashMap parse() throws IOException {
        return this.parseUSFM2(oCollectionsFile, oBaseSourceDirectory, oSourceTextPath);
    }
    public File getCollectionsFile() {
        return oCollectionsFile;
    }









        // the extension that is used by the USFM files
        // introduced in version 2.3
        protected static String usfmSourceFileExtension = null;

        // flag to determine if red lettering is used
        // introduced in version 2.3
	protected static boolean useRedLettering = true;

        // listing of the possible types of supported source file formats
        // introduced in version 2.3
        protected static String sTitleTag = "\\h";

	/** Identical to the keys used in the books HashMap except retains the order
	    of books from the XML file (the HashMap does not retain order).
		@see parseBooks **/
	protected static Vector<String> bookNames = new Vector<String>();


        public USFMConverter() {};

        public static HashMap parseUSFM2(File collectionsFile, String baseSourceDirectory, String sourceTextPath ) throws IOException {

            extractCommonFields(collectionsFile);
            //make sure that the file extension is retrieved
            extractUSFMfields(collectionsFile);
            return parseUSFM(baseSourceDirectory, sourceTextPath, sTitleTag);
        }
    /*
	 * This is a common place where globals / common elements can be read from
	 * the collections file that apply to all data types. It does mean the file
	 * is opened and read an additional time, but this process isn't meant to be
	 * fast of efficient - but rather correct.
	 *
	 */
	public static void extractCommonFields(File collectionsFile) throws IOException
	{
		// Open the file for reading one line at a time in UTF-8 character encoding
		LineNumberReader reader = new LineNumberReader(new InputStreamReader(new FileInputStream(collectionsFile), "UTF-8"));

		String line = null;
		// Read the collections in the file
		while ((line = reader.readLine()) != null)
		{
			if (line.startsWith("RedLettering:"))
			{
				//defaults to true
				if (line.toLowerCase().indexOf("false") > 0 || line.toLowerCase().indexOf("no") > 0)
				{
					useRedLettering = false;
				}
			}
			// Empth verse text
			else if (line.startsWith("Empty-Verse-Text:"))
			{
				// Grab text to display where the verse text is missing
				GoBibleCreator.EmptyVerseString = line.substring(17).trim(); /* TODO: ugly static variable usage */
			}
		}
		reader.close();

		return;
	}



        /*
	 * @return the extension of the the USFM files
	 */
	public static void extractUSFMfields(File collectionsFile) throws IOException
	{
		// Open the file for reading one line at a time in UTF-8 character encoding
		LineNumberReader reader = new LineNumberReader(new InputStreamReader(new FileInputStream(collectionsFile), "UTF-8"));

		String line = null;
		// Read the collections in the file
		while ((line = reader.readLine()) != null)
		{
                    // Test if line contains the source text property
                    if (line.startsWith("Source-FileExtension:"))
                    {
                        usfmSourceFileExtension =  line.substring("Source-FileExtension:".length()).trim().toLowerCase();
                        //check for starting period and strip off
                        usfmSourceFileExtension = usfmSourceFileExtension.replaceAll("\\.", "");
                    }
                    if (line.startsWith("USFM-TitleTag:"))
                    {
                        sTitleTag  =  line.substring("USFM-TitleTag:".length()).trim().toLowerCase();
                    }
                    if (line.startsWith("RedLettering:"))
                    {
                        //defaults to true
                        if (line.toLowerCase().indexOf("false") > 0 || line.toLowerCase().indexOf("no") > 0)
                        {
                            useRedLettering = false;
                        }
                    }
		}
		reader.close();

		return;
	}


        /*
	 * Parses the USFM files format and extracts and returns
	 * books as a HashMap.
	 */
	public static HashMap parseUSFM(String baseSourceDirectory, String sourceTextPath, String sTitleTag) throws IOException
	{
            //iterate through the source file directory and get the filenames present
            //USFM uses a predefined file format of:
            // 01GENxxxx.ptx
            HashMap books = new HashMap();
            if (baseSourceDirectory == null)
            {
                baseSourceDirectory = "";
            }

            try
            {
                System.out.println("Base Dir: " + baseSourceDirectory);
                System.out.println("Source Path: " + sourceTextPath);

                //get the list of files in the directory
                File folder = new File(baseSourceDirectory, sourceTextPath);
                File[] listOfFiles = folder.listFiles();

                for (int i = 0; i < listOfFiles.length; i++)
                {
                  if (listOfFiles[i].isFile())
                  {
                      String sFileName = listOfFiles[i].getName();
                      if (sFileName.toLowerCase().endsWith("." + usfmSourceFileExtension))
                      {
                          System.out.println("File " + listOfFiles[i].getName());
                          String sFilename = folder.toString() + "\\" + listOfFiles[i].getName();
                          // Create a new book
                          USFMBook book = new USFMBook(sFilename, GoBibleCreator.STYLE_RED, GoBibleCreator.fileCodepage, useRedLettering, sTitleTag);
                          book.fileName = listOfFiles[i].getName();

                          // Add book to the lookup table
                          try
                          {
                            books.put(book.name, book);
                          }
                          catch (Exception e)
                          {
                              System.out.println("Error while inserting book into books: " + e.getMessage());
                          }
                          bookNames.add(book.name);
                      }
                  }
                }
            }
            catch (Exception e)
            {
              System.out.println("Error: " + e.getMessage());
                e.printStackTrace();
            }
            return books;
	}


    /** USFM Parsing Utility Functions */


    /*
     * Check to see if the line starts with certain
     * tags that allow us to delete the entire line
     * like '\cl A'
     *
     * Return 'true' if we keep
     */
    public static boolean CheckIfWeKeepLine(String sTmp)
    {
        boolean bKeep = true;
        String sMarker[] = {
            "\\cl","\\cp", "\\qa", "\\sr",
            "\\mte", "\\mt", "\\mr ", "\\sr ",
            "\\s", "\\r "};
        //loop through the markers removing them all from the text
        for (int i = 0; i < sMarker.length; i++)
        {
            if (sTmp.startsWith(sMarker[i]))
            {
                return false;
            }
        }
        return bKeep;
    }

    public static String HighlightSectionTags(String sTmp) {
        String sSectionTags[] = {"(\\s*)\\\\ms[0-9]+(\\s*)"};
        for (int i = 0; i < sSectionTags.length; i++)
        {
            //replace tag (trailing blank remove)
            sTmp = sTmp.replaceAll(sSectionTags[i], " \\\\ms1 ? ");
        }
        return sTmp;
    }

    /*
     * This function takes a string and removes the USFM markers
     * which are singular (do not have and end tag).  Example
     * of this is the paragraph tag \p which is not needed
     */
    public static String RemoveSingularMarkerTags(String sTmp)
    {
        String sTagDesc[] = {"\\\\sp", "\\\\d"};
        for (int i = 0; i < sTagDesc.length; i++)
        {
            //replace tag (trailing blank remove)
            sTmp = sTmp.replaceAll(sTagDesc[i] + " ", "\\\\d ? ");
        }

        // markers + number >> (delete)
        //for ones that are a tag and number '\q1'
        String sTagNum[] = {"\\pi", "\\ph"};
        for (int i = 0; i < sTagNum.length; i++)
        {
            //replace tag with trailing number (trailing blank remove)
            sTmp = sTmp.replaceAll("\\" + sTagNum[i] + "+[0-9] ", "");
            //replace tag with trailing number
            sTmp = sTmp.replaceAll("\\" + sTagNum[i] + "[0-9]+", "");
            //replace tag without trailing number (trailing blank remove)
            sTmp = sTmp.replaceAll("\\" + sTagNum[i] + " ", "");
            //replace tag without trailing number
            sTmp = sTmp.replaceAll("\\" + sTagNum[i], "");
        }
        // markers >> newline
        //NOTE: you need four '\' to account for the \p string
        //as this is a regex command
        String sMarker[] = {"\\pmo", "\\pm", "\\pmc",
        "\\pmr", "\\mi", "\\nb", "\\cls", "\\pc", "\\pr",
        "\\qr", "\\qc", "\\pb", "\\b", "\\m", "\\p", "\\z", "\\tr"};

        //loop through the markers removing them all from the text
        for (int i = 0; i < sMarker.length; i++)
        {
            //replace the marker tag with an empty string (trailing blank remove)
            //sTmp = sTmp.replaceAll("\\" + sMarker[i] + " ", "");
            //replace the marker tag with an empty string
            sTmp = sTmp.replaceAll("\\" + sMarker[i] + "( )?", "\n");
        }

        //for ones that do not start with the slash
        String sSpecial[] = {"!\\$"};
        for (int i = 0; i < sSpecial.length; i++)
        {
            //replace the marker tag with an empty string (trailing blank remove)
            sTmp = sTmp.replaceAll(sSpecial[i] + " ", "");
            //replace the marker tag with an empty string
            sTmp = sTmp.replaceAll(sSpecial[i], "");
        }

        // newline >> newline
        String sSpecial2[] = {"//"};
        for (int i = 0; i < sSpecial.length; i++)
        {
            //replace the marker tag with a newline (trailing blank remove)
            sTmp = sTmp.replaceAll(sSpecial2[i] + " ", "\n");
            //replace the marker tag with a newline
            sTmp = sTmp.replaceAll(sSpecial2[i], "\n");
        }

        // markers >> newline + indent
        /* In this section we want to use \q_n_ marks to indent the text by
         * _n_ number of spaces
         */
        //for ones that are a tag and number '\q1', and we want to indent
        // by replacing with corresponding number of (non-breakable) spaces.
        String sTagNumIndent[] = {"\\q"};
        StringBuffer result = new StringBuffer();
        int lastIndex = 0;
        for (int i = 0; i < sTagNumIndent.length; i++)
        {
            java.util.regex.Pattern p = Pattern.compile("\\" + sTagNumIndent[i] + "([0-9]*) ");
            java.util.regex.Matcher mt = p.matcher(sTmp);
            while (mt.find()) {
                result.append(sTmp.substring(lastIndex, mt.start()));
                result.append("\n");
                String numberPart = mt.group(1);
                int length=1;
                if (numberPart.length() > 0) {
                    try {
                        length = Integer.parseInt(numberPart);
                        if (length == 0) { // ensure minimum
                            length = 1;
                        }
                    }catch (NumberFormatException nfe) {
                    }
                }
                while (length -- >= 0) {
                    result.append(' ');
                }
                lastIndex = mt.end();
            }
            result.append(sTmp.substring(lastIndex));
            sTmp = result.toString();
        }

        // markers + number >> newline + "..." (USFM tables)

        // note: first cell does not need formatting, since
        // line break has been made by \tr
        sTmp = sTmp.replaceAll("\\\\tc1 ", "");
        sTmp = sTmp.replaceAll("\\\\tc[0-9]+ ", "\n...");
        sTmp = sTmp.replaceAll("\\\\tc ", "");

        // There is some difficulty adding formatting
        // to the table headers, e.g. boldface (since we do not know
        // where the end of the cell is). No formatting for now.
        sTmp = sTmp.replaceAll("\\\\th1 ", "");
        sTmp = sTmp.replaceAll("\\\\th[0-9]+ ", "\n...");
        sTmp = sTmp.replaceAll("\\\\th ", "");
        
        // markers + number >> newline
        String sTagNum2[] = {"\\li", "\\qm", "\\q"};
        for (int i = 0; i < sTagNum2.length; i++)
        {
            //replace tag with trailing number (trailing blank remove)
            //sTmp = sTmp.replaceAll("\\" + sTagNum2[i] + "+[0-9] ", "");
            //replace tag with trailing number
            sTmp = sTmp.replaceAll("\\" + sTagNum2[i] + "[0-9]+", "\n");
            //replace tag without trailing number (trailing blank remove)
            //sTmp = sTmp.replaceAll("\\" + sTagNum2[i] + " ", "");
            //replace tag without trailing number
            sTmp = sTmp.replaceAll("\\" + sTagNum2[i], "\n");
        }

        return sTmp;
    }

    /*
     * This function takes a string and removes the USFM markers
     * which are double (have start and end tags).  Example
     * of this is the bold tag \bd...\bd* in which we want to
     * remove the tags only but keep the text between
     */
    public static String RemoveDoubleMarkerTags(String sTmp)
    {
        //the list of markers that this applies to
        String sMarker[] = {"\\qs", "\\qac", "\\dc",
        "\\ord", "\\sig", "\\sls",
        "\\tl", "\\em", "\\bd", "\\it", "\\bdit", "\\no", "\\sc", "\\k"};
        //loop through the markers removing them all from the text
        for (int i = 0; i < sMarker.length; i++)
        {
            sTmp = sTmp.replaceAll("\\" + sMarker[i] + "\\*", "");
            //remove trailing blank from tag
            sTmp = sTmp.replaceAll("\\" + sMarker[i] + " ", "");
            sTmp = sTmp.replaceAll("\\" + sMarker[i], "");
        }
        return sTmp;
    }

    public static final String tags[] = {
        "wj",
        "add",
        "nd",
        "qt",
        "pn"
    };
    public static final char controlChars[] = {
        '\01',
        '\02',
        '\03',
        '\04',
        '\05'
    };

    /*
     * This funtion finds the USFM marker for
     * ADDED, WJ, ND, QT, PN text
     * and replaces them with corresponding markers
     */
    public static String ReplaceMarkers(String sTmp)
    {
        //replace the marker tag with the red lettering tag
        //NOTE: you must delete the * ending tag first before the
        //start tag otherwise you end up with only the * left

        for (int i = 0; i < tags.length; i++ ) {
            sTmp = sTmp.replaceAll("(\\s*)\\\\" + tags[i] + "\\*(\\s*)", Character.toString(controlChars[i]) + " ");
            sTmp = sTmp.replaceAll("(\\s*)\\\\" + tags[i] + "(\\s*)", " " + Character.toString(controlChars[i]));
            /* There must be a space after these tags, e.g. "\wj " because otherwise the
             * tags become ambiguous. However, this space interferes with proper rendering.
             * On the other hand, there need not be a space after \wj* because the
             * "*" unambiguously marks the end of the tag.
             *
             * We preserve the leading space but discard any trailing space.
             *
             * At least based on the files I have.
             */
        }

        return sTmp;
    }

    /*
     * This funtion finds the USFM marker for words of Jesus
     * and replaces them with the red lettering tag
     */
    public static String ReplaceWordsOfJesus(String sTmp, char sWJ, boolean useRedLettering)
    {
        //replace the marker tag with the red lettering tag
        //NOTE: you must delete the * ending tag first before the
        //start tag otherwise you end up with only the * left
        if (useRedLettering)
        {
            //insert the red lettering character
            sTmp = sTmp.replaceAll("\\\\wj\\*", Character.toString(sWJ));
            sTmp = sTmp.replaceAll("\\\\wj", Character.toString(sWJ));
        }
        else
        {
            //just dump the tags
            sTmp = sTmp.replaceAll("\\\\wj\\*", "");
            sTmp = sTmp.replaceAll("\\\\wj", "");
        }
        return sTmp;
    }

    /*
     * This function takes a string and removes the USFM markers
     * which are double (have start and end tags).  Example
     * of this is the footnote tag \f...\f* in which we want to
     * remove the tags and all the text between
     */
    public static String RemoveDoubleMarkerTagsFull(String sTmp)
    {
        //the list of markers that this applies to
        //note that order is important here so that \fm is processed
        //before \f
        String sMarker[] = {"\\ca", "\\va", "\\vp", "\\fe", "\\bk",
        "\\xdc", "\\fdc", "\\fm", "\\fig", "\\ndx", "\\pro",
        "\\wg", "\\wh", "\\f", "\\w", "\\x", "\\rq", "\\xot", "\\xnt", "\\iqt"};
        //loop through the markers removing them all from the text
        for (int i = 0; i < sMarker.length; i++)
        {
            //find if the start tag is found
            while (sTmp.indexOf(sMarker[i]) != -1)
            {
                int iStart = sTmp.indexOf(sMarker[i]);
                int iEnd = sTmp.indexOf(sMarker[i] + "*");
                if (iStart != -1 && iEnd != -1)
                {
                    //replace the marker tag with an empty string
                    String sFirstPart = sTmp.substring(0, iStart);
                    String sSecondPart = sTmp.substring(iEnd + sMarker[i].length() + 1);
                    //insert a space if needed here
//                    if(sFirstPart.endsWith(" ") || sSecondPart.startsWith(" "))
//                    {
//                        //blank exists between parts
//                        sTmp = sFirstPart + sSecondPart;
//                    }
//                    else
//                    {
//                        //slap a blank between the parts
//                        sTmp = sFirstPart + " " + sSecondPart;
//                    }
                    sTmp = sFirstPart + sSecondPart;
                }
                else
                {
                    //keep this from going into an endless loop
                    //if the end tag is missing
                    break;
                }
            }
        }

        return sTmp;
    }

    /*
     * Drop the verse number from the start of the verse string
     */
    public static String RemoveVerseNumbering(String sVerse)
    {
        //find the first verse and remove everything before that
        int iStart = sVerse.indexOf(" ");
        if (iStart == -1)
        {
            //there is no space after the verse number which happens when
            //there are multiline verses and somebody is putting in blank
            //verse lines
            sVerse = "..";
        }
        else
        {
            sVerse = sVerse.substring(iStart + 1);
        }
        return sVerse;
    }

    /*
     * Parse through the book name and try and normalize it to standard characters
     * which won't cause a problem with the hashmap lookup
     */
    public static String CleanBookName(String sTmp)
    {
        /*
        //try to dump string into hashmap and then retrieve what came out
        //anything that has \\ufff we can replace as being a bad character
        java.util.HashMap h = new java.util.HashMap();
        h.put(sTmp.trim(), 0);
        String sHash = h.toString();
        sHash = sHash.substring(1, sHash.length()-3);
        //look for the bad characters and replace them out
        sTmp = "";
        for (int i=0; i<sHash.length(); i++)
        {
            char c = sHash.charAt(i);
            String s = Long.toHexString((long)c).toUpperCase();
            //bad character if it has the FFF
            if (!s.startsWith("FFF"))
            {
                sTmp += Character.toString(c);
            }
        }
        */
        return sTmp;
    }
    
}

class USFMBook extends Book {

    //USFM file format
    public USFMBook(String sFilename, char cSTYLE_RED, String fileCodepage, boolean useRedLettering, String sTitleTag)
    {
        //open up the filename for parsing through line-by-line
        try
        {
            //convert source text to file
            File fIn = new File(sFilename);
            if (fileCodepage == null)
            {
                fileCodepage = "UTF-8";
            }
            fileName = fIn.getName();
            //read in up to the first 200 lines and see if you can spot the tags
            BufferedReader readerIn = new BufferedReader(new InputStreamReader(new FileInputStream(fIn), fileCodepage));
            String lineIn;
            StringBuffer strBuff = new StringBuffer();

            while ((lineIn = readerIn.readLine()) != null)
            {
                //check if it is line we want to keep or not
                if(USFMConverter.CheckIfWeKeepLine(lineIn))
                {
                    if (lineIn.startsWith("\\"))
                    {
                        //some line that starts with a verse
                        strBuff.append(lineIn);
                    }
                    else
                    {
                        //probably a verse continuation line so append a space
                        strBuff.append(" " + lineIn);
                    }
                }
            }
            readerIn.close();

            //find the book name from the tags
            String sHtag = "";
            int iStart = strBuff.indexOf(sTitleTag);
            if (iStart != -1)
            {
                //grab a section of text
                if(strBuff.length() >= iStart + 200)
                {
                    sHtag = strBuff.substring(iStart + 2, iStart + 200);
                }
                else
                {
                    sHtag = strBuff.substring(iStart + 2);
                }
                //parse out the actual information by finding the next tag
                //and grabbing the text between
                iStart = sHtag.indexOf("\\");
                name = USFMConverter.CleanBookName(sHtag.substring(1, iStart).trim());
            }

            if (sHtag.equals(""))
            {
                System.out.println("ERROR: Can not process the following book as there are no " + sTitleTag + " tags");
                System.out.println("Filename:" + sFilename);
                System.out.println("Please remove the book from the collection or fix the tags...");
            }

            String strBook = USFMConverter.HighlightSectionTags(strBuff.toString());
            //split up the book into chapters
            String[] sChapters = strBook.split("(\\\\ms1|\\\\c) ");
            //the [0] index should be header information that we can
            //just ignore - we can check for sure by finding the first
            //index that starts with "1"
            for (int i = 0; i < sChapters.length; i++)
            {
                String trimmed = sChapters[i].trim();
                if (trimmed.startsWith("1") || trimmed.startsWith("?"))
                {
                    iStart = i;
                    break;
                }
            }

            StringBuffer chPrefix = null;

            int numberingOffsetCounter = 0;
            //loop through each chapter pulling out the verses
            int iChapter = 0;
            int trackedChapterNumber = 0;
            for (int i = iStart; i < sChapters.length; i++)
            {
                if (startChapter == -1)
                {
                    startChapter = i;
                }
                iChapter++;
                Pattern chNumberPattern = Pattern.compile("^\\s*(\\d+)\\s*");
                Matcher chNumberMatcher = chNumberPattern.matcher(sChapters[i]);
                try {
                    if (chNumberMatcher.find()) {
                        trackedChapterNumber ++;
                        String chNumberString = sChapters[i].substring(chNumberMatcher.start(1), chNumberMatcher.end(1));
                        int chNumber = Integer.parseInt(chNumberString);

                        if (chNumber != trackedChapterNumber) {
                            System.err.println("ASSERTION FAILED in source file " + sFilename);
                            System.err.println("Chapter number specified in source file, " + chNumber + ", does not match our count, " + trackedChapterNumber);
                            System.exit(0);
                        }
                        sChapters[i] = sChapters[i].substring(chNumberMatcher.end());
                    }
                    else if (sChapters[i].startsWith("? ")) {
                        System.err.println(sChapters[i]);
                        if (chPrefix == null) {
                            chPrefix = new StringBuffer();
                        }
                        chPrefix.append('\07');
                        chPrefix.append(sChapters[i].substring(2));
                        chPrefix.append('\07');
                        continue;
                    }
                    else {
                        System.err.println("WARNING: Chapter number not found. Syntax error? After Ch " + iChapter);
                        System.err.println("String is: " + sChapters[i].substring(0,30));
                                System.exit(0);
                    }
                }
                catch(NumberFormatException nfe) {
                    nfe.printStackTrace();
                    System.err.println("WARNING: Chapter number not found");
                                System.exit(0);
                }

                //create a new chapter
                Chapter chapter = new Chapter();
                String sTmp = sChapters[i].toString();

                //sTmp = USFM_Utilities.RemoveChapterHeaders(sTmp);
                //remove all the double tags that we can just blow away
                sTmp = USFMConverter.RemoveDoubleMarkerTags(sTmp);
                //remove all the double tags that we need to blow away
                //the text between the verses
                sTmp = USFMConverter.RemoveDoubleMarkerTagsFull(sTmp);
                //get rid of all the singular tags in the text that do
                //not have an ending tag e.g. \p
                sTmp = USFMConverter.RemoveSingularMarkerTags(sTmp);
                //remove all the double tags that we can just blow away
                //sTmp = ReplaceWordsOfJesus(sTmp, cSTYLE_RED, useRedLettering);

                // TODO: need a way to control 'useRedLettering' and 'cSTYLE_RED'
                // now they are hard-coded in USFM
                sTmp = USFMConverter.ReplaceMarkers(sTmp);

                //we should have just verse tags left
                // But as opposed to the previous version, there may be
                // information contained BEFORE the verses, BETWEEN the
                // verses and in many other places.
                //parse out all the verses into individual ones

                // Split either on description tag or verse tag
                /* From the code in RemoveSingularMarkerTags,
                 *  \v nn ttt   --> \v nn ttt
                 *  \d ttt      --> \d ? ttt.
                 * This allows us to determine whether the verse is a verse
                 * or a header. If it is a header, store it first.
                 */

                String[] sVerses = sTmp.split("(\\\\d|\\\\v) ");
                String verseString = "";
                int trackedVerseIndex = 0; // Check
                StringBuffer prefixBuffer = new StringBuffer();

                for (int j = 0; j < sVerses.length; j++)
                {
                    //System.err.println(" doing para #" + ": " + sVerses[j].substring(0, Math.min(10, sVerses[j].length())));
                    //System.out.println("Chap:[" + i + "]  Verse:[" + j + "]");
                    verseString = sVerses[j];

                    Pattern verseNumberPattern = Pattern.compile("^(\\d+)\\s");
                    Matcher verseNumberMatcher = verseNumberPattern.matcher(verseString);

                    boolean hasVerseNumber = verseNumberMatcher.find();
                    boolean hasHeader = verseString.startsWith("? ");

                    if (hasVerseNumber) {
                        try { // Test whether verse number is correct
                            trackedVerseIndex ++;
                            String verseNumberString = verseString.substring(verseNumberMatcher.start(), verseNumberMatcher.end()-1);
                            int verseIndex = Integer.parseInt(verseNumberString);
                            if (verseIndex != trackedVerseIndex) {
                                System.err.println("ASSERTION FAILED in source file " + sFilename);
                                System.err.println("Verse number specified in source file, " + verseIndex + ", does not match our count, " + trackedVerseIndex);
                                System.err.println("String is: " + verseString);
                                if (j > 0) {
                                System.err.println("Previous verse: " + sVerses[j - 1]);
                                }
                                System.out.println("Reference is: " + name + " " + String.valueOf(iChapter) + ":" + String.valueOf(j)
                            + " in file: " + sFilename);

                                while (verseIndex < trackedVerseIndex + numberingOffsetCounter ) {
                                    numberingOffsetCounter --;
                                }
                                while (verseIndex > trackedVerseIndex + numberingOffsetCounter ) {
                                    numberingOffsetCounter ++;
                                }
                            }
                        } catch (NumberFormatException nfe) {
                            System.err.println("Could not parse the verse number: " + nfe.getMessage());
                            System.err.println("String is: " + verseString);
                            System.err.println("Reference is: " + name + " " + String.valueOf(iChapter) + ":" + String.valueOf(j)
                            + " in file: " + sFilename);
                                System.exit(0);

                        }

                        //parse out the verse number in the beginning
                        //of the verse
                        verseString = verseString.substring(verseNumberMatcher.end());
                        //check for any remaining USFM tags...
                        if (verseString.indexOf("\\") != -1)
                        {
                            int iTagStart = verseString.indexOf("\\");
                            int iTagEnd = verseString.indexOf(" ", iTagStart);
                            if (iTagEnd == -1)
                            {
                                iTagEnd = verseString.length();
                            }
                            System.out.println("EXTRA UNPROCESSED TAGS: [" + verseString.substring(iTagStart, iTagEnd)
                                    + "] " + name + " " + String.valueOf(iChapter) + ":" + String.valueOf(j)
                                    + " in file: " + sFilename);
                                System.exit(0);
                        }
                        if (verseString.length() == 0 && GoBibleCreator.EmptyVerseString != null) {
                            verseString = GoBibleCreator.EmptyVerseString;
                        }

                        // verse numbering offsets
                        if (numberingOffsetCounter < 0) {
                            for (int k = 0; k > numberingOffsetCounter; k-- ) {
                                prefixBuffer.insert(0, (char)0x0e);
                            }
                        }
                        else if (numberingOffsetCounter > 0) {
                            for (int k = 0; k < numberingOffsetCounter; k++ ) {
                                prefixBuffer.insert(0, (char)0x0f);
                            }
                        }

                        // section headings (only for Psalms - Book I, Book II)
                        if (chPrefix != null) {
                            prefixBuffer.insert(0, chPrefix);
                            chPrefix = null;
                        }

                        prefixBuffer.append(verseString);
                        chapter.verses.addElement(prefixBuffer.toString());
                        chapter.allVerses.append(prefixBuffer);

                        prefixBuffer = new StringBuffer();
                    }
                    else if (hasHeader) {
                        verseString = verseString.substring(2);
                        if (verseString.indexOf("\\") != -1)
                        {
                            int iTagStart = verseString.indexOf("\\");
                            int iTagEnd = verseString.indexOf(" ", iTagStart);
                            if (iTagEnd == -1)
                            {
                                iTagEnd = verseString.length();
                            }
                            System.out.println("EXTRA UNPROCESSED TAGS: [" + verseString.substring(iTagStart, iTagEnd)
                                    + "] " + name + " " + String.valueOf(iChapter) + ":" + String.valueOf(j)
                                    + " in file: " + sFilename);
                                System.exit(0);
                        }
                        if (verseString.length() == 0 && GoBibleCreator.EmptyVerseString != null) {
                            verseString = GoBibleCreator.EmptyVerseString;
                        }

                        prefixBuffer.append("\06");
                        prefixBuffer.append(verseString);
                        prefixBuffer.append("\06");
                    }
                    else if (verseString.matches("\\s*")) {}
                    else {
                        System.err.println("No verse number, no header number. Please investigate.");
                        System.err.println("String is: " + verseString);
                        System.out.println("Reference is: " + name + " " + String.valueOf(iChapter) + ":" + String.valueOf(j)
                            + " in file: " + sFilename);
                                System.exit(0);

                    }
                }
                if (prefixBuffer.length() != 0) {

                }
                chapters.addElement(chapter);
            }
        }
        catch(Exception e)
        {
            System.out.println("Error: " + e.getMessage());
            e.printStackTrace();
        }
    }

}