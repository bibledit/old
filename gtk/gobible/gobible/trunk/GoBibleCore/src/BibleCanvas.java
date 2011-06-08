//
//  BibleCanvas.java
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

import javax.microedition.lcdui.*;

public class BibleCanvas extends Canvas implements CommandListener
{
	private final static boolean DEBUG = false;

	private final static int TEXT_BORDER_LEFT = 3;
	private final static int TEXT_BORDER_RIGHT = 3;
	private final static int BAR_PADDING = 2;
	private final static int BOX_CORNER = 10;

	// Modes
	private final static int MODE_SPLASH = 0;
	private final static int MODE_LOADING = 1;
	private final static int MODE_VIEWING = 2;
	private final static int MODE_GOTO = 3;

	// Selection
	private final static int SELECTION_BOOK = 0;
	private final static int SELECTION_CHAPTER = 1;
	private final static int SELECTION_VERSE = 2;
	
	private final static int SELECTION_BORDER = 5;
	
	// Viewing
	private final static int ALIGN_LEFT = 0;
	private final static int ALIGN_RIGHT = 1;
	
	private final static char STYLE_RED = 1;
	private final static int COLOUR_RED = 0xB00000;
	private final static int HIGHLIGHT_BACKGROUND_COLOUR = 0xE0E0FF;
	private final static int HIGHLIGHT_OUTLINE_COLOUR = 0xC0C0FF;

	private GoBible goBible;

	private Command gotoCommand = new Command(GoBible.getString("UI-Goto"), Command.OK, 0);
	private Command searchCommand = new Command(GoBible.getString("UI-Search"), Command.SCREEN, 0);
	private Command searchResultsCommand = new Command(GoBible.getString("UI-Search-Results"), Command.SCREEN, 0);
	private Command addBookmarkCommand = new Command(GoBible.getString("UI-Add-Bookmark"), Command.SCREEN, 0);
	private Command bookmarksCommand = new Command(GoBible.getString("UI-Bookmarks"), Command.SCREEN, 0);
	private Command historyCommand = new Command(GoBible.getString("UI-History"), Command.SCREEN, 0);
	private Command sendSMSCommand, sendMMSCommand;
	private Command prefsCommand = new Command(GoBible.getString("UI-Preferences"), Command.SCREEN, 0);
	private Command aboutCommand = new Command(GoBible.getString("UI-About"), Command.SCREEN, 0);
	private Command exitCommand = new Command(GoBible.getString("UI-Exit"), Command.EXIT, 0);
	private Command cancelCommand = new Command(GoBible.getString("UI-Cancel"), Command.CANCEL, 0);

	/**
	 * The verses of string. eg. Mark 1:1 of 45.
	 */
	private String ofString = GoBible.getString("UI-Of");
	
	/**
	 * By default display the splash screen
	 */
	private int mode = MODE_SPLASH;
	
	// Colours
	int textColour = 0x000000;
	int backColour = 0xFFFFFF;
	int christWordsColour = 0xD00000;
	int highlightColour = 0xE0E0FF;
	
	// Text alignment
	int align, anchor;
	static boolean reverseCharacters;
	
	// Save the width and height for fast access
	int width;
	int height;

	// position information used in the Touch event processing
	private int firstX, firstY;
	private int diffX, diffY;
	//private int keyCode;
	
	// Viewing mode fields
	int lineStart, lastSpace, y, drawX;

	// These fields indicate that the current verse is larger than the displayable
	// area and that intra-verse scrolling is required
	private boolean moreVerseDown;
	
	// When scrolling through a verse it is handy to keep a record of 
	// the lines for each scroll point. This saves having to
	// rewrap all lines up to the current scroll point.
	private int[] scrollPoints = new int[100];
	private boolean[] scrollStyleRed = new boolean[100];
	private int currentScrollPointIndex = -1;
	
	// These scroll points are used for all verses when scrolling downwards 
	// (not just within a verse as the preceding fields are used for)
	private int currentScrollPoint = -1;
	private boolean currentScrollStyle;
	
	private int nextScrollPointVerse = -1;
	private int nextScrollPoint;
	private boolean nextScrollStyle;
	
	// Goto mode fields
	
	private int[] selection = new int[3];
	private int selectionIndex = SELECTION_BOOK;
	private boolean editingSelection;
	
	public BibleCanvas(GoBible goBible)
	{
		this.goBible = goBible;
		// boolean supportsMotion = hasPointerMotionEvents();
	}

	// the stylus, touch screen, or trackball was pressed
	protected void pointerPressed(int x, int y)
	{
		firstX =  x;
		firstY =  y;
		diffX = diffY = 0;
	}

	/// These constants are used as an initial guess/start for values that should
	/// eventually end up in the UI Preferences and would likely be customized
	/// by users on different devices due to performance of the specific hardware.
	private static final int SCROLL_SLOW = 9;	// 5;
	private static final int SCROLL_MED = 13;	// 9;
	private static final int SCROLL_FAST = 18;	// 12;
	private int throwAwayScrollCount = 0;

	/**
	 * This method is used to handle the 'Touch' event 'pointerDragged'.
	 *
	 * Simulate the user scrolling by verse, chapter or book depending on how
	 * fast the users 'swipe' gesture is.
	 *
	 * @param x, x location
	 * @param y, y location
	 */
	protected void pointerDragged(int x, int y)
	{
		if (y==firstY)
			return;
		// allow the user to stop motion after fast scroll before taking action again
		if (throwAwayScrollCount > 0)
		{
			firstX = x;
			firstY = y;
			throwAwayScrollCount--;
			return;
		}
		boolean didWork = false;
		diffX = Math.abs(firstX - x);
		diffY = Math.abs(firstY - y);

		// if the change in 'x' is greater than the change in 'y'
		if (diffY > diffX)
		{
			// if it's quantified as a 'slow scroll' - send scroll keypresses
			if (diffY >= SCROLL_SLOW && diffY <= SCROLL_MED-1)
			{
				if (y > firstY)
					keyPressed(KEY_NUM3);	// scroll up
				else
					keyPressed(KEY_NUM6);	// scroll down
				didWork = true;
			}
			// if it's quantified as a 'med scroll' - send chapter keypresses
			else if (diffY >= SCROLL_MED && diffY <= SCROLL_FAST-1)
			{
				if (y > firstY)
					keyPressed(KEY_NUM2);	// chapter up
				else
					keyPressed(KEY_NUM5);	// chapter down
				didWork = true;
				// ignore the next couple to allow the user to slow down
				throwAwayScrollCount = 8;	//2// skipp the next two
			}
			// if it's quantified as a 'fast scroll' - send book keypresses
			else if (diffY >= SCROLL_FAST)
			{
				// limit the fast scroll to stop at the last and first books
				if (y > firstY)
				{
					if (!goBible.isFirstBook())
						keyPressed(KEY_NUM1);	// book up
				}
				else
				{
					if (!goBible.isLastBook())
						keyPressed(KEY_NUM4);
				}
				didWork = true;
				// ignore the next few to allow the user to slow down
				throwAwayScrollCount = 25; //5;	// skipp the next five
			}
			// if we did something, update the display and reset the position
			if (didWork)
			{
				update();
				firstX = x;
				firstY = y;
			}
		}
	}

	/**
	 * pointerReleased 'touch' event - not currently needed / used.
	 * @param x
	 * @param y
	 */
//	protected void pointerReleased(int x, int y)
//	{
//		diffX = Math.abs(firstX - x);
//		diffY = Math.abs(firstY - y);
//		if (diffY > diffX && diffY > 20)
//		{
//			if (y > firstY)
//				goBible.previousChapter(false);
//			else
//				goBible.nextChapter();
//			update();
//		}
//	}

	public void init()
	{
		// Get the alignment property
		String alignmentString = goBible.getAppProperty("Go-Bible-Align");
		if (alignmentString == null || alignmentString.equals("Left"))
		{
			align = ALIGN_LEFT;
			anchor = Graphics.TOP | Graphics.LEFT;
		}
		else if (alignmentString.equals("Right"))
		{
			align = ALIGN_RIGHT;
			anchor = Graphics.TOP | Graphics.RIGHT;
		}

		if (GoBible.USE_MIDP20)
		{
			// Only create the send SMS command if the WMA API is supported
			try
			{
				Class.forName("javax.wireless.messaging.MessageConnection");
				sendSMSCommand = new Command(GoBible.getString("UI-Send-SMS"), Command.SCREEN, 0);

				if (false)
				{
					// Only create the MMS command if the WMA 2.0 API is supported
					Class.forName("javax.wireless.messaging.MessagePart");
					sendMMSCommand = new Command(GoBible.getString("UI-Send-MMS"), Command.SCREEN, 0);
				}
			}
			catch (ClassNotFoundException e)
			{
				// Okay we just won't create the command
			}
			catch (Exception e)
			{
				/*
				As of release 1.4, this exception has been retrofitted to conform
				 to the general purpose exception-chaining mechanism. The "optional
				 exception that was raised while loading the class" that may be provided
				 at construction time and accessed via the getException() method is now
				 known as the cause, and may be accessed via the Throwable.getCause()
				 method, as well as the aforementioned "legacy method."
				 * */
				// try {Class.forName("javax.wireless.messaging.MessagePart");} catch(ClassNotFoundException te){}
			}
		}

		// Add commands for the main screen
		// If not in full screen mode then add the Go to command here
		if (!goBible.fullScreen)
		{
			addCommand(gotoCommand);
		}
		
		addCommands();		
	}

	public void addCommands()
	{
		// If in fullScreen mode add the Go to command here
		if (goBible.fullScreen)
		{
			addCommand(gotoCommand);
		}
		
		addCommand(searchCommand);
		addCommand(searchResultsCommand);
		addCommand(addBookmarkCommand);
		addCommand(bookmarksCommand);
		addCommand(historyCommand);
		
		if (GoBible.USE_MIDP20)
		{
			// Only add the send SMS command if the WMA API is supported
			if (sendSMSCommand != null)
			{
				addCommand(sendSMSCommand);
			}

			// Only add the send MMS command if the WMA 2.0 API is supported
			if (sendMMSCommand != null && false)	// disable for the 2.4.0 release
			{
				addCommand(sendMMSCommand);
			}
		}
		
		addCommand(prefsCommand);
		addCommand(aboutCommand);
		addCommand(exitCommand);
		
		// Set up the command listener
		setCommandListener(this);	
	}

	/**
	 * Called when Canvas is about to be displayed. We simply reset the
	 * backlight timer task.
	 */
	public void showNotify()
	{
		goBible.turnOnBacklight();
	}

	/**
	 * Called when a menu item has been selected.
	 */
	public void commandAction(Command command, Displayable display)
	{
		switch(command.getCommandType())
		{
			case Command.EXIT:
			{
				goBible.destroyApp(false);
				goBible.notifyDestroyed();
				break;
			}
			
			case Command.OK:
			{
				if (command == gotoCommand)
				{
					enterGotoMode();
				}
			}
			
			case Command.SCREEN:
			{
				if (command == prefsCommand)
				{
					goBible.showPrefsScreen();
				}
				else if (command == aboutCommand)
				{
					goBible.showAboutAlert();
				}
				else if (command == historyCommand)
				{
					goBible.showHistoryScreen();
				}
				else if (GoBible.USE_MIDP20 && command == sendSMSCommand)
				{
					goBible.showSendSMSScreen();
				}
				else if (GoBible.USE_MIDP20 && command == sendMMSCommand)
				{
					goBible.showSendMMSScreen();
				}
				else if (command == searchCommand)
				{
					goBible.showSearchScreen();
				}
				else if (command == searchResultsCommand)
				{
					goBible.showSearchResultsScreen();
				}
				else if (command == addBookmarkCommand)
				{
					goBible.addBookmark();
				}
				else if (command == bookmarksCommand)
				{
					goBible.showBookmarksScreen();
				}
				
				break;
			}
			
			case Command.BACK:
			case Command.CANCEL:
			{
				// If we were in goto mode then we need to remove the cancel button and add
				// the removed commands
				if (mode == MODE_GOTO)
				{
					removeCommand(cancelCommand);
					
					addCommands();		
				}
				else if (mode == MODE_VIEWING)
				{
					// Exit the application
					goBible.destroyApp(false);
					goBible.notifyDestroyed();
				}

				mode = MODE_VIEWING;
			
				repaint(); serviceRepaints();
			}
		}
	}

	public void keyPressed(int keyCode)
	{
		//this.keyCode = keyCode;
		boolean keyHandled = false;

		// Check the keyCodes first as the game actions may share some of the 
		// keys we want to treat separately. Eg. some platforms treat '2' as
		// game UP but UP and 2 have different meanings in our application.

		if (mode == MODE_VIEWING)
		{
			switch (keyCode)
			{
				case KEY_NUM2:
				{
					goBible.previousChapter(false);
					keyHandled = true;
					break;
				}

				case KEY_NUM5:
				{
					goBible.nextChapter();
					keyHandled = true;
					break;
				}

				case KEY_NUM1:
				{
					goBible.previousBook(true);
					keyHandled = true;
					break;
				}

				case KEY_NUM4:
				{
					goBible.nextBook(true);
					keyHandled = true;
					break;
				}
			}

			if (keyHandled)
			{
				update();
			}
			else
			{
				int gameAction = getGameAction(keyCode);
				
				if (gameAction == UP || keyCode == KEY_NUM3)
				{
					if (currentScrollPointIndex > 0)
					{
						// There is more of the current verse yet to be displayed
						// so scroll by half the height of the canvas
						currentScrollPointIndex--;
						repaint(); serviceRepaints();
					}
					else if (currentScrollPoint >= 0)
					{
						// We are part way through a verse so just go back and show the whole thing
						update();
					}
					else
					{
						goBible.scrollPrevious();
						update();
					}			
				}
				else if (gameAction == DOWN || keyCode == KEY_NUM6)
				{
					if (moreVerseDown)
					{
						// There is more of the current verse yet to be displayed
						// so scroll to the next scroll point
						currentScrollPointIndex++;
						repaint(); serviceRepaints();
					}
					else
					{
						// If the next verse is known then jump to it
						if (nextScrollPointVerse >= 0)
						{
							goBible.currentVerseIndex = nextScrollPointVerse;
							currentScrollPoint = nextScrollPoint;
							currentScrollStyle = nextScrollStyle;
							currentScrollPointIndex = -1;
							repaint(); serviceRepaints();
						}
						else
						{
							// The next verse doesn't exist so jump to the next chapter
							goBible.nextChapter();
							update();
						}
					}	
				}
				else if (gameAction == FIRE)
				{
					enterGotoMode();
				}
			}
		}
		else if (mode == MODE_GOTO)
		{
			// The starting chapter or verse is usually 1
			int startIndex = 1;
			
			// However, some books may be split resulting in chapters starting after 1
			if (selectionIndex == SELECTION_CHAPTER)
			{
				startIndex = goBible.bibleSource.getStartChapter(selection[SELECTION_BOOK]);
			}
			
			// If the verse or chapter is selected and one of the numeric keys was pressed 
			// then modify the contents with the key selected
			if ((selectionIndex >= SELECTION_CHAPTER) && (keyCode >= KEY_NUM0) && (keyCode <= KEY_NUM9))
			{
				int value = keyCode - KEY_NUM0;
				int index = selection[selectionIndex];
				
				// If the selection isn't being edited then replace the current contents 
				// with the keyCode
				if (!editingSelection)
				{
					// The selection can only be modified if the first number isn't zero
					if (keyCode != KEY_NUM0)
					{
						editingSelection = true;
						selection[selectionIndex] = value - startIndex;
					}
				}
				// Only add characters to the existing selection if the existing selection
				// is less than 3 characters
				else if ((index + startIndex) < 100)
				{
					// Append the key code to the existing selection
					selection[selectionIndex] = ((index + startIndex) * 10) + value - startIndex;
				}
			}
			else
			{
				// If a number wasn't pressed then exit editing mode the current value will 
				// be validated below
				editingSelection = false;
				
				int gameAction = getGameAction(keyCode);
				
				if (keyCode == KEY_STAR)
				{
					gameAction = LEFT;
				}
				else if (keyCode == KEY_POUND)
				{
					gameAction = RIGHT;
				}
				
				// If game action is zero then no game action was pressed so
				// check if one of the soft keys was pressed and map to a game action
				// this may only work on Nokia phones
				// Left Soft Key
				if (gameAction == 0)
				{
					if (keyCode == -6)
					{
						gameAction = GAME_A;
					}
					else if (keyCode == -7)
					{
						gameAction = GAME_B;
					}
				}
				
				switch (gameAction)
				{
					case LEFT:
					{
						selectionIndex--;
						if (selectionIndex < 0)
						{
							selectionIndex = 2;
						}
						break;
					}
					
					case RIGHT:
					{
						selectionIndex++;
						selectionIndex %= 3;
						break;
					}
	
					case UP:
					{
						selection[selectionIndex]--;
						break;
					}
					
					case DOWN:
					{
						selection[selectionIndex]++;
						break;
					}
					
					case FIRE:
					{
						enterGotoMode();
						break;
					}
					
					// Cancel Go to mode
					case GAME_B:
					{
						// Switch to viewing mode
						// Add the removed commands
						addCommands();
						
						mode = MODE_VIEWING;
						repaint(); serviceRepaints();
						break;
					}
				}
				
				validateSelection();
			} // If keyCode or gameAction
			
			repaint(); serviceRepaints();
		}

		// Turn on the backlight and reschedule the turn off backlight task,
		// do this last to ensure responsiveness (probably doesn't matter that much)
		goBible.turnOnBacklight();		
	}
	
	/**
	 * Ensures that the current selection in the Go to screen is correct.
	 */
	private boolean validateSelection()
	{
		boolean correct = true;
		
		// Validate the book index
		int numberOfBooks = goBible.bibleSource.getNumberOfBooks();
		
		if (selection[SELECTION_BOOK] >= numberOfBooks)
		{
			selection[SELECTION_BOOK] = 0;
			correct = false;
		}
		else if (selection[SELECTION_BOOK] < 0)
		{
			selection[SELECTION_BOOK] = numberOfBooks - 1;
			correct = false;
		}
		
		// Validate the chapter
		int numberOfChapters = goBible.bibleSource.getNumberOfChapters(selection[SELECTION_BOOK]);

		if (selection[SELECTION_CHAPTER] >= numberOfChapters)
		{
			selection[SELECTION_CHAPTER] = 0;
			correct = false;
		}
		else if (selection[SELECTION_CHAPTER] < 0)
		{
			selection[SELECTION_CHAPTER] = numberOfChapters - 1;
			correct = false;
		}
		
		// Validate the verse
		int numberOfVerses = goBible.bibleSource.getNumberOfVerses(selection[SELECTION_BOOK], selection[SELECTION_CHAPTER]);

		if (selection[SELECTION_VERSE] >= numberOfVerses)
		{
			selection[SELECTION_VERSE] = 0;
			correct = false;
		}
		else if (selection[SELECTION_VERSE] < 0)
		{
			selection[SELECTION_VERSE] = numberOfVerses - 1;
			correct = false;
		}
		
		return correct;
	}
	
	public void enterLoadingMode()
	{
		mode = MODE_LOADING;
		
		// Repaint the canvas to indicate to the user that we are loading data
		repaint(); serviceRepaints();
	}

	/**
	 * enterViewingMode should be called if the current verse has changed.
	 * Will also repaint the canvas.
	 */
	public void update()
	{
		// If we were in goto mode then we need to remove the cancel button and add
		// the removed commands
		if (mode == MODE_GOTO);
		{
			removeCommand(cancelCommand);
			
			addCommands();		
		}
	
		// Turn loading off
		mode = MODE_VIEWING;
	
		// Reset the verse scrolling
		moreVerseDown = false;
		currentScrollPointIndex = -1;
		currentScrollPoint = -1;
		currentScrollStyle = false;
		repaint();
		serviceRepaints();
	}
	
	public void enterGotoMode()
	{
		if (mode == MODE_VIEWING)
		{
			mode = MODE_GOTO;
			
			selection[SELECTION_BOOK] = goBible.currentBookIndex;
			selection[SELECTION_CHAPTER] = goBible.currentChapterIndex;
			selection[SELECTION_VERSE] = goBible.currentVerseIndex;
			
			// Remove all existing commands except for the Goto command
			removeCommand(historyCommand);
			removeCommand(addBookmarkCommand);
			removeCommand(bookmarksCommand);
			removeCommand(searchCommand);
			removeCommand(searchResultsCommand);
			removeCommand(prefsCommand);
			removeCommand(aboutCommand);
			removeCommand(exitCommand);

			if (GoBible.USE_MIDP20)
			{
				// Only remove the send SMS command if the WMA API is supported
				if (sendSMSCommand != null)
				{
					removeCommand(sendSMSCommand);
				}

				// Only remove the send MMS command if the WMA 2.0 API is supported
				if (sendMMSCommand != null)
				{
					removeCommand(sendMMSCommand);
				}
			}
			
			// If full screen mode then remove all commands including Go to command as well
			// as the command listener, if not add a Cancel command as well
			if (goBible.fullScreen)
			{
				removeCommand(gotoCommand);
				setCommandListener(null);
			}
			else
			{
				// Add cancel command
				addCommand(cancelCommand);
			}
						
			// Repaint the canvas to show the goto stuff
			repaint(); serviceRepaints();
			
			// Initially nothing is being edited
			editingSelection = false;
		}
		else if (mode == MODE_GOTO)
		{
			if (validateSelection())
			{
				goBible.gotoPassage(selection[SELECTION_BOOK], selection[SELECTION_CHAPTER], selection[SELECTION_VERSE]);
			}
		}
	}
	
	public void keyRepeated(int keyCode)
	{
		keyPressed(keyCode);
	}
		
	public void paint(Graphics g)
	{
		//long startTime = System.currentTimeMillis();

		// Store width and height to reduce method calls
		// We grab the width and height for each paint just in case
		// the canvas size has changed, eg. because of switching to full screen mode
		width = getWidth();
		height = getHeight();

		if (mode == MODE_LOADING)
		{
			// paint loading message
			paintLoading(g);
		}
		else if (mode == MODE_VIEWING)
		{
			int numberOfVerses = goBible.bibleSource.getNumberOfVerses(goBible.currentBookIndex, goBible.currentChapterIndex);
						
			// Draw the background of the top info bar
			Font boldFont = Font.getFont(Font.FACE_SYSTEM, Font.STYLE_BOLD, goBible.FONT_SIZE_MAP[goBible.fontSize]);
			
			int barHeight = 2 * BAR_PADDING + boldFont.getHeight();

			// Draw the background of the reference line
			g.setColor(highlightColour);
			g.fillRect(0, 0, width, barHeight);

			// Draw the background of the body
			g.setColor(backColour);
			g.fillRect(0, barHeight, width, height - barHeight);
						
			//g.setColor(textColour);
			//g.drawLine(0, barHeight, width, barHeight);
			
			// Draw the text of the top info bar
			g.setFont(boldFont);
			
			String bookName = goBible.bibleSource.getBookName(goBible.currentBookIndex);
			
			String referenceString = " " + goBible.bibleSource.getReferenceString(goBible.currentBookIndex, goBible.currentChapterIndex, goBible.currentVerseIndex);
			
			int bookNameWidth = boldFont.stringWidth(bookName);
			int referenceStringWidth = boldFont.stringWidth(referenceString);
			int ofStringWidth = boldFont.stringWidth(" " + ofString + " " + numberOfVerses);
			
			int coreWidth = bookNameWidth + referenceStringWidth + 2 * BAR_PADDING;
			
			// If there is room to draw the of string then draw it
			if (coreWidth + ofStringWidth <= width)
			{
				// Draw the " of " string
				//g.setGrayScale(64);
				g.setColor(textColour);
				g.drawString(" " + ofString + " " + numberOfVerses, width - BAR_PADDING, BAR_PADDING, Graphics.RIGHT | Graphics.TOP);
			}
			else
			{
				// If there isn't room to draw it then set its width to zero
				ofStringWidth = 0;
			}

			//g.setGrayScale(0);
			g.setColor(textColour);

			// Draw the reference string
			g.drawString(referenceString, width - BAR_PADDING - ofStringWidth, BAR_PADDING, Graphics.RIGHT | Graphics.TOP);
			
			// Clip the bookName if there is not enough room to display the reference
			if (coreWidth + ofStringWidth > width)
			{
				g.clipRect(0, 0, width - (BAR_PADDING + referenceStringWidth + ofStringWidth), height);
			}
			
			// Draw the book name
			drawString(g, bookName, BAR_PADDING, BAR_PADDING, Graphics.LEFT | Graphics.TOP);

			// Prepare to draw the verses
			Font plainFont = Font.getFont(Font.FACE_SYSTEM, goBible.fontStyle == GoBible.FONT_STYLE_BOLD ? Font.STYLE_BOLD : Font.STYLE_PLAIN, goBible.FONT_SIZE_MAP[goBible.fontSize]);
	
			int textWrap = width - TEXT_BORDER_RIGHT;
			int fontHeight = plainFont.getHeight();
			y = barHeight + BAR_PADDING;
			moreVerseDown = false;
			
			g.setClip(0, y, width, height - y);
		
			boolean styleRed = false;
		
			char[] verse = goBible.verseData;

			g.setColor(textColour);

			for (int verseIndex = goBible.currentVerseIndex; verseIndex < numberOfVerses && (y + fontHeight) <= height; verseIndex++)
			{
				int verseOffset = goBible.verseIndex[verseIndex << 1];
				int verseEnd = goBible.verseIndex[(verseIndex << 1) + 1];
	
				//char[] verse = new String(GoBible.verseData, verseOffset, verseLength).toCharArray();
				//chapter[verseIndex].toCharArray();
	
				drawX = TEXT_BORDER_LEFT;
						
				lineStart = verseOffset;
				lastSpace = -1;
				int startChar = verseOffset;
				
				// If this verse is the currently selected verse then check if a scroll point is specified
				if (verseIndex == goBible.currentVerseIndex)
				{
					// Start at the recorded scroll point
					if (currentScrollPointIndex > 0)
					{
						lineStart = startChar = scrollPoints[currentScrollPointIndex];
						styleRed = scrollStyleRed[currentScrollPointIndex];
					}
					else if (currentScrollPoint >= 0)
					{
						lineStart = startChar = currentScrollPoint;
						styleRed = currentScrollStyle;
					}
					
					if (currentScrollPointIndex == -1)
					{
						// A scroll point hasn't been recorded so record one now
						currentScrollPointIndex = 0;
						scrollPoints[currentScrollPointIndex] = verseOffset;
						scrollStyleRed[currentScrollPointIndex] = styleRed;
					}
					
					if (styleRed)
					{
						g.setColor(christWordsColour);
					}
						
					// Clear the next scroll point
					scrollPoints[currentScrollPointIndex + 1] = 0;
				}

				// If we are at the beginning of a verse then draw the verse number
				if (startChar == verseOffset)
				{
					g.setFont(boldFont);
					String verseString = (verseIndex + 1) + "";
					
					if (align == ALIGN_LEFT)
					{
						g.drawString(verseString, drawX, y, Graphics.LEFT | Graphics.TOP);
					}
					else if (align == ALIGN_RIGHT)
					{
						g.drawString(verseString, width - drawX, y, Graphics.RIGHT | Graphics.TOP);
					}
					
					// Offset the x axis by the width of the verse number
					drawX += boldFont.stringWidth(verseString) + 5;
				}

				g.setFont(plainFont);

				for (int charIndex = startChar; (charIndex < verseEnd) && ((y + fontHeight) <= height); charIndex++)
				{
					wrapCheck(verse, charIndex, textWrap, fontHeight, styleRed, g, plainFont, verseIndex, barHeight);

					char currentChar = verse[charIndex];
					
					// Test for a change in style
					if (currentChar == STYLE_RED)
					{
						lastSpace = charIndex;
						
						// Only draw if there is more that can be seen, if not, there is no need to store
						// the current state as it will have been stored in the call to wrapCheck()
						if ((y + fontHeight) <= height)
						{
							// If there is still more to draw after the wrap then draw it
							if (lineStart < charIndex)
							{
								// The style has changed so print out the line so far
								// Draw from line start up to previous character
								drawChars(g, verse, lineStart, charIndex - lineStart, drawX, y);

								// Increment the x offset by the width of the characters
								drawX += plainFont.charsWidth(verse, lineStart, charIndex - lineStart);
								
								// If this is the last character then shift the line down
								if (charIndex == verseEnd - 1)
								{
									y += fontHeight;
								}
							}
							
							// Start the line at the next character
							lineStart = charIndex + 1;
							
							// Even though there may not be a space after the style change
							// we may actually want to wrap on the space before the style change
							// if the first word after the style change won't fit on the screen.
							// So set the last space to the character before the line start.
							//lastSpace = lineStart - 1;
							
							// If the style isn't red then make it and vice versa
							if (!styleRed)
							{
								g.setColor(christWordsColour);
								styleRed = true;
							}
							else
							{
								g.setColor(textColour);
								styleRed = false;
							}
						} // If line of text fits within the height of the display
					}
// use Unicode Space code points
//					else if (currentChar == ' ')
					else if (isSpace(currentChar))
					{
						// if the space is the first character in the verse, don't track it
						if (charIndex != startChar)
							lastSpace = charIndex;
					}
				} // For each character in the verse
				
				// Render last line
				if ((y + fontHeight) <= height && ((lineStart < verseEnd) || (verseOffset == verseEnd)))
				{
					wrapCheck(verse, verseEnd, textWrap, fontHeight, styleRed, g, plainFont, verseIndex, barHeight);

					// If there are still characters to be drawn then draw them
					if ((y + fontHeight) <= height && ((lineStart < verseEnd) || (verseOffset == verseEnd)))
					{
						// Draw from line start up to last space
						drawChars(g, verse, lineStart, verseEnd - lineStart, drawX, y);

						lineStart = verseEnd;
						y += fontHeight;
					}
				}
				
				// Check if there is still more of the currently displayed verse to be seen
				if (lineStart < verseEnd)
				{
					if (verseIndex == goBible.currentVerseIndex)
					{
						moreVerseDown = true;
					}
				}
				else if (verseIndex == (numberOfVerses - 1))
				{
					// This is the end of the chapter
					nextScrollPointVerse = -1;
				}
				else
				{
					// If this isn't the last verse then the next verse will be the
					// next scroll point verse, a scroll point of -1 indicates the
					// start of the verse
					nextScrollPointVerse = verseIndex + 1;
					nextScrollPoint = -1;
					nextScrollStyle = false;
				}
			} // Render verses
		}
		else if (mode == MODE_GOTO)
		{
			paintGoto(g);
		}
		
		if (DEBUG)
		{
			//long duration = System.currentTimeMillis() - startTime;

			g.setClip(0, 0, getWidth(), height);
			
			g.setGrayScale(255);
			
			g.fillRect(80, 20, 48, 60);
			
			g.setGrayScale(0);
			
			//g.drawString("t=" + duration, 82, 3, Graphics.LEFT | Graphics.TOP);
			
			// Draw bible reading times
			g.drawString("l=" + goBible.loadChapterTime, 82, 23, Graphics.LEFT | Graphics.TOP);
			g.drawString("s=" + goBible.skipTime, 82, 43, Graphics.LEFT | Graphics.TOP);
			g.drawString("c=" + goBible.byteToCharTime, 82, 63, Graphics.LEFT | Graphics.TOP);

			/*if (keyCode == UP)
			{
				g.drawString("UP", 64, 50, Graphics.HCENTER | Graphics.TOP);
			}
			else if (keyCode == DOWN)
			{
				g.drawString("DOWN", 64, 50, Graphics.HCENTER | Graphics.TOP);
			}
			else
			{
				g.drawString("Key = " + keyCode, 64, 50, Graphics.HCENTER | Graphics.TOP);
			}*/
		}
	}
	
	/** 
	 * Similar to Graphics.drawString() but supports reversing of text.
	 */
	public static void drawString(Graphics g, String string, int x, int y, int anchor)
	{
		if (reverseCharacters)
		{
			// Grab the char array
			char[] reversedChars = string.toCharArray();
			int length = reversedChars.length;
			
			for (int i = 0; i < (length >> 1); i++)
			{
				// Swap characters
				char temp = reversedChars[i];
				reversedChars[i] = reversedChars[length - i - 1];
				reversedChars[length - i - 1] = temp;
			}
			
			// Draw chars
			g.drawChars(reversedChars, 0, length, x, y, anchor);
		}
		else
		{
			g.drawString(string, x, y, anchor);
		}
	}
	
	/**
	 * Similar to Graphics.drawChars() but takes into account the character direction
	 * of the original text.
	 */
	private void drawChars(Graphics g, char[] verse, int offset, int length, int x, int y)
	{
		if (align == ALIGN_RIGHT)
		{
			x = width - x;
		}

		if (reverseCharacters)
		{
			// Create a copy of the verse data and reverse the chars
			char[] reversed = new char[length];
			
			for (int i = 0; i < length; i++)
			{
				reversed[i] = verse[offset + length - i - 1];
			}
			
			g.drawChars(reversed, 0, length, x, y, anchor);
		}
		else
		{
			g.drawChars(verse, offset, length, x, y, anchor);
		}
	}

	/**
	 * Convert from a Surrogate Pair to the int Code Point
	 *
	 * @param sp1 Surrogate Pair - 1st UTF-16 char
	 * @param sp2 Surrogate Pair - 2nd UTF-16 char
	 * @return integer code point value
	 */
	public static int SPtoCP(char sp1, char sp2)
	{
		int cp = ((sp1 - 0xd800) << 10) + (sp2 - 0xDC00) + 0x10000;
		return cp;
	}

	/**
	 * Returns a surrogate pair number (1 or 2) OR zero if not a surrogate pair
	 */
	public static int isSurrogatePair(char ch)
	{
		if (ch >= 0xD800 && ch <= 0xDBFF) 	// 0xD800-0xDBFF
		{
			return 1;
		}

		if (ch >= 0xDC00 && ch <= 0xDFFF)	// 0xDC00-0xDFFF
		{
			return 2;
		}

		return 0;
	}

	/**
	 * Is this a breaking character?
	 * - See if it's a space, tab, etc...
	 */
	private boolean isBreakingCharacter(char ch)
	{
		boolean result = isSpace(ch);
		if (result)
		{
			return result;
		}

		switch ((int)ch)
		{
			case 0x0009: // TAB
			case 0x00AD: // SOFT HYPHEN (SHY)
				result = true;
				break;
			default:
				break;
		}
		return result;
	}

   /**
	 * Follows the Unicode standard for determining if a code point is
	 * a Unicode space character
	 */
	private boolean isSpace(char ch)
	{
		boolean result = false;
		switch (ch)
		{
			case 0x0020: // SPACE
			case 0x1680: // OGHAM SPACE MARK
			case 0x2000: // EN QUAD
			case 0x2001: // EM QUAD
			case 0x2002: // EN SPACE
			case 0x2003: // EM SPACE
			case 0x2004: // THREE-PER-EM SPACE
			case 0x2005: // FOUR-PER-EM SPACE
			case 0x2006: // SIX-PER-EM SPACE
			case 0x2007: // FIGURE SPACE
			case 0x2008: // PUNCTUATION SPACE
			case 0x2009: // THIN SPACE
			case 0x200A: // HAIR SPACE
			case 0x200B: // ZERO WIDTH SPACE
			case 0x205F: // MEDIUM MATHEMATICAL SPACE
			case 0x3000: // IDEOGRAPHIC SPACE
				result = true;
				break;
			default:
				break;
		}
		return result;
	}

	/**
	 * Follows the Unicode standard for determining if a code point is
	 * a Unicode non-breaking space (NBSP)
	 */
	private boolean isNonBreakingSpace(char ch)
	{
		boolean result = false;
		switch ((int)ch)
		{
			case 0x00A0: // NO-BREAK SPACE
			case 0x202F: // NARROW NO-BREAK SPACE
			case 0x2060: // WORD JOINER (a zero width non-breaking space)
				result = true;
				break;
			default:
				break;
		}
		return result;
	}

	/**
	 * Checks if the line needs to be wrapped and wraps if necessary.
	 * The logic has been updated 11-2009 to consider the following cases:
	 * - use all Unicode space code points
	 * - allow non-breaking code points in the data
	 * - not to break surrogate pairs when in the data
	 */
	private void wrapCheck(char[] verse, int charIndex, int textWrap, int fontHeight, boolean styleRed, Graphics g, Font font, int verseIndex, int barHeight)
	{
		int x = font.charsWidth(verse, lineStart, charIndex - lineStart) + drawX;
		
		if (x >= textWrap)
		{
			/*
			 * Wrap - logic (turns out NonBreaking spaces aren't a factor)
			 * - if lastSpace != -1 then always go there as the break place
			 * - if lastSpace == -1
			 *   + just back up until we are not in a surrogate pair
			 **/
			
			int lineChars = lastSpace - lineStart;
			int charsToBackup = 0;
			// If there was no last space to wrap with then wrap on the previous character
			if (lastSpace == -1)
			{
				charsToBackup = 1;	// the current is not completly visible, so back up One
				char gz = verse[charIndex-1];
				char gz1 = verse[charIndex-2];

				// make sure this isn't the first part of a surrogate pair
				if (isSurrogatePair(verse[charIndex-1]) == 1)
				{
					charsToBackup++;	// have to go to the preceeding character
				}
				//lineChars = charIndex - lineStart;	// dlh not correct - 1;
				lineChars = charIndex - lineStart - charsToBackup;
			}
			
			// Only draw characters if they are visible, and the last space
			// is after the line start, it may be before or after a style change
			if ((y + fontHeight) <= height && (y + fontHeight) > barHeight && lineChars >= 0)
			{
				// Draw from line start up to last space
				drawChars(g, verse, lineStart, lineChars, drawX, y);
			}
			
			// Line start is now the character after the last space
			lineStart = lastSpace != -1 ? lastSpace + 1 : charIndex; // dlh not correct - 1;
			lineStart = lastSpace != -1 ? lastSpace + 1 : charIndex - charsToBackup;
			lastSpace = -1;	// new line, so reset the last space found
//			if (lastSpace != -1)
//			{
//				lastSpace = charIndex;
//			}
			
			y += fontHeight;
			drawX = TEXT_BORDER_LEFT;
			
			// If the current line is not completely visible then use it as the
			// next scrollpoint
			if ((y + fontHeight) > height)
			{
				// If the last line is still part of the same verse then
				// record all of the scroll points so that the user can
				// conveniently scroll forwards and backwards
				if (verseIndex == goBible.currentVerseIndex)
				{
					if (scrollPoints[currentScrollPointIndex + 1] == 0)
					{
						scrollPoints[currentScrollPointIndex + 1] = lineStart;
						scrollStyleRed[currentScrollPointIndex + 1] = styleRed;
					}
				}
				else
				{
					// The last line is in a different verse so record it so
					// the user can conveniently scroll one whole screen forward
					nextScrollPointVerse = verseIndex;
					nextScrollPoint = lineStart;
					nextScrollStyle = styleRed;
					
					// Reset the intraverse scroll point
					//currentScrollPointIndex = -1;
				}
			}
		}
	}
	
	private void paintLoading(Graphics g)
	{
		// Display the loading message in a bold font
		Font font = Font.getFont(Font.FACE_SYSTEM, Font.STYLE_BOLD, Font.SIZE_MEDIUM);
		g.setFont(font);

		// Determine the size of the loading message
		String message = GoBible.getString("UI-Loading") + "...";

		// Determine the width of the reference string
		String bookName = goBible.bibleSource.getBookName(goBible.currentBookIndex);
		String referenceString = " " + goBible.bibleSource.getReferenceString(goBible.currentBookIndex, goBible.currentChapterIndex, goBible.currentVerseIndex);
		
		int bookNameWidth = font.stringWidth(bookName);
		int referenceStringWidth = font.stringWidth(referenceString);
				
		int messageWidth = font.stringWidth(message);
		int messageHeight = font.getHeight();
		
		if ((bookNameWidth + referenceStringWidth) > messageWidth)
		{
			messageWidth = bookNameWidth + referenceStringWidth;
		}
		
		// Determine the size of the box to hold the message
		int boxWidth = messageWidth + 10;
		int boxHeight = messageHeight * 4;
		
		// Make sure the box isn't larger than the canvas
		boxWidth = boxWidth > width ? width : boxWidth; 
		boxHeight = boxHeight > height ? height : boxHeight; 
		
		int boxLeft = (width - boxWidth) >> 1;
		int boxTop = (height - boxHeight) >> 1;
		
		// Draw a grey shadow
		g.setGrayScale(128);
		g.drawRect(boxLeft + 1, boxTop + 1, boxWidth, boxHeight);
		
		// Draw a white background
		g.setGrayScale(255);
		g.fillRect(boxLeft, boxTop, boxWidth, boxHeight);
		
		// Draw a black outline
		g.setGrayScale(0);
		g.drawRect(boxLeft, boxTop, boxWidth, boxHeight);
		
		// Draw the text
		g.drawString(message, width >> 1, (height - 2 * messageHeight) >> 1, Graphics.HCENTER | Graphics.TOP);
		
		int referenceTop = ((height - 2 * messageHeight) >> 1) + messageHeight;
		
		// If the reference string is smaller than the box then draw it centered in the box
		if (bookNameWidth + referenceStringWidth < boxWidth)
		{
			// Determine the start x position of the centred text
			int startX = (width - (bookNameWidth + referenceStringWidth)) >> 1;
			
			// Draw the book name separately as it may need to be drawn reversed
			drawString(g, bookName, startX, referenceTop, Graphics.LEFT | Graphics.TOP);
			g.drawString(referenceString, startX + bookNameWidth, referenceTop, Graphics.LEFT | Graphics.TOP);
		}
		else
		{
			// Draw the reference string aligned to the right of the box and then clip the book name
			g.drawString(referenceString, boxWidth + boxLeft, referenceTop, Graphics.RIGHT | Graphics.TOP);
			
			// Clip the book name
			g.clipRect(0, 0, boxLeft + boxWidth - referenceStringWidth, height);
			
			// Draw the book name
			drawString(g, bookName, boxLeft + 2, referenceTop, Graphics.LEFT | Graphics.TOP);
		}
	}
	
	public void paintGoto(Graphics g)
	{
		// Get the current chapter and verse
		String currentChapter = "" + (selection[SELECTION_CHAPTER] + goBible.bibleSource.getStartChapter(selection[SELECTION_BOOK]));
		String currentVerse = "" + (selection[SELECTION_VERSE] + 1);
	
		// Get book names
		String[] bookNames = goBible.bibleSource.getBookNames();
		
		// Set the font to bold
		Font font = Font.getFont(Font.FACE_SYSTEM, Font.STYLE_BOLD, goBible.FONT_SIZE_MAP[goBible.fontSize]);
		g.setFont(font);

		int fontHeight = font.getHeight();

		int widest = 0;

		// Find the widest book name
		for (int i = 0; i < bookNames.length; i++)
		{
			int width = font.stringWidth(bookNames[i]);
			
			if (width > widest)
			{
				widest = width;
			}
		}
		
		int spaceWidth = font.charWidth(' ');
		
		// Find out the chapter width
		int chapterWidth = font.stringWidth(currentChapter);
		
		int colonWidth = font.charWidth(':');
		
		// Find out the verse width
		int verseWidth = font.stringWidth(currentVerse);
		
		int passageWidth = widest + (2 * spaceWidth) + chapterWidth + spaceWidth + colonWidth + spaceWidth + verseWidth;
		
		// If the passageWidth is wider than the max width of the box (which is two pixels 
		// narrower than the width of the display) then truncate the difference from the
		// book name
		if (passageWidth > (width - 2))
		{
			widest -= passageWidth - (width - 2);
			passageWidth = width - 2;
		}
		
		int passageLeft = (width - passageWidth) >> 1;
		int passageTop = (height - fontHeight) >> 1;
		
		int chapterRight = passageLeft + widest + (2 * spaceWidth) + chapterWidth;
		
		int boxWidth = passageWidth + 20;
		int boxHeight = fontHeight + 20;
		
		// Make sure the box isn't larger than the canvas
		boxWidth = width - 2; //boxWidth > (width - 2) ? (width - 2) : boxWidth; 
		boxHeight = boxHeight > (height - 2) ? (height - 2) : boxHeight; 
		
		int boxLeft = (width - boxWidth) >> 1;
		int boxTop = (height - boxHeight) >> 1;
		
		// Draw a grey shadow
		g.setGrayScale(128);
		g.drawRect(boxLeft + 1, boxTop + 1, boxWidth - 1, boxHeight - 1);
		
		// Draw a white background
		g.setGrayScale(255);
		g.fillRect(boxLeft, boxTop, boxWidth - 1, boxHeight - 1);
		
		// Draw a black outline
		g.setGrayScale(0);
		g.drawRect(boxLeft, boxTop, boxWidth - 1, boxHeight - 1);

		// Draw the selection in blue
		g.setColor(HIGHLIGHT_BACKGROUND_COLOUR);

		int halfSpace = spaceWidth >> 1;
		int doubleSpace = spaceWidth << 1;

		// Draw the background of the selected object
		if (selectionIndex == SELECTION_BOOK)
		{
			g.fillRoundRect(passageLeft - spaceWidth, passageTop - spaceWidth, widest + spaceWidth, fontHeight + doubleSpace, BOX_CORNER, BOX_CORNER);
		}
		else if (selectionIndex == SELECTION_CHAPTER)
		{
			g.fillRoundRect((chapterRight - chapterWidth) - halfSpace, passageTop - spaceWidth, chapterWidth + spaceWidth, fontHeight + doubleSpace, BOX_CORNER, BOX_CORNER);
		}
		else if (selectionIndex == SELECTION_VERSE)
		{
			g.fillRoundRect((chapterRight + spaceWidth + colonWidth + spaceWidth) - halfSpace, passageTop - spaceWidth, verseWidth + spaceWidth, fontHeight + doubleSpace, BOX_CORNER, BOX_CORNER);
		}
		
		// Draw the text in black
		g.setGrayScale(0);
		
		// Draw chapter
		g.drawString(currentChapter, chapterRight, passageTop, Graphics.RIGHT | Graphics.TOP);
		
		// Draw Verse
		g.drawString(" : " + currentVerse, chapterRight, passageTop, Graphics.LEFT | Graphics.TOP);

		// Clip the book name to width of the display area as it may have been reduced to fit on the
		// screen
		g.clipRect(passageLeft, passageTop, widest, fontHeight);
		
		// Draw the book name
		drawString(g, bookNames[selection[SELECTION_BOOK]], passageLeft, passageTop, Graphics.LEFT | Graphics.TOP);
		
		// Draw the menu at the bottom of the screen if in full screen mode
		if (goBible.fullScreen)
		{
			// Reset clip
			g.setClip(0, 0, width, height);
			
			// Draw a white box at the bottom of the screen to contain the menu
			// Allow for font height, plus a pixel above and below the font, plus a pixel
			// for the button outline, plus another pixel
			g.setColor(0xFFFFFFFF);
			
			// Use the largest font for the middle button
			font = Font.getFont(Font.FACE_SYSTEM, Font.STYLE_BOLD, Font.SIZE_LARGE);
			g.setFont(font);
			
			int menuBoxHeight = font.getHeight() + 5;
			
			g.fillRect(0, height - menuBoxHeight, width, menuBoxHeight);
			
			// Draw each button
			drawButton(g, GoBible.getString("UI-Goto"), font, Graphics.HCENTER);
			
			// Use the medium font for the Cancel button
			font = Font.getFont(Font.FACE_SYSTEM, Font.STYLE_PLAIN, Font.SIZE_MEDIUM);
			g.setFont(font);
			
			drawButton(g, GoBible.getString("UI-Cancel"), font, Graphics.RIGHT);
		}
	}
	
	/**
	 * Draws our own custom menu buttons at the bottom of the screen.
	 * @param position is one of the Graphics.LEFT, Graphics.CENTER, or Graphics.RIGHT.
	 */
	public void drawButton(Graphics g, String label, Font font, int position)
	{
		// Width of box depends on width of label plus a pixel either side
		int buttonWidth = font.stringWidth(label) + 4;
		
		int buttonHeight = font.getHeight() + 3;
		
		// y position depends on buttonHeight
		int buttonY = height - buttonHeight;
		
		// x position depends on location (default is left)
		int buttonX = 0;
		
		switch (position)
		{
			case Graphics.RIGHT:
			{
				buttonX = width - buttonWidth - 1;
				break;
			}
			
			case Graphics.HCENTER:
			{
				buttonX = (width >> 1) - (buttonWidth >> 1);
				break;
			}
		}
		
		// Draw a filled round rect background in light blue
		g.setColor(HIGHLIGHT_BACKGROUND_COLOUR);
		
		// We add some height to the button because we don't want to see the bottom
		// round rectangles
		g.fillRoundRect(buttonX, buttonY, buttonWidth, buttonHeight + BOX_CORNER, BOX_CORNER, BOX_CORNER);
		
		// Use a slightly darker colour for the button outline
		g.setColor(HIGHLIGHT_OUTLINE_COLOUR);
		g.drawRoundRect(buttonX, buttonY, buttonWidth, buttonHeight + BOX_CORNER, BOX_CORNER, BOX_CORNER);

		// Draw label in black
		g.setColor(0xFF000000);
		
		g.drawString(label, buttonX + 3, buttonY + 3, Graphics.TOP | Graphics.LEFT);
	}
}
