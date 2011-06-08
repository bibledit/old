//
//  GoBible.java
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
import java.util.*;
import javax.microedition.lcdui.*;
import javax.microedition.midlet.*;
import javax.microedition.rms.*;

public class GoBible extends MIDlet implements Runnable
{
	// Compile time constants
	public final static boolean USE_MIDP20 = true;

	public final static String UI_PROPERTIES_FILE_NAME = "/ui.properties";
	public final static String GBCoreVer = "2.4.0";	// the version of this Core application

	public final static int FONT_SIZE_SMALL = 0;
	public final static int FONT_SIZE_MEDIUM = 1;
	public final static int FONT_SIZE_LARGE = 2;
	
	public final static int[] FONT_SIZE_MAP = new int[]{Font.SIZE_SMALL, Font.SIZE_MEDIUM, Font.SIZE_LARGE};

	public final static int FONT_STYLE_PLAIN = 0;
	public final static int FONT_STYLE_BOLD = 1;

	// Theme indices
	public final static int THEME_PAPER = 0;
	public final static int THEME_COMPUTER = 1;
	public final static int THEME_FLORAL = 2;
	public final static int THEME_NATURAL = 3;
	public final static int THEME_BLUE = 4;
	public final static int THEME_SUNSHINE = 5;
	
	// Theme text colours
	public final static int[] THEME_TEXT_COLOUR = new int[]
	{
		0x000000,
		0x00FF00,
		0x660066,
		0x883300,
		0x000088,
		0x006600,
	};

	// Theme back colours
	public final static int[] THEME_BACK_COLOUR = new int[]
	{
		0xFFFFFF,
		0x000000,
		0xFFDDF0,
		0xFFFFCC,
		0xDDEEFF,
		0xFFFF00,
	};

	// Theme highlight colours
	public final static int[] THEME_HIGHLIGHT_COLOUR = new int[]
	{
		0xE0E0FF,
		0x005500,
		0xDDC0DD,
		0xCCFFCC,
		0xFFFFFF,
		0xBBFF00,
	};

	// Theme Christ words colours
	public final static int[] THEME_CHRIST_COLOUR = new int[]
	{
		0xC00000,
		0xFF8800,
		0x006666,
		0x006600,
		0xD00000,
		0xDD4400,
	};
	

	// Static variables
		
	// Preferences record store indices
	public static int PREFS_FONT_SIZE = 0;
	
	// The last book, chapter, and verse viewed
	public static int PREFS_BOOK = 1;
	public static int PREFS_CHAPTER = 2;
	public static int PREFS_VERSE = 3;

	// UI Properties file Strings
	public static Hashtable uiProperties = new Hashtable();
	
	
	// Instance variables
	
	public Display display;

	//private SplashScreen splashScreen;
	public BibleCanvas bibleCanvas;
	//private GotoForm gotoForm;
	private PrefsForm prefsForm;
	
	private boolean firstRun = true;

	public int currentBookIndex = 0;
	public int currentChapterIndex = 0;
	public int currentVerseIndex = 0;
	
	// Display preferences
	
	public int fontSize = FONT_SIZE_MEDIUM;
	public int fontStyle = FONT_STYLE_PLAIN;
	public boolean fullScreen = false;
	public int theme = THEME_PAPER;
	public boolean reverseColours = false;
	
	// Current chapter contents
	public static int[] verseIndex;
	public static char[] verseData;
	
	public BibleSource bibleSource;
	
	// Search preferences
	public String lastSearchString = "";
	public int lastFromBook = -1;
	public int lastToBook = -1;
	public boolean searchCanSpanMultipleVerses = false;	// true for old search behavior

	// Search results
	Vector searchResults = new Vector();
	public int lastSearchIndex = 0;

	// Bookmarks
	Vector bookmarks = new Vector();


	// History
	public final static int HISTORY_SIZE = 20;
	public final static int HISTORY_EXCERPT_SIZE = 30;
	
	// History is a circular array, the start index indicates where in the array
	// the list starts. When a new entry is inserted the startIndex will be decremented
	// and the new entry inserted at the new startIndex. If the startIndex goes below
	// zero then it becomes one less than the number of history elements.
	public int historyStartIndex = 0;
	
	// Verse indices contains bookIndex, chapterIndex, then verseIndex
	public byte[] historyVerseIndices = new byte[HISTORY_SIZE * 3];
	public String[] historyExcerpts = new String[HISTORY_SIZE];
	

	// Timers to switch off backlight after 2 minutes
	Timer backlightTimer;
	TimerTask backlightTask;

	// Profiling
	public long fromJarTime;
	public long readUTFTime;
	public long readBytesTime;
	public long byteToStringTime;
	public long byteToCharTime;
	public long loadChapterTime;
	public long skipTime;

	public void startApp()
	{
		// Try to turn on the backlight
		turnOnBacklight();


		if (firstRun)
		{
			firstRun = false;

			// Read in the UI properties file
			readUiProperties();
			
			display = Display.getDisplay(this);

			try
			{
				// Create the main screen
				bibleCanvas = new BibleCanvas(this);

				bibleCanvas.init();

				display.setCurrent(bibleCanvas);

				readPreferences();
				readBookmarks();
				readHistory();

				if (USE_MIDP20)
				{
					bibleCanvas.setFullScreenMode(fullScreen);
				}

				// Load the data in another thread allowing the splash screen to be seen
				new Thread(this).start();			
			}
			catch (Throwable e)
			{
				Form form = new Form("Error");
				form.append(e.toString());
				display.setCurrent(form);
			}
		}
/*		if (firstRun)
		{
			SplashScreen splashScreen = new SplashScreen(this);
			
			// Display the splash screen while the data is being loaded
			display.setCurrent(splashScreen);
			
			// Load the data after the splash screen is visible
			display.callSerially(this);
			
			firstRun = false;
		}*/
	}
	
	public void pauseApp()
	{
	}
	
	public void destroyApp(boolean unconditional)
	{
		// Save the preferences
		writePreferences();
		
		// Save the bookmarks
		writeBookmarks();
		
		// Save the history
		writeHistory();
		
		// Cancel the backlight timer
		if (backlightTimer != null)
		{
			backlightTimer.cancel();
		}
	}
		
	/**
	 * This method is part of the Runnable interface and is called from startApp()
	 * through the Display.callSerially() method so that the splash screen can be
	 * seen before we start loading in the files.
	 */
	public void run()
	{
		boolean error = false;
		
		// Read in the current chapter 
		try
		{
			bibleSource = new CombinedChapterBibleSource(this);
			loadCurrentChapter();
		}
		catch (IOException e)
		{
			error = true;
			System.out.println("Error");
			display.setCurrent(new Alert(getString("UI-Error"), e.toString(), null, AlertType.ERROR));
		}
		
		if (!error)
		{
			// Also create the goto screen for use later
			//gotoForm = new GotoForm(this);
			
			showMainScreen();
		}
	}
	
	/**
	 * If the Nokia UI API exists then turn on the backlight
	 * and schedule a task to turn it off after 2 minutes of
	 * inactivity.
	 */
	public void turnOnBacklight()
	{
		// See if Nokia UI API exists
		// This has been disabled as it can cause problems on some phones
		// with not turning the backlight off and also restricts
		// Go Bible from installing on BlackBerry phones
		/*try
		{
			Class.forName("com.nokia.mid.ui.DeviceControl");
			
			// Turn on the backlight
			// O is the backlight
			// 100 indicates on, 0 indicates off
			com.nokia.mid.ui.DeviceControl.setLights(0, 100);
			
			// If timer already exists then cancel it and create a new one
			// unfortunately existing tasks can not be rescheduled
			if (backlightTimer != null)
			{
				backlightTimer.cancel();
			}
			
			// Create a new timer
			backlightTimer = new Timer();

			// Timer task turns off backlight
			backlightTask = new TimerTask()
			{
				public void run()
				{
					com.nokia.mid.ui.DeviceControl.setLights(0, 0);
				}
			};

			// Turn backlight off after 2 minutes
			backlightTimer.schedule(backlightTask, 120000);
		}
		catch (ClassNotFoundException e)
		{
			// Fail quietly
		}*/
	}
	
	/**
	 * This method is called to redisplay the main screen, the
	 * currently displayed passage is not changed.
	 */
	public void showMainScreen()
	{
		// Set up the theme colours
		int textColour = THEME_TEXT_COLOUR[theme];
		int backColour = THEME_BACK_COLOUR[theme];
		
		if (reverseColours)
		{
			int temp = textColour;
			textColour = backColour;
			backColour = temp;
			bibleCanvas.highlightColour = backColour;
		}
		else
		{
			bibleCanvas.highlightColour = THEME_HIGHLIGHT_COLOUR[theme];
		}
		
		bibleCanvas.textColour = textColour;
		bibleCanvas.backColour = backColour;
		bibleCanvas.christWordsColour = THEME_CHRIST_COLOUR[theme];
	
		display.setCurrent(bibleCanvas);
		
		if (USE_MIDP20)
		{
			bibleCanvas.setFullScreenMode(fullScreen);
		}
		
		// Some of the values may have changed so refresh the display accordingly
		bibleCanvas.update();
	}

	public void addBookmark()
	{
		try
		{
			// Create a new BookmarkEntry
			BookmarkEntry bookmark = new BookmarkEntry(currentBookIndex, currentChapterIndex, currentVerseIndex, getExcerpt(currentBookIndex, currentChapterIndex, currentVerseIndex));
			
			// Insert bookmark at the beginning of the bookmarks vector
			bookmarks.insertElementAt(bookmark, 0);
			
			// Show the bookmarks screen
			showBookmarksScreen();
		}
		catch (IOException e)
		{
			e.printStackTrace();
		}
	}

	public void showBookmarksScreen()
	{
		display.setCurrent(new BookmarksList(this));
	}

	public void showHistoryScreen()
	{
		display.setCurrent(new HistoryList(this));
	}

	public void showSendSMSScreen()
	{
		if (USE_MIDP20)
		{
			display.setCurrent(new SendSMSForm(this, SendSMSForm.SMS));
		}
	}

	public void showSendMMSScreen()
	{
		if (USE_MIDP20 && false)	// disable for now, 2.4.0 release until code fixed
		{
			display.setCurrent(new SendSMSForm(this, SendSMSForm.MMS));
		}
	}

	public void showSearchScreen()
	{
		display.setCurrent(new SearchForm(this));
	}

	public void showSearchResultsScreen()
	{
		display.setCurrent(new SearchResultsList(this, searchResults));
	}
	
	public void showPrefsScreen()
	{
		PrefsForm prefsForm = new PrefsForm(this);
		display.setCurrent(prefsForm);
	}
	
	public void showAboutAlert()
	{
		String infoString = getAppProperty("Go-Bible-Info");
		String safeAppName = getAppProperty("Go-Bible-Safe-Name");
		String regAppName = getAppProperty("Application-Name");

		String midletVersion = getAppProperty("MIDlet-Version");
		String midletName = getAppProperty("MIDlet-Name");

		String midletInfoUrl = getAppProperty("MIDlet-Info-URL");
		String midletVendor = getAppProperty("MIDlet-Vendor");

		if (midletVendor == null)	// shouldn't ever be null since GBCreator puts out a default value if not given
			midletVendor = "";		// but just to be sure, we'll make it empty

		if (midletInfoUrl == null)	// shouldn't ever be null since GBCreator puts out a default value if not given
			midletInfoUrl = "";		// but just to be sure, we'll make it empty

		if (infoString == null)
			infoString = "";

		// If there is a 'safe name' use it, if not use the 'app name', otherwise
		// use 'Go Bible'.
		if (safeAppName == null)
		{
			if (regAppName == null)
				safeAppName = "Go Bible";
			else
				safeAppName = regAppName;
		}

		// application name used in go bible creator
		// the manifest file still has GoBible hardcoded. Should change?
		//        		attributes.putValue("MIDlet-Name", collection.name + " " + applicationName);

		display.setCurrent(new Alert(getString("UI-About") + " " + safeAppName, "Vendor: " + midletVendor + "\n" + midletInfoUrl + "\n\"For the glory of our Lord Jesus Christ and the furtherance of His Kingdom\"\n" + infoString + " Viewer Version: " + GBCoreVer, null, AlertType.INFO));
	}
	
	/**
	 * This method is called to change the currently displayed passage.
	 */
	public void gotoPassage(int bookIndex, int chapterIndex, int verseIndex)
	{
		boolean error = false;
		String bookName = null;

		if (bookIndex != currentBookIndex || chapterIndex != currentChapterIndex || verseIndex != currentVerseIndex)
		{
			// Change the passage
			currentBookIndex = bookIndex;
			currentChapterIndex = chapterIndex;
			currentVerseIndex = verseIndex;
						
			// Make sure the chapter exists
			int numberOfChapters = bibleSource.getNumberOfChapters(bookIndex);
			
			if (chapterIndex >= numberOfChapters)
			{
				error = true;
				currentChapterIndex = numberOfChapters - 1;
			}
			
			int numberOfVerses = bibleSource.getNumberOfVerses(currentBookIndex, currentChapterIndex);
			
			// Make sure the verse exists
			if (verseIndex >= numberOfVerses)
			{
				error = true;
				currentVerseIndex = numberOfVerses - 1;
			}

			bookName = bibleSource.getBookName(bookIndex);
		}

		// Show the main screen and indicate that we are loading
		bibleCanvas.enterLoadingMode();
		
	/*	if (error)
		{
			String errorString = bookName + " " + (chapterIndex + 1) + ":" + (verseIndex + 1) + " doesn't exist.\nDisplaying closest.";
			Alert alert = new Alert("Verse not found", errorString, null, AlertType.INFO);
			display.setCurrent(alert);
		}*/

		// Load the chapter data
		loadCurrentChapter();
		
		try
		{
			//System.out.println("Storing in history");
		
			// Store in history
			// Decrement the start index
			historyStartIndex--;

			// If the start index is less than zero then wrap it
			if (historyStartIndex < 0)
			{
				historyStartIndex = HISTORY_SIZE - 1;
			}
			
			//System.out.println("Storing in history start index = " + historyStartIndex);
			
			// Store the entry
			historyVerseIndices[historyStartIndex * 3] = (byte) currentBookIndex;
			historyVerseIndices[historyStartIndex * 3 + 1] = (byte) currentChapterIndex;
			historyVerseIndices[historyStartIndex * 3 + 2] = (byte) currentVerseIndex;

			//System.out.println("Indices stored");

			historyExcerpts[historyStartIndex] = getExcerpt(currentBookIndex, currentChapterIndex, currentVerseIndex);

			//System.out.println("Excerpt = " + historyExcerpts[historyStartIndex]);
		}
		catch (IOException e)
		{
		}
		
		bibleCanvas.update();
	}
	
	/**
	 * Returns a string excerpt of the verse reference.
	 */
	public String getExcerpt(int bookIndex, int chapterIndex, int verseIndex) throws IOException
	{
		// Grab first 20 characters from verse for excerpt
		int[] verseIndices = bibleSource.getChapterIndex(bookIndex, chapterIndex);
		
		int verseOffset = verseIndices[verseIndex * 2];
		int verseEndOffset = 0;
		
		int numberOfVerses = bibleSource.getNumberOfVerses(bookIndex, chapterIndex);

		// Try to get the next offset
		if (verseIndex < numberOfVerses - 1)
		{
			verseEndOffset = verseIndices[(verseIndex + 1) * 2];
		}
		else
		{
			// This is the last verse in the chapter so just set the verseEndOffset to the
			// verse data size
			verseEndOffset = bibleSource.getVerseDataSize();
		}
		
		//System.out.println("verseOffset = " + verseOffset);
		//System.out.println("verseEndOffset = " + verseEndOffset);

		char[] verseData = bibleSource.getChapter(bookIndex, chapterIndex);
		
		// Extract the first 20 characters or to the end of the verse
		int excerptSize = HISTORY_EXCERPT_SIZE;
		
		if (verseEndOffset - verseOffset < HISTORY_EXCERPT_SIZE)
		{
			excerptSize = verseEndOffset - verseOffset;
		}
		else
		{
			// If the excerpt ends mid-word then continue excerpt
			// until a space, punctuation, or end of verse
			while (excerptSize < verseEndOffset - verseOffset)
			{
				char nextChar = verseData[verseOffset + excerptSize];
				
				if (nextChar == ' ' || nextChar == '.' || nextChar == ',' || nextChar == ';')
				{
					break;
				}
				else
				{
					excerptSize++;
				}
			}
		}
		
		// Grab the characters ignoring any control characters
		// eg. indicating Christ's word in red
		char[] chars = new char[excerptSize];
		int j = 0;
		
		for (int i = verseOffset; i < (verseOffset + excerptSize); i++)
		{
			// Skip any control characters
			if (verseData[i] >= 10)
			{
				chars[j++] = verseData[i];
			}
		}
		
		String excerpt = new String(chars, 0, j);
		
		// If the excerpt length is less than the verse length then append an ellipsis
		if (excerptSize < (verseEndOffset - verseOffset))
		{
			//excerpt += "�";
			excerpt += '\u2026';	// HORIZONTAL ELLIPSIS
		}
		
		return excerpt;
	}
	
	/**
	 * Called by the BibleCanvas when the up arrow is pressed.
	 */
	public void scrollPrevious()
	{
		currentVerseIndex--;
		
		if (currentVerseIndex < 0)
		{
			previousChapter(true);
		}
	}

	/**
	 * Called by the BibleCanvas when the down arrow is pressed.
	 */
	public void scrollNext()
	{
		currentVerseIndex++;
		
		if (currentVerseIndex >= bibleSource.getNumberOfVerses(currentBookIndex, currentChapterIndex))
		{
			nextChapter();
		}
	}
	
	public void previousChapter(boolean lastVerse)
	{
		// Go to the previous chapter
		currentChapterIndex--;

		if (currentChapterIndex < 0)
		{
			previousBook(false);
			
			// Go to the last chapter in the previous book
			currentChapterIndex = bibleSource.getNumberOfChapters(currentBookIndex) - 1;
		}

		if (lastVerse)
		{
			currentVerseIndex = bibleSource.getNumberOfVerses(currentBookIndex, currentChapterIndex) - 1;
		}
		else
		{
			currentVerseIndex = 0;
		}
		
		loadCurrentChapter();
	}
	
	public void nextChapter()
	{
		// Go to the next chapter
		currentChapterIndex++;
		
		if (currentChapterIndex >= bibleSource.getNumberOfChapters(currentBookIndex))
		{
			nextBook(false);
		}

		currentVerseIndex = 0;
		
		loadCurrentChapter();
	}

	public boolean isLastBook() { return currentBookIndex == bibleSource.getNumberOfBooks();}
	public boolean isFirstBook() { return currentBookIndex == 0;}

	public void previousBook(boolean load)
	{
		// Go to the previous book
		currentBookIndex--;
		
		// Wrap
		if (currentBookIndex < 0)
		{
			currentBookIndex = bibleSource.getNumberOfBooks() - 1;
		}
		
		currentChapterIndex = currentVerseIndex = 0;
		
		if (load)
		{
			loadCurrentChapter();
		}
	}
	
	public void nextBook(boolean load)
	{
		// Go to the next book
		currentBookIndex++;
		
		// Wrap
		if (currentBookIndex >= bibleSource.getNumberOfBooks())
		{
			currentBookIndex = 0;
		}
		
		currentChapterIndex = currentVerseIndex = 0;
		
		if (load)
		{
			loadCurrentChapter();
		}
	}

	/**
	 * All of the visible strings are stored in the ui.properties file
	 * to allow for internationalisation.
	 */
	private void readUiProperties()
	{
		try
		{
			// Read in the UI properties file
			InputStream input = getClass().getResourceAsStream(UI_PROPERTIES_FILE_NAME);

			// Read it all in
			byte[] fileData = new byte[32768];
			
			int offset = 0;
			int bytesRead = 0;
			
			do
			{
				bytesRead = input.read(fileData, offset, fileData.length - offset);
				offset += bytesRead;
			}
			while (bytesRead >= 0);
			
			// Close input stream
			input.close();
			
			// Convert byte data to String
			String uiPropertiesString = new String(fileData, 0, offset, "UTF-8");
			
			// Read each line
			int startOfLineIndex = 0;
			int endOfLineIndex = uiPropertiesString.indexOf('\n');
			
			while (endOfLineIndex != -1)
			{
				readUiPropertiesLine(uiPropertiesString, startOfLineIndex, endOfLineIndex);
				
				// Read in next line
				startOfLineIndex = endOfLineIndex + 1;
				endOfLineIndex = uiPropertiesString.indexOf('\n', startOfLineIndex);
			}
			
			// Read in last line
			readUiPropertiesLine(uiPropertiesString, startOfLineIndex, uiPropertiesString.length());
		}
		catch (IOException e)
		{
			e.printStackTrace();
		}
	}
	
	/**
		Parses a UI properties line and inserts it into the uiProperties hashtable.
	 */
	private void readUiPropertiesLine(String uiPropertiesString, int startOfLineIndex, int endOfLineIndex)
	{
		String lineString = uiPropertiesString.substring(startOfLineIndex, endOfLineIndex);

		// Get property name
		int colonIndex = lineString.indexOf(':');
		
		if (colonIndex >= 0)
		{
			String propertyName = lineString.substring(0, colonIndex).trim();
			String uiString = lineString.substring(colonIndex + 1, lineString.length()).trim();
			
			// Insert into hashtable
			uiProperties.put(propertyName, uiString);
		}		
	}
	
	/**
	 * Returns a display string for the specified property name from the
	 * UI properties file.
	 * If a string for the property name can not be found then the 
	 * property name is simply returned.
	 */
	public static String getString(String propertyName)
	{
		String result = (String) uiProperties.get(propertyName);
		
		if (result == null)
		{
			result = propertyName;
		}
		
		return result;
	}

	/**
	 * The application's state is stored in the record store under the name "Preferences".
	 */
	private void readPreferences()
	{
		try
		{
			// Open the "Preferences" record store and create it if necessary
			RecordStore store = RecordStore.openRecordStore("Preferences", true);
			
			// Make sure the record store isn't empty
			if (store.getNumRecords() > 0)
			{
				byte[] data = store.getRecord(1);
	
				DataInputStream input = new DataInputStream(new ByteArrayInputStream(data));
	
				// Get the font size
				fontSize = input.readByte();

				// Get the last displayed book, chapter, and verse
				currentBookIndex = input.read();
				currentChapterIndex = input.read();
				currentVerseIndex = input.read();
				
				//System.out.println("Book index = " + currentBookIndex);
				//System.out.println("Chapter index = " + currentChapterIndex);
				//System.out.println("Verse index = " + currentVerseIndex);
				
				lastSearchString = input.readUTF();
				
				// Read in full screen mode
				fullScreen = input.readBoolean();
				
				// Read in reverse characters mode
				bibleCanvas.reverseCharacters = input.readBoolean();

				// Read in theme
				theme = input.readByte();
				
				// Read in reverse colours
				reverseColours = input.readBoolean();
				
				// Get the font style
				fontStyle = input.readByte();
				
				// Read in the last from and to books used in the search
				lastFromBook = input.readByte();
				lastToBook = input.readByte();
				
				input.close();
			}
		}
		catch (IOException ee)
		{
		}
		catch (RecordStoreException e)
		{
		}
	}
	
	/**
	 * Save the application's state so that it can return to the same state when reopened.
	 */
	public void writePreferences()
	{
		try
		{
			// Open the "Preferences" record store and create it if necessary
			RecordStore store = RecordStore.openRecordStore("Preferences", true);
			
			ByteArrayOutputStream byteArrayOutputStream = new ByteArrayOutputStream();
			DataOutputStream output = new DataOutputStream(byteArrayOutputStream);
	
			// Set the font size
			output.write(fontSize);
			
			// Set the last displayed book, chapter, and verse
			output.write(currentBookIndex);
			output.write(currentChapterIndex);
			output.write(currentVerseIndex);
			
			output.writeUTF(lastSearchString);
			
			// Write out full screen mode
			output.writeBoolean(fullScreen);

			// Write out reverse characters mode
			output.writeBoolean(bibleCanvas.reverseCharacters);

			// Write out theme
			output.write(theme);
			
			// Write out reverse colours
			output.writeBoolean(reverseColours);

			// Write out the font style
			output.write(fontStyle);
			
			// Write out the last from and to books used in the search
			output.write(lastFromBook);
			output.write(lastToBook);
			
			output.close();
			
			byte[] data = byteArrayOutputStream.toByteArray();
			
			// Make sure the record store isn't empty
			if (store.getNumRecords() > 0)
			{
				store.setRecord(1, data, 0, data.length);
			}
			else
			{
				// Create record if it doesn't exist
				store.addRecord(data, 0, data.length);		
			}
		}
		catch (IOException e)
		{
		}
		catch (RecordStoreException e)
		{
		}
	}

	/**
	 * Bookmarks are stored in the record store under the name "Bookmarks".
	 */
	private void readBookmarks()
	{
		try
		{
			// Open the "Bookmarks" record store and create it if necessary
			RecordStore store = RecordStore.openRecordStore("Bookmarks", true);
			
			// Make sure the record store isn't empty
			if (store.getNumRecords() > 0)
			{
				byte[] data = store.getRecord(1);
	
				DataInputStream input = new DataInputStream(new ByteArrayInputStream(data));
	
				//System.out.println("Reading Bookmarks");
	
				// Read in the number of bookmarks
				int numberOfBookmarks = input.readUnsignedShort();

				//System.out.println("Number of bookmarks = " + numberOfBookmarks);

				// Read in the bookmarks
				for (int i = 0; i < numberOfBookmarks; i++)
				{
					// Read in a bookmark, constructor reads from input stream
					BookmarkEntry bookmark = new BookmarkEntry(input);
					bookmarks.addElement(bookmark);

					//System.out.println("Excerpt = " + bookmark.excerpt);
				}

				input.close();
			}
		}
		catch (IOException ee)
		{
		}
		catch (RecordStoreException e)
		{
		}
	}

	/**
	 * Save the bookmarks.
	 */
	public void writeBookmarks()
	{
		try
		{
			// Open the "Bookmarks" record store and create it if necessary
			RecordStore store = RecordStore.openRecordStore("Bookmarks", true);
			
			ByteArrayOutputStream byteArrayOutputStream = new ByteArrayOutputStream();
			DataOutputStream output = new DataOutputStream(byteArrayOutputStream);

			// Write out the number of bookmarks
			output.writeShort(bookmarks.size());

			// Write out the history
			for (Enumeration e = bookmarks.elements(); e.hasMoreElements(); )
			{
				BookmarkEntry bookmark = (BookmarkEntry) e.nextElement();
				
				bookmark.write(output);
			}
	
			output.close();
			
			byte[] data = byteArrayOutputStream.toByteArray();
			
			// Make sure the record store isn't empty
			if (store.getNumRecords() > 0)
			{
				store.setRecord(1, data, 0, data.length);
			}
			else
			{
				// Create record if it doesn't exist
				store.addRecord(data, 0, data.length);		
			}
		}
		catch (IOException e)
		{
		}
		catch (RecordStoreException e)
		{
		}
	}



	/**
	 * The application's history is stored in the record store under the name "History".
	 */
	private void readHistory()
	{
		try
		{
			// Open the "History" record store and create it if necessary
			RecordStore store = RecordStore.openRecordStore("History", true);
			
			// Make sure the record store isn't empty
			if (store.getNumRecords() > 0)
			{
				byte[] data = store.getRecord(1);
	
				DataInputStream input = new DataInputStream(new ByteArrayInputStream(data));
	
				//System.out.println("Reading history");
	
				// Read in the start index
				historyStartIndex = input.readByte();

				//System.out.println("Start Index = " + historyStartIndex);

				// Read in the history
				for (int i = 0, j = 0; i < HISTORY_SIZE; i++)
				{
					// Read in the book index
					historyVerseIndices[j++] = input.readByte();
					
					// Read in the chapter index
					historyVerseIndices[j++] = input.readByte();

					// Read in the verse index
					historyVerseIndices[j++] = input.readByte();
					
					// Read in the excerpt
					historyExcerpts[i] = input.readUTF();

					//System.out.println("Excerpt = " + historyExcerpts[i]);
				}

				input.close();
			}
		}
		catch (IOException ee)
		{
		}
		catch (RecordStoreException e)
		{
		}
	}
	
	/**
	 * Save the application's history.
	 */
	public void writeHistory()
	{
		try
		{
			// Open the "History" record store and create it if necessary
			RecordStore store = RecordStore.openRecordStore("History", true);
			
			ByteArrayOutputStream byteArrayOutputStream = new ByteArrayOutputStream();
			DataOutputStream output = new DataOutputStream(byteArrayOutputStream);

			// Write out the start index
			output.writeByte(historyStartIndex);

			// Write out the history
			for (int i = 0, j = 0; i < HISTORY_SIZE; i++)
			{
				// Write out the book index
				output.writeByte(historyVerseIndices[j++]);

				// Write out the chapter index
				output.writeByte(historyVerseIndices[j++]);

				// Write out the verse index
				output.writeByte(historyVerseIndices[j++]);
				
				// Write out the excerpt
				output.writeUTF(historyExcerpts[i] == null ? "" : historyExcerpts[i]);
			}
	
			output.close();
			
			byte[] data = byteArrayOutputStream.toByteArray();
			
			// Make sure the record store isn't empty
			if (store.getNumRecords() > 0)
			{
				store.setRecord(1, data, 0, data.length);
			}
			else
			{
				// Create record if it doesn't exist
				store.addRecord(data, 0, data.length);		
			}
		}
		catch (IOException e)
		{
		}
		catch (RecordStoreException e)
		{
		}
	}

	public void loadCurrentChapter()
	{
		try
		{
			verseData = bibleSource.getChapter(currentBookIndex, currentChapterIndex);
			verseIndex = bibleSource.getChapterIndex(currentBookIndex, currentChapterIndex);
		}
		catch(IOException e)
		{
			System.out.println("IOException in loadCurrentChapter()");
		}
	}
}
