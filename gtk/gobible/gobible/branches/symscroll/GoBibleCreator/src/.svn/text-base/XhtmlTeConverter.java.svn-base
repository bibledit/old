//
//  XhtmlTeConverter.java
//  GoBibleCreator
//
//  Created by Larry Waswick on Feb 19 2010, based on OsisConverter by Jolon Faichney.
//  For the glory of our Lord Jesus Christ and the furtherance of His Kingdom.
//  This file is placed into the public domain.
//

import java.io.*;
import java.util.*;
import java.util.jar.*;
import jolon.xml.*;

public class XhtmlTeConverter extends XMLConverter
{

    public XhtmlTeConverter
            (//File collectionsFile,
            XMLObject xml)
    {
        super(xml);
    }





	/** Tag to identify Xhtml-TE XML data. **/
	public final static String XHTML_TAG = "html";

	/** XhtmlTE tag that contains all of the testaments. **/
	public final static String XHTML_BODY_TAG = "body";

	/** XhtmlTE tag that contains a testament (New Testament, Old Testament, Aprocrypha, etc). **/
	public final static String TESTAMENT_TAG[] = {"div"}; // class="testament"
	
	/** XhtmlTE tag that contains a book (Psalms, Mark, etc). **/
	public final static String BOOK_TAG = "div"; // class="scrBook"

	/** OSIS attribute within the BOOK_TAG that contains the book name. **/
	public final static String BOOK_NAME_ATTRIBUTE = "title";
	
	/** OSIS attribute within the BOOK_TAG that contains the short book name. 
		It is assumed that the contents of this attribute will be Latin and 
		will be converted to US-ASCII. **/
	public final static String BOOK_SHORT_NAME_ATTRIBUTE = "title";

	/** OSIS tag that contains one chapter. **/
	public final static String CHAPTER_TAG_CHAPTER = "div"; // class="chapter"
	public final static String CHAPTER_TAG_ATTRIBUTE = "chapter";

	/** OSIS attribute within the CHAPTER_TAG that contains the chapter number (eg "Gen.1"). **/
	public final static String CHAPTER_NUMBER_ATTRIBUTE = "title";

	/** Verse data is contained within the verse tag. **/
	public final static String VERSE_TAG = "div";

	public HashMap parse(XMLObject xhtml)
	{
		// Call the GoBibleCreator convert method with the 
		// XHTML specific tags
		
		// Extract the books from the XHTML indexed by book name
		return parseBooks(xhtml, XHTML_BODY_TAG, TESTAMENT_TAG, BOOK_TAG, BOOK_NAME_ATTRIBUTE, null);
	}
	
	/**
	 * XHTML-TE chapters are inside a "div" tag.
	 **/
	public boolean isChapter(XMLObject xml)
	{
		String tag = xml.getTag();
        String tempString = xml.getAttribute("class", "none");
        Boolean isChapter = tag.equals(CHAPTER_TAG_CHAPTER) && tempString.equals(CHAPTER_TAG_ATTRIBUTE);
		return isChapter;
	}

	/**
	 * OSIS chapter titles are assumed to end with a period then the chapter number.
	 **/
	public int getChapterNumber(XMLObject xml)
	{
		String chapterNumber = xml.getAttribute(CHAPTER_NUMBER_ATTRIBUTE, "*Error no " + CHAPTER_NUMBER_ATTRIBUTE + " attribute*");
		
		return Integer.parseInt(chapterNumber);
	}

	public void parseChapter(XMLObject xml, Chapter chapter)
	{
		//int verse = 1;
		
		// Find each verse
		for (Enumeration e = xml.getChildren(); e.hasMoreElements(); )
		{
			XMLObject xmlVerse = (XMLObject) e.nextElement();
			
			//System.out.println("Tag: " + xml.getTag());
			
			// See if it is a verse tag
			if (xmlVerse.getTag().equals(VERSE_TAG))
			{
				//System.out.println("Parsing verse " + verse++);
				
				// Extract verse data and add verse
				String verseString = extractCDATA(xmlVerse);
                int indexOfAmp = verseString.indexOf('&');

        		// Convert HTML ampersand characters if the verse data contains one.
                if (indexOfAmp >= 0)
                {
                    verseString = convertAmpersands(verseString);
                }

				chapter.verses.addElement(verseString);		
				chapter.allVerses.append(verseString);
			}
		}		
	}
}
