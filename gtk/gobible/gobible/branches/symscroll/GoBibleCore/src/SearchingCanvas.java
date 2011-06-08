//
//  SearchingCanvas.java
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

import java.util.*;
import javax.microedition.lcdui.*;

public class SearchingCanvas extends Canvas implements CommandListener, Runnable
{
	private final static int EXTRA_DISPLAY_CHARS = 10;
	private int MAX_RESULTS = 50;

	private GoBible goBible;
	private boolean stop;
	private int currentChapter, currentBook, currentVerse;
	private int lastChapter, lastBook, lastVerse;
	private int searchFromBookIndex, searchToBookIndex;
	private String searchString;

        private SearchOptions options;
        
        private boolean jumpToResult = false;
        private boolean directionForward = false; // not used
        private boolean exhausted = false;

	//private static Hashtable searchStringJump = new Hashtable();
	public SearchingCanvas(GoBible goBible, final int fromBookIndex, final int toBookIndex, final String searchString)
	{
            this (
                    goBible,
                    new SearchOptions(fromBookIndex, toBookIndex, searchString, fromBookIndex, 0, 0)
                    );
	}
        /**
         * Restores the search from a snapshot.
         * 
         * @param goBible
         * @param snapshot
         */
	public SearchingCanvas(GoBible goBible, SearchOptions snapshot)
	{
            this.goBible = goBible;

            /* We will be displaying a list of the results. Hence, limit to some manageable value
            default: 50 */
            try {
                MAX_RESULTS = Integer.parseInt(goBible.getAppProperty("Go-Bible-Max-Results"));
            } catch (NumberFormatException nfe) {} catch (NullPointerException npe) { }

            goBible.searchResults.removeAllElements();
            goBible.lastSearchIndex = 0;	// dlh - have to reset it for the new search

            this.searchFromBookIndex = snapshot.fromBook;
            this.searchToBookIndex = snapshot.toBook;
            this.searchString = snapshot.searchString;
            addCommand(new Command(GoBible.getString("UI-Stop"), Command.STOP, 0));
            setCommandListener(this);

            currentBook = snapshot.currentBook;
            currentChapter = snapshot.currentChapter;
            currentVerse = snapshot.currentVerse;

            snapshot.dump();

            // Spawn a search thread
            new Thread(this).start();
	}

        /** Creates the Canvas for a quick backward/forward search. This
         * circumvents the size limitation of the cache
         *
         * @param goBible           The GoBible midlet.
         * @param searchString      The search string
         * @param directionForward  true if it is a forward search, false otherwise.
         */
        protected SearchingCanvas(GoBible goBible, final String searchString, boolean directionForward) {
            this.jumpToResult = true;
            this.directionForward = directionForward;
            this.goBible = goBible;

            this.searchFromBookIndex = goBible.currentBookIndex;
            this.searchToBookIndex = goBible.bibleSource.getNumberOfBooks() - 1;
            this.searchString = searchString;

            addCommand(new Command(GoBible.getString("UI-Stop"), Command.STOP, 0));
            setCommandListener(this);

            System.err.println("SEARCH " + goBible.currentBookIndex + ":" + goBible.currentChapterIndex + ":" + goBible.currentVerseIndex);

            currentBook = goBible.bibleCanvas.currentBook();
            currentChapter = goBible.bibleCanvas.currentChapter();
            currentVerse = goBible.bibleCanvas.currentVerse();
        }
	public static void quickSearch(GoBible goBible, final String searchString, boolean directionForward)
	{
            new SearchingCanvas(goBible, searchString, directionForward).run();
            goBible.display.setCurrent(goBible.bibleCanvas);
	}
	
	public void commandAction(Command command, Displayable display)
	{
		switch (command.getCommandType())
		{
			case Command.STOP:
			{
                            if (jumpToResult) {
                                // wanted only ONE result but could not be found in time.
                                goBible.showMainScreen();
                            }
                            else {
                                stop = true;
                                
                                // Show search results screen
                                goBible.display.setCurrent(new SearchResultsList(goBible, goBible.searchResults, getSearchSnapshot()));
                                break;
                            }
			}
		}
	}

        /**
         * saves the list of search options into a utility class
         * 
         * @return
         */
        private SearchOptions getSearchSnapshot() {
            SearchOptions opts = new SearchOptions(
                    this.searchFromBookIndex,
                    this.searchToBookIndex,
                    this.searchString,
                    this.lastBook,
                    this.lastChapter,
                    this.lastVerse
                    );
            opts.exhausted = exhausted;
            return opts;
        }
        /**
         * Resumes the search where we left off
         * Only applicable if we have a result list.
         * 
         * @return @true if there are possibly more results, @false otherwise.
         * "Possibly" means the search was halted because the max number of results
         * was reached, or because it was stopped by the user.
         */
        public boolean nextResultSet() {
            goBible.display.setCurrent(this);
            goBible.searchResults.removeAllElements();
            
            // Identify the reasons for stopping
            if (stop || numberFound == MAX_RESULTS) {
                // probably more results
                new Thread(this).start();
                return true;
            }
            else {
                return false;
            }
        }

        private int numberFound;
        
	/**
	 * This search method is called from a thread created in the constructor.
	 * Searches all books from fromBookIndex to toBookIndex.
	 */
	public void run()
	{
            try {
            char[] searchData = searchString.toLowerCase().toCharArray();

            // Search each book
            int endBook = searchToBookIndex;

            int numberOfBooks = goBible.bibleSource.getNumberOfBooks();

            this.numberFound = 0;

            // We use a different variable to count through the books since
            // currentBook is also used in paint and the index can end up
            // past the last book due to the bookIndex++ in the for loop before
            // the bookIndex != endBook + 1 condition. currentBook is assigned to
            // bookIndex within the loop ensuring that it is always valid.
            int bookIndex = currentBook;

            for (
                ;
                ( (bookIndex != endBook + 1) || !(exhausted = true) ) // if we reached the last book, set exhausted = true
                    && (!stop) // user did not stop process
                    && (numberFound < MAX_RESULTS); // results list not full
                bookIndex++, currentChapter = 0
                )
            {
                // Check if bookIndex is greater than the number of books,
                // if so then wrap back to the beginning.
                // eg. if searching from Jude to Exodus.
                if (bookIndex >= numberOfBooks)
                {
                        bookIndex = 0;
                }

                currentBook = bookIndex;

                // Repaint the screen for every book
                repaint();

                // Search each chapter
                int endChapter = goBible.bibleSource.getNumberOfChapters(currentBook) - 1;

                for (; (currentChapter <= endChapter) && (!stop) && (numberFound < MAX_RESULTS); currentChapter++, currentVerse = -1)
                {
                    // Repaint the screen for every chapter
                    //repaint();
                    //serviceRepaints();

                    // Grab the chapter data and look for verses that contain the search text
                    try
                    {
                        char[] verseData = goBible.bibleSource.getChapter(currentBook, currentChapter);
                        int[] verseIndex = goBible.bibleSource.getChapterIndex(currentBook, currentChapter);

                        //String verseString = new String(verseData).toLowerCase();

                        int numberOfVerses = goBible.bibleSource.getNumberOfVerses(currentBook, currentChapter);

                        int index = 0;
                        int end = goBible.bibleSource.getVerseDataSize();

                        // Do not search in the current verse (it might have been a previous search result)
                        currentVerse ++;

                        // Stop when we have searched all verses or when no search string was found
                        for (; (currentVerse < numberOfVerses) && (index >= 0) && (!stop) && (numberFound < MAX_RESULTS); currentVerse++)
                        {
                            int verseStart = verseIndex[currentVerse << 1];
                            int verseEnd = verseIndex[(currentVerse << 1) + 1];

                            lastVerse = currentVerse;
                            lastChapter = currentChapter;
                            lastBook = currentBook;

                            // If the next found instance was in a previous verse then look
                            // for it in this one
                            if ((index < verseStart) || (index == 0))
                            {
                                //index = verseString.indexOf(searchString, verseStart);
                                index = find(verseData, verseStart, end, searchData);
                            }

                            // if we found a match
                            if (index >= 0)
                            {

                                // true if the index starts in this verse and contains the match
                                boolean containedInVerse = index >= verseStart && (index + searchData.length -1) < verseEnd;


                                // if the search can cross verse boundries AND it starts in this verse OR
                                //    the search can't cross verse boundries AND it starts and ends in this verse
                                if ((goBible.searchCanSpanMultipleVerses && index < verseEnd) ||
                                                (!goBible.searchCanSpanMultipleVerses && containedInVerse))
                                {

                                    if (jumpToResult) {
                                        // TODO: Weird threading issues with this block of code.
                                        // Disabling threads here for now
                                        // Probably because BibleCanvas.paint() is still running,
                                        // using the goBible to navigate, while we navigate goBible
                                        // here and there here as well.

                                        System.err.println("SEARCH FOUND AT " + currentBook + ":" + currentChapter + ":" + currentVerse);
                                        System.err.println("SEARCH INDEX IS " + (index - verseStart));
                                        goBible.bibleCanvas.requestPassage(currentBook, currentChapter, currentVerse);
                                        return;
                                    }
                                    // We want to extract 10 characters before and after the
                                    // word. However, we also want to know whether how many
                                    // characters we are missing from the verse before
                                    // and after the extra 10 characters. If one side
                                    // of the search result doesn't use all 10 characters
                                    // eg. it is at the end of the verse, then we can
                                    // use these values to shift characters over to the
                                    // other side.

                                    int extraCharsBefore = (index - verseStart) - EXTRA_DISPLAY_CHARS;
                                    int extraCharsAfter = verseEnd - (index + searchData.length + EXTRA_DISPLAY_CHARS);

                                    int beforeIndex = extraCharsBefore >= 0 ? index - EXTRA_DISPLAY_CHARS : verseStart;
                                    int afterIndex = extraCharsAfter >= 0 ? index + searchData.length + EXTRA_DISPLAY_CHARS : verseEnd;

                                    // If there are spare characters which weren't used after
                                    // then use them before if they are needed
                                    if (extraCharsAfter < 0 && beforeIndex > verseStart)
                                    {
                                            beforeIndex += extraCharsAfter;
                                            beforeIndex = beforeIndex < verseStart ? verseStart : beforeIndex;
                                    }

                                    // If there are spare characters which weren't used before
                                    // then use them after if they are needed
                                    if (extraCharsBefore < 0 && afterIndex < verseEnd)
                                    {
                                            afterIndex -= extraCharsBefore;
                                            afterIndex = afterIndex > verseEnd ? verseEnd : afterIndex;
                                    }

                                    // If the first character is a letter then decrement the
                                    // beforeIndex until a non-letter is found, this ensures
                                    // that the extracted doesn't start with half a word
                                    if (isLetter(verseData[beforeIndex]))
                                    {
                                            while (beforeIndex > verseStart && isLetter(verseData[beforeIndex - 1]))
                                            {
                                                    beforeIndex--;
                                            }
                                    }

                                    // If the last character is a letter then increment the
                                    // afterIndex until a non-letter is found, this ensures
                                    // that the extracted doesn't end with half a word
                                    if (isLetter(verseData[afterIndex - 1]))
                                    {
                                            while (afterIndex < verseEnd && isLetter(verseData[afterIndex]))
                                            {
                                                    afterIndex++;
                                            }
                                    }

                                    // Add verse to search results replacing control characters with a space
                                    StringBuffer sanitizedString = new StringBuffer(afterIndex - beforeIndex);
                                    for (int k = beforeIndex; k < afterIndex; k++) {
                                        if (verseData[k] >= ' ') {
                                            sanitizedString.append(verseData[k]);
                                        }
                                    }

                                    goBible.searchResults.addElement(
                                            new Object[] {
                                        new Integer((currentBook << 16) | (currentChapter << 8) | (currentVerse)),
                                        sanitizedString.toString()
                                    });

                                    numberFound++;

                                    // Repaint screen when a new verse has been found
                                    repaint();
                                    serviceRepaints();

                                    // Look for next instance of verse
                                    //index = verseString.indexOf(searchString, index + 1);
                                    index = find(verseData, index + 1, end, searchData);
                                }
                            }
                        }
                    }
                    catch (Exception e)
                    {
                    }
                }
            }

            if (goBible.searchResults.size() > 0)
            {
                // Show search results screen
                goBible.display.setCurrent(new SearchResultsList(goBible, goBible.searchResults, getSearchSnapshot()));
            }
            else
            {
                // Revert to the previously displayed verse
                goBible.display.setCurrent(goBible.bibleCanvas);
                goBible.showMainScreen();
            }
            }catch (NullPointerException npe) {
                npe.printStackTrace();
            }
	}

	/**
	 * This method returns true if we have an ASCII letter 'a' - 'z' ignoring case.
	 * @param c : character to test
	 * @return True if it's a letter.
	 */
	private final static boolean isLetter(char c)
	{
		return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
	}
	
	/**
	 * Finds the first occurrence of the searchData string within
	 * the data string starting at the offset specified.
	 */
	private final static int find(char[] data, int offset, int end, char[] searchData)
	{
		//return findUnicode(data, offset, end, searchData);

		int result = -1;
		int searchStringLength = searchData.length;
		int totalEnd = end - searchStringLength;
		char lowerDiff = (char) ('a' - 'A');
		
		for (int i = offset; i <= totalEnd && result == -1; i++)
		{
                        
			// ignore control characters
			if (data[i] < ' ')
				continue;

			char currentChar = data[i];
			
			// Convert to lower case
			if (currentChar >= 'A' && currentChar <= 'Z')
			{
				currentChar += lowerDiff;
			}
			
			// Quickly test the first character, if it doesn't match then we
			// can quickly move onto the next one
			if (currentChar == searchData[0])
			{
                                int skipCount = 0; // counts number of control characters skipped
                                boolean skipSpace = false;
                                
				boolean matches = true;
				
				for (int j = 1; j < searchStringLength + skipCount && matches; j++)
				{
					// ignore all code points below ' '
                                        // ignore if it is a space preceded by a space (multiple spaces)
                                        //   (e.g. indents)
					if (data[i + j] < ' ')
                                        {
                                                skipCount ++;
						continue;
                                        }
                                        else if (GBCToolkit.isSpace(data[i + j]) // All (remaining) unicode spaces
                                                || data[i + j] == 0x3000) { // and the Chinese ideographic space
                                                    // which has been left out for reasons mentioned in GBCToolkit.java
                                                if (skipSpace) {
                                                    skipCount ++;
                                                    continue;
                                                }
                                                skipSpace = true;
                                        }
                                        else {
                                                skipSpace = false;
                                        }

					currentChar = data[i + j];
                                        
//                                        if (skipCount != 0) {
//                                            System.err.println("Comparing " + currentChar + " with " + searchData[j - skipCount]);
//                                        }
				
					// Convert to lower case
					if (currentChar >= 'A' && currentChar <= 'Z')
					{
						currentChar += lowerDiff;
					}

					if (currentChar != searchData[j - skipCount])
					{
						matches = false;
					}
				}
				
				if (matches)
				{
					result = i;
				}
			}
		}
		
		return result;
	}


//#if (DO_CASE_FOLDING)	// when this is defined, use the Character methods for case folding
//# 	/**
//# 	 * Finds the first occurance of the searchData string within
//# 	 * the data string starting at the offset specified.
//# 	 */
//# 	private final static int findUnicode(char[] data, int offset, int end, char[] searchData)
//# 	{
//# 		int result = -1;
//# 		int searchStringLength = searchData.length;
//# 		int totalEnd = end - searchStringLength;
//#
//# 		for (int i = offset; i <= totalEnd && result == -1; i++)
//# 		{
//# 			// ignore the U+001 code point - it's the toggle for red letter style
//# 			if (data[i] == 0x01)
//# 				continue;
//# 			// Convert to the lowercase equivalent of the character, if any;
//# 			//  otherwise, the character itself is returned
//# 			char currentChar = Character.toLowerCase(data[i]);
//#
//# 			// Quickly test the first character, if it doesn't match then we
//# 			// can quickly move onto the next one
//# 			if (currentChar == searchData[0])
//# 			{
//# 				boolean matches = true;
//#
//# 				for (int j = 1; j < searchStringLength && matches; j++)
//# 				{
//# 					// ignore the U+001 code point - it's the toggle for red letter style
//# 					if (data[i+j] == 0x01)
//# 						continue;
//# 					// Convert to lower case
//# 					currentChar = Character.toLowerCase(data[i+j]);
//#
//# 					if (currentChar != searchData[j])
//# 					{
//# 						matches = false;
//# 					}
//# 				}
//#
//# 				if (matches)
//# 				{
//# 					result = i;
//# 				}
//# 			}
//# 		}
//#
//# 		return result;
//# 	}
//#endif

	public void paint(Graphics g)
	{
		int width = getWidth();
		
		g.setColor(0xffffff);
		g.fillRect(0, 0, width, getHeight());

		Font font = Font.getFont(Font.FACE_SYSTEM, Font.STYLE_BOLD, Font.SIZE_MEDIUM);

		g.setFont(font);
		
		g.setColor(0);
		int halfWidth = width >> 1;
		
		g.drawString(GoBible.getString("UI-Searching") + ":", halfWidth, 20, Graphics.HCENTER | Graphics.TOP);
		
		// Grab the book name
		String bookName = goBible.bibleSource.getBookName(currentBook);
		//String chapterString = " " + (currentChapter + goBible.bibleSource.getStartChapter(goBible.currentBookIndex));
		
		// Grab the width of the book name
		int bookNameWidth = font.stringWidth(bookName);
		int totalWidth = font.stringWidth(bookName/* + chapterString*/);
		
		// Draw the text centred so determine starting position
		int startX = (width - totalWidth) >> 1;
                
		g.drawString(bookName, startX, 40, Graphics.LEFT | Graphics.TOP);
		
		//g.drawString(chapterString, startX + bookNameWidth, 40, Graphics.LEFT | Graphics.TOP);
		
		g.drawString(GoBible.getString("UI-Found") + " " + goBible.searchResults.size(), halfWidth, 60, Graphics.HCENTER | Graphics.TOP);
		
		g.drawString(GoBible.getString("UI-Please-Wait") + "...", halfWidth, 80, Graphics.HCENTER | Graphics.TOP);
	}

}
