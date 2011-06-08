/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

import java.util.*;

/**
 * Represents a book from the XML file or a book to be created as part of a collection.
 * Use the XML constructor to create a book from the ThML file. This parse the entire book.
 * Use the name, startChapter, endChapter constructor to create a book that represents
 * a book in a collection that is to be written out as a Go Bible data file.
 **/
public class Book {

    public GoBibleCreator creator;

    /** Book name. **/
	public String name;

	/** Short book name used for file names as it is guaranteed to be 7-bit ASCII. **/
	public String fileName;

	/** Chapters parsed from ThML file of type Chapter. **/
	public Vector chapters = new Vector();

	/** Start chapter if not 1, else -1. **/
	public int startChapter = -1;

	/** End chapter if not 1, else -1. **/
	public int endChapter = -1;


        protected Book() {}

	/**
	 * Constructor for creating a book that represents a book in a collection to be created.
	 * @param name Short name of book.
	 * @param startChapter The chapter to start the book, otherwise -1 for whole chapter.
	 * @param endChapter The chapter to end the book, otherwise -1 for whole chapter.
	 **/
	public Book(String name, int startChapter, int endChapter)
	{
		this.name = name;
		this.startChapter = startChapter;
		this.endChapter = endChapter;
	}


	public boolean hasVerses()
	{
		int verseSize = 0;
		for (int i = 0; i < chapters.size(); i++)
		{
			Chapter c = (Chapter) chapters.elementAt(i);

			if (GoBibleCreator.EmptyVerseString == null)
			{
				verseSize += c.allVerses.length();
			}
			else
			{
				for (Enumeration e = c.verses.elements(); e.hasMoreElements(); )
				{
					String verse = (String) e.nextElement();
					// this isn't just the filler for an empty verse, so count it as data and stop looking
					if (!GoBibleCreator.EmptyVerseString.equals(verse) && verse.length() > 0)
					{
						verseSize += verse.length();
						break;
					}
				}
			}
			if (verseSize > 0)
				break;
		}
		return verseSize > 0;
	}

}