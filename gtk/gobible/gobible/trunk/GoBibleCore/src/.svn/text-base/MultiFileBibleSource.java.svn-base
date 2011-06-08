//
//  MultiFileBibleSource.java
//  GoBible
//
//	Go Bible is a Free Bible viewer application for Java mobile phones (J2ME MIDP 1.0 and MIDP 2.0).
//	Copyright © 2003-2008 Jolon Faichney.
//	Copyright © 2008-2009 CrossWire Bible Society.
//
//	This program is free software; you can redistribute it and/or
//	modify it under the terms of the GNU General Public License
//	as published by the Free Software Foundation; either version 2
//	of the License, or (at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program; if not, write to the Free Software
//	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//

import java.io.*;

public class MultiFileBibleSource extends BibleSource
{
	private GoBible goBible;
	
	// Current chapter loaded
	private int currentBookIndex = -1;
	private int currentChapterIndex = -1;
	
	// Bible index
	
	private int numberOfBooks;
	private String[] bookNames;
	
	/** Short book names are for file names as they are ensured to be 7-bit ASCII. **/
	private String[] shortBookNames;
	
	// The chapter number that the book starts with, usually 1 but may be
	// a larger chapter if the book has been split
	private int[] startChapters;
	private int[] numberOfChapters;
	private int[][] numberOfVerses;
	
	// Book index
	private int[][] bookIndex;
	
	// Current chapter data
	private char[] verseData;
	
	// The verse bytes are converted to verse data but the array is reused
	// for future chapters
	private byte[] verseBytes;
	
	private int verseDataSize;
	
	public MultiFileBibleSource(GoBible goBible) throws IOException
	{
		// We record the midlet as it may be required to access resources
		this.goBible = goBible;

		// Read in the main index
		
		DataInputStream input = new DataInputStream(goBible.getClass().getResourceAsStream("Bible Data/Index"));
		
		// Read in the number of books
		numberOfBooks = input.read();
		
		bookNames = new String[numberOfBooks];
		shortBookNames = new String[numberOfBooks];
		startChapters = new int[numberOfBooks];
		numberOfChapters = new int[numberOfBooks];
		numberOfVerses = new int[numberOfBooks][];
		
		for (int bookIndex = 0; bookIndex < numberOfBooks; bookIndex++)
		{
			// Read in the name of the book
			bookNames[bookIndex] = input.readUTF();
			
			// Read in the short book name
			shortBookNames[bookIndex] = input.readUTF();
			startChapters[bookIndex] = input.read();
			
			// Read in the number of chapters in this book
			int numberOfChapters = input.read();
			this.numberOfChapters[bookIndex] = numberOfChapters;
			numberOfVerses[bookIndex] = new int[numberOfChapters];
			
			// Read in the number of verses for each chapter
			for (int chapterIndex = 0; chapterIndex < numberOfChapters; chapterIndex++)
			{
				numberOfVerses[bookIndex][chapterIndex] = input.read();
			}
		}
		
		input.close();
	}
	
	public char[] getChapter(int bookIndex, int chapterIndex) throws IOException
	{
		// Load the chapter if it isn't loaded
		loadChapter(bookIndex, chapterIndex);
		
		return verseData;
	}

	public int[] getChapterIndex(int bookIndex, int chapterIndex) throws IOException
	{
		// Load the chapter if it isn't loaded
		loadChapter(bookIndex, chapterIndex);
		
		return this.bookIndex[chapterIndex];
	}
	
	private void loadChapter(int bookIndex, int chapterIndex) throws IOException
	{
		// If the chapter isn't loaded then load it
		if (currentBookIndex != bookIndex || currentChapterIndex != chapterIndex)
		{
			// If the bible canvas is displayed then indicate that we are loading a chapter
			if (goBible.display.getCurrent() == goBible.bibleCanvas)
			{
				// Notify the application that we'll be spending some time loading
				goBible.bibleCanvas.enterLoadingMode();
			}
		
			// If the book is different then we need to load its index
			if (currentBookIndex != bookIndex)
			{
				loadBookIndex(bookIndex);
			}

			// Load the chapter as it will be different if either the chapter or book changed
			//start = System.currentTimeMillis();
			
			DataInputStream input = new DataInputStream(goBible.getClass().getResourceAsStream("Bible Data/" + shortBookNames[bookIndex] + "/" + shortBookNames[bookIndex] + " " + (chapterIndex + startChapters[bookIndex])));

			currentChapterIndex = chapterIndex;
			
			verseData = input.readUTF().toCharArray();
			verseDataSize = verseData.length;
			
			//goBible.loadChapterTime = System.currentTimeMillis() - start;
		}
	}
	
	private void loadBookIndex(int bookIndex) throws IOException
	{
		int numberOfChapters = this.numberOfChapters[bookIndex];
		this.bookIndex = new int[numberOfChapters][];
		
		currentBookIndex = bookIndex;
		
		DataInputStream input = new DataInputStream(goBible.getClass().getResourceAsStream("Bible Data/" + shortBookNames[bookIndex] + "/Index"));
		
		// Read each verse length in for each chapter
		for (int chapter = 0; chapter < numberOfChapters; chapter++)
		{
			// There are two verse entries (offset, length) for every verse
			int numberOfVerses = 2 * this.numberOfVerses[bookIndex][chapter];
			int[] chapterIndex = new int[numberOfVerses];
			this.bookIndex[chapter] = chapterIndex;
			
			int offset = 0;
			
			// Read in each verse length
			for (int verse = 0; verse < numberOfVerses; )
			{
				chapterIndex[verse++] = offset;
				offset += input.readUnsignedShort();
				chapterIndex[verse++] = offset;
			}
		}
		
		input.close();
	}
	
	public String[] getBookNames()
	{
		return bookNames;
	}
	
	public String getBookName(int bookIndex)
	{
		return bookNames[bookIndex];
	}
	
	public int getNumberOfBooks()
	{
		return numberOfBooks;
	}
	
	public int getStartChapter(int bookIndex)
	{
		return startChapters[bookIndex];
	}
	
	public int getNumberOfChapters(int bookIndex)
	{
		return numberOfChapters[bookIndex];
	}
	
	public int getNumberOfVerses(int bookIndex, int chapterIndex)
	{
		return numberOfVerses[bookIndex][chapterIndex];
	}
	
	public int getVerseDataSize()
	{
		return verseDataSize;
	}
}
