/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

import jolon.xml.*;
import java.util.*;
import java.io.*;

/**
 *
 * @author Daniel
 */
public abstract class XMLConverter implements Converter {

    
    private File oCollectionsFile;
    private XMLObject oXMLObject;
    public XMLConverter
            (//File collectionsFile,
            XMLObject xml)
    {
        //this.oCollectionsFile = collectionsFile;
        this.oXMLObject = xml;
    }
    public File getCollectionsFile() {
        return oCollectionsFile;
    }
    public static XMLConverter getConverter
            (//File collectionsFile,
            File xmlFile) throws IOException {

        System.out.println("Parsing " + xmlFile.getName() + "...");

        // Parse the XML file
        XMLObject xml = XMLParser.parseFile(xmlFile);

        System.out.println("Parsing " + xmlFile.getName() + " complete.");

        // Determine whether the XML file contains OSIS content.
        XMLObject parent = xml.getChild(OsisConverter.OSIS_TAG);

        XMLConverter creator = null;

        if (parent != null)
        {
                System.out.println("Converting " + xmlFile.getName() + " from OSIS format.");
                creator = new OsisConverter(parent);
                return creator;
        }
        // Determine whether the XML file contains XHTML-TE content.
        parent = xml.getChild(XhtmlTeConverter.XHTML_TAG);

        if (parent != null)
        {
            System.out.println("Converting " + xmlFile.getName() + " from XHTML-TE format.");
            creator = new XhtmlTeConverter(parent);

            return creator;
        }

        System.out.println("Converting " + xmlFile.getName() + " from ThML format.");

        parent = xml.getChild(ThmlConverter.THML_TAG);

        if (parent != null)
        {
                creator = new ThmlConverter(parent);
                return creator;
        }

        System.out.println("Error: XML file does not contain OSIS or ThML content: " + xmlFile.getName());
        return null;
    }


















    /** Identical to the keys used in the books HashMap except retains the order
        of books from the XML file (the HashMap does not retain order).
            @see parseBooks **/
    protected static Vector<String> bookNames = new Vector<String>();
    
    /**
     * No collections file was specified so generating collections file.
     */
    public static void generateCollectionsFile(File collectionsFile, String xmlFileName, HashMap books) throws IOException
    {
        System.out.println("No Collections file specified, generating: " + collectionsFile.getPath());

        PrintWriter writer = new PrintWriter(new FileWriter(collectionsFile));

        // Make the collection name the file name without extension
        int dotIndex = xmlFileName.lastIndexOf('.');
        String collectionName = xmlFileName;

        if (dotIndex > 0)
        {
                collectionName = collectionName.substring(0, dotIndex);
        }

        // Write out the required "Info:" and "Source-Text:" lines for an xhtml_te file.
        if (xmlFileName.toLowerCase().endsWith(".xhtml"))
        {
            // ToDo: Is this the right way to specify the current directory?
            writer.println("Source-Text: " + xmlFileName);
            writer.println("Source-Format: xhtml_te");
            writer.println("Info: Bible text exported from FieldWorks Translation Editor.");
        }

        writer.println("Collection: " + collectionName);

        // Print out books
        for (String bookName: bookNames)
        {
            writer.println("Book: " + bookName);
        }

        writer.close();

        System.out.println("Collections file generated.");
    }

    /**
     * Performs essentially the same as String.trim() but leaves style characters
     * intact.
     */
    public String trim(String string)
    {
            String result = string;

            if (string.length() > 0)
            {
                    // Find first non-white space character
                    char[] charArray = string.toCharArray();

                    int charIndex = 0;
                    char currentChar = charArray[charIndex];

                    while ((charIndex < charArray.length) && (charArray[charIndex] > GoBibleCreator.STYLE_RED) && (charArray[charIndex] <= ' '))
                    {
                            charIndex++;
                    }

                    int startIndex = charIndex;

                    if (startIndex < charArray.length)
                    {
                            // Find the last non-white space character
                            charIndex = charArray.length - 1;

                            while ((charIndex >= 0) && (charArray[charIndex] > GoBibleCreator.STYLE_RED) && (charArray[charIndex] <= ' '))
                            {
                                    charIndex--;
                            }

                            result = string.substring(startIndex, charIndex + 1);
                    }
                    else
                    {
                            result = "";
                    }
            }

            return result;
    }

    /**
     * Converts XML into a HashMap of Book objects indexed by book name.
     * @param xml Parsed XML.
     * @return HashMap of Book objects.
     */
    public HashMap parseBooks(XMLObject xml, String bodyTag, String testamentTags[], String bookTag, /*String bookNameAttribute,*/ String primaryBookNameAttribute, String secondaryBookNameAttribute)
    {
            HashMap books = new HashMap();

            // Flag to count how many books have no verses
            int numBooksWithOutVerses = 0;

            // The ThML body (as opposed to the head) contains the actual contents of the Bible
            XMLObject body = xml.getChild(bodyTag);

            // Go through the testaments of the ThML body looking for books
            for (Enumeration t = body.getChildren(); t.hasMoreElements();)
            {
                    XMLObject testament = (XMLObject) t.nextElement();

                    // We are only interested in testaments
                    boolean found = false;
                    for (int i=0; i < testamentTags.length; i++)
                    {
                            if (testament.getTag().equals(testamentTags[i]))
                            {
                                    found = true;
                                    break;
                            }
                    }

                    // only process if we found one of the given testament tags
                    if (found)
                    {
                            // Find all of the books in each testament
                            for (Enumeration e = testament.getChildren(); e.hasMoreElements(); )
                            {
                                    XMLObject xmlBook = (XMLObject) e.nextElement();

                                    // See if it is a book tag
                                    if (xmlBook.getTag().equals(bookTag))
                                    {
                                            // Create a new book
                                            XMLBook book = new XMLBook(xmlBook, this, primaryBookNameAttribute, secondaryBookNameAttribute);

                                            // Check to see if the book has any verses in it, log an error if it doesn't'
                                            if (!book.hasVerses())
                                            {
                                                    System.out.println("Error: Book (" + book.name+") doesn't contain any verses.");
                                                    numBooksWithOutVerses++;
                                            }
                                            // Add book to the lookup table
                                            books.put(book.name, book);

                                            bookNames.add(book.name);
                                    }
                            }
                    }
            }

            if (numBooksWithOutVerses > 0)
            {
                    //System.err.println("Error: " + numBooksWithOutVerses + ");
            }
            return books;
    }

    public HashMap parse() {
        return this.parse(this.oXMLObject);
    }
    public abstract HashMap parse(XMLObject xml);

    /**
     * Parses the contents of the chapter into the Chapter object.
     **/
    public abstract void parseChapter(XMLObject xml, Chapter chapter);

    // UTILITY FUNCTIONS THAT CAN BE USED BY SUBCLASSES

    public static String extractCDATA(XMLObject xml)
    {
            String result = "";

            for (Enumeration e = xml.getChildren(); e.hasMoreElements(); )
            {
                    XMLObject child = (XMLObject) e.nextElement();

                    if (child instanceof CDATA)
                    {
                            // Add the CDATA to the result
                            result += child.getTag();
                    }
                    // Don't include CDATA within the sup, reference, or title tags
                    else if (!child.getTag().equals("sup") && !child.getTag().equals("reference") && !child.getTag().equals("title"))
                    {
                            // The child itself may contain CDATA so check it
                            result += extractCDATA(child);
                    }
            }

            return result;
    }

    /**
     * HTML text may contain ampersand characters such as &quot; which need to be
     * replaced with the " character.
     **/
    public String convertAmpersands(String text)
    {
            String result = "";

            // Search for the next ampersand
            int semiColonIndex = -1;
            int ampersandIndex = -1;

            do
            {
                    ampersandIndex = text.indexOf('&', semiColonIndex + 1);

                    if (ampersandIndex != -1)
                    {
                            // Add the text up to the ampersand to the result
                            result += text.substring(semiColonIndex + 1, ampersandIndex);

                            // Find the end of the ampersand
                            semiColonIndex = text.indexOf(';', ampersandIndex + 1);

                            // Test the type of ampersand
                            String type = text.substring(ampersandIndex + 1, semiColonIndex);

                            if (type.equals("quot"))      { result += '\"'; }
            else if (type.equals("amp"))  { result += '&'; }
            else if (type.equals("apos")) { result += '\''; }
            else if (type.equals("lt"))   { result += '<'; }
            else if (type.equals("gt"))   { result += '>'; }
                    }
            }
            while (ampersandIndex != -1);

            // Add the final string
            result += text.substring(semiColonIndex + 1);

            return result;
    }


    /**
     * Returns true if XMLObject is a chapter.
     **/
    public abstract boolean isChapter(XMLObject xml);

    /**
     * Extracts the chapter number from the chapter.
     **/
    public abstract int getChapterNumber(XMLObject xml);

}

class XMLBook extends Book {
    //used by osis & thml file format
    public XMLBook(XMLObject book, XMLConverter creator, String primaryBookNameAttribute, String secondaryBookNameAttribute)
    {
            // Grab the book's name
            //name = book.getAttribute(bookNameAttribute, "*Error no book name*");

            // Grab the book's short name
            name = book.getAttributeAsString(primaryBookNameAttribute, null);

            if (name == null)
            {
                    if (secondaryBookNameAttribute != null)
                    {
                            name = book.getAttribute(secondaryBookNameAttribute, "*Error no book name*");
                    }
                    else
                    {
                            System.out.println("Error: no book name");
                    }
            }

            // Convert the short name to US-ASCII
            try
            {
                    byte[] shortNameBytes = name.getBytes("US-ASCII");
                    fileName = new String(shortNameBytes, "US-ASCII");

                    // Replace '?' with '_' a question mark indicates an unrepresentable character
                    String temp = fileName.replace('?', '_');
                    fileName = temp;

                    //System.out.println("Short name: " + shortName);
            }
            catch (UnsupportedEncodingException e)
            {
                    System.out.println(e.toString());
            }

            //System.out.println("Writing " + name + "...");
            int chapterNumber = 0;

            // Find each chapter
            for (Enumeration e = book.getChildren(); e.hasMoreElements(); )
            {
                    XMLObject xml = (XMLObject) e.nextElement();

                    // See if it is a chapter tag
                    if (creator.isChapter(xml))
                    {
                            // The chapter tags title attribute will indicate the chapter number
                            // It will be in the format "Chapter 107". We need to extract
                            // the chapter number as books may be split up.
                            if (startChapter == -1)
                            {
                                    chapterNumber = startChapter = creator.getChapterNumber(xml);
                            }

                            Chapter chapter = new Chapter();
                            creator.parseChapter(xml, chapter);

                            //System.out.println("Processing " + name + " " + chapterNumber + " with " + chapter.verses.size() + " verses.");
                            chapters.addElement(chapter);
                            chapterNumber++;
                    }
            }
    }

}