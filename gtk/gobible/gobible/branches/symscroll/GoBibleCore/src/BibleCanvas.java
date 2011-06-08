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
import java.util.Vector;

/* TODO: reverseChars
 * Implementation Notes:
 * - Not for all paint methods. Digits should not be reversed (i.e.
 * verse numbers, chapter numbers
 * - If the translation contained digits, then they will be incorrectly reversed too.
 * - This presentation bug can only be solved with better understanding of what
 * can be reversed, what cannot. In short, Unicode implementation.
 *
 */

public class BibleCanvas extends SuperCanvas implements CommandListener, Runnable
{
	private final static boolean DEBUG = false;

	private final static int TEXT_BORDER_LEFT = 3;
	private final static int TEXT_BORDER_RIGHT = 3;
	private final static int BAR_PADDING = 2;
	private final static int BOX_CORNER = 10;

	// Modes
	private final static int MODE_SPLASH = 0;
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
        private final static char STYLE_ADD = 2;
        private final static char STYLE_ND = 3;
        private final static char STYLE_QUOTE = 4;
        private final static char STYLE_PROPERNOUN = 5;
        private final static char STYLE_CHAPTER_TITLE = 6; // ACK
        private final static char STYLE_SECTION_TITLE = 7; // BEL

	private final static int COLOUR_RED = 0xB00000;
	private final static int HIGHLIGHT_BACKGROUND_COLOUR = 0xE0E0FF;
	private final static int HIGHLIGHT_OUTLINE_COLOUR = 0xC0C0FF;

	private GoBible goBible;

        private Command gotoCommand = new Command(GoBible.getString("UI-Goto"), Command.OK, 0);
        // for debug purposes only
        //private Command displaceCommand = new Command("UI-Displace", Command.SCREEN, 0);
	private Command searchCommand = new Command(GoBible.getString("UI-Search"), Command.SCREEN, 0);
        private Command searchNextCommand = new Command(GoBible.getString("UI-Search-Next"), Command.SCREEN, 0);
	private Command searchResultsCommand = new Command(GoBible.getString("UI-Search-Results"), Command.SCREEN, 0);
	private Command addBookmarkCommand = new Command(GoBible.getString("UI-Add-Bookmark"), Command.SCREEN, 0);
	private Command bookmarksCommand = new Command(GoBible.getString("UI-Bookmarks"), Command.SCREEN, 0);
	private Command historyCommand = new Command(GoBible.getString("UI-History"), Command.SCREEN, 0);
	private Command sendSMSCommand = new Command(GoBible.getString("UI-Send-SMS"), Command.SCREEN, 0);
        private Command sendMMSCommand;
	private Command prefsCommand = new Command(GoBible.getString("UI-Preferences"), Command.SCREEN, 0);
	private Command keySettingsCommand = new Command(GoBible.getString("UI-Key-Settings"), Command.SCREEN, 0);
	private Command aboutCommand = new Command(GoBible.getString("UI-About"), Command.SCREEN, 0);
	private Command exitCommand = new Command(GoBible.getString("UI-Exit"), Command.EXIT, 0);
	private Command cancelCommand = new Command(GoBible.getString("UI-Cancel"), Command.CANCEL, 0);
        
        private Command[] defaultCommands = {
            gotoCommand, /* Note this is a Command.OK command, so it will not appear on the list of usual commands */
            searchCommand,
            searchNextCommand, /* ExtendedUI */
            searchResultsCommand,
            addBookmarkCommand,
            bookmarksCommand,
            historyCommand,
            sendSMSCommand, /* MIDP2.0 */
            //sendMMSCommand, /* disabled */
            prefsCommand,
            keySettingsCommand, /*Extended UI */
            aboutCommand,
            exitCommand
        };
        private Command[] gotoModeCommands = {
            gotoCommand,
            cancelCommand
        };
        /* Subsequent filters */
        private final Command[] ExtendedUICommands = {
            keySettingsCommand,
            searchNextCommand
        };
        private final Command[] MIDP2Commands = {
            sendSMSCommand
        };

        // Rectangle to determine where my Goto rectangle is. To receive pointer events
        Rectangle gotoRectangle = new Rectangle(0,0,0,0);

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
	
	// Save the width and height for fast access
	int width;
	int height;

        int barHeight;
        TextStyle referenceStyle = new TextStyle();

	// position information used in the Touch event processing
	private int firstX, firstY;
	//private int diffX, diffY;
	private int savedX, savedY;
        private boolean suspendEvents = false;
        // disable caching for devices that don't need it (keypad devices)
        // because keypad devices look better (no flickering)
        // However the trade-off is slower response time.
        public boolean needCache = false;

        private PassageReference currentPassage;
	
	// Goto mode fields
	private int[] selection = new int[3];
	private int selectionIndex = SELECTION_BOOK;
	private boolean editingSelection;
        private int bookNumberOffset = 1;

        private int pixelOffset = 0;
	
	public BibleCanvas(GoBible goBible)
	{
            this.goBible = goBible;
            this.currentPassage = new PassageReference(goBible);
	}

        /**
         * userRequestPassage - used only if this action should store the requested passage in history.
         * 
         * @param bookIndex
         * @param chapterIndex
         * @param verseIndex
         */
        public void userRequestPassage(int bookIndex, int chapterIndex, int verseIndex) {
            goBible.addToHistory(bookIndex, chapterIndex, verseIndex);
            this.requestPassage(bookIndex, chapterIndex, verseIndex, 0);
        }
        /**
         * This and all its overloaded functions are used only if the requested passage references should not be
         * stored in history.
         *
         * @param bookIndex
         * @param chapterIndex
         * @param verseIndex
         */
        public void requestPassage(int bookIndex, int chapterIndex, int verseIndex) {
            this.requestPassage(bookIndex, chapterIndex, verseIndex, 0);
        }
        public void requestPassage(int bookIndex, int chapterIndex, int verseIndex, boolean needValidation) {
            if (needValidation) {
                selection[SELECTION_BOOK] = bookIndex;
                selection[SELECTION_CHAPTER] = chapterIndex;
                selection[SELECTION_VERSE] = verseIndex;
                validateSelection();
            }
            this.requestPassage(bookIndex, chapterIndex, verseIndex, 0);
        }
        public void requestPassage(int bookIndex, int chapterIndex, int verseIndex, int lineOffset) {
            synchronized (currentPassage) { // Because the cache thread can update its reference
                currentPassage.bookIndex = bookIndex;
                currentPassage.chapterIndex = chapterIndex;
                currentPassage.verseIndex = verseIndex;
                currentPassage.lineOffset = lineOffset;
            }
            repaint();
        }
        public int currentBook() {
            return currentPassage.bookIndex;
        }
        public int currentChapter() {
            return currentPassage.chapterIndex;
        }
        public int currentVerse() {
            return currentPassage.verseIndex;
        }

	// the stylus, touch screen, or trackball was pressed
	protected void pointerPressed(int x, int y)
	{
            super.pointerPressed(x, y);
            if (mode == MODE_GOTO) {
                // If we receive a pointer event in Goto mode, the user
                // is trying to use a touchscreen to navigate. But we don't
                // support that yet, so we degrade into Goto Screen.
                if (gotoRectangle.contains(x, y)) {
                    goBible.showGotoScreen();
                }
                else {
                    // enter viewing mode
                    update();
                }
            }
            else if (mode == MODE_VIEWING) {
                if (y <= barHeight) {
                    setFullScreenMode(!goBible.fullScreen);
                    goBible.fullScreen = !goBible.fullScreen;
                    return;
                }

                firstX =  x;
                firstY =  y;
                savedX = savedY = 0;
                suspendEvents = false;
            }
	}

	/**
	 * This method is used to handle the 'Touch' event 'pointerDragged'.
	 *
	 * Simulate the user scrolling by verse, chapter or book depending on how
	 * fast the users 'swipe' gesture is.
	 *
	 * @param x, x location
	 * @param y, y location
	 */
	protected void pointerDragged(int x, int y) {
            super.pointerDragged(x, y);

            needCache = true;

            if (mode == MODE_VIEWING) {
                if (suspendEvents)
                    return;
                double diffX = Math.abs(x - firstX);
                double diffY = Math.abs(y - firstY);
                // Only change chapter if the horizontal distance moved is
                // big enough.
                if (diffX > diffY && diffX * 2 > width) {
                    /* under align LEFT (presumable LTR text)
                     *   swipe right = previous chapter
                     *   swipe left = next chapter
                     *
                     * under align RIGHT (presumably RTL text)
                     *   swipe right = next chapter
                     *   swipe left = previous chapter
                     */
                    if ( (x > firstX) ^ (align == ALIGN_LEFT)) {
                        /* TH: requestNextChapter() */
                        currentPassage.nextChapter();
                    }
                    else {
                        /* TH: requestPreviousChapter() */
                        currentPassage.previousChapter();
                    }
                    
                    suspendEvents=true;
                    repaint();
                }
            }
        }

        protected void pointerMovingRelative(int x, int y, int delX, int delY)
	{
            if (mode == MODE_VIEWING) {
                if (suspendEvents)
                    return;

                double diffX = Math.abs(x - firstX);
                double diffY = Math.abs(y - firstY);
		// if the change in 'x' is greater than the change in 'y'
		if (diffY > diffX)
		{
                    int linesToScroll = (savedY + delY) / TextStyle.fontHeight;
                    pixelOffset = (savedY + delY) % TextStyle.fontHeight;
                    
                    currentPassage.lineOffset -= linesToScroll;

                    savedY = savedY + delY - (linesToScroll * TextStyle.fontHeight);

                    repaint();
                }
            }
	}

	protected void pointerReleased(int x, int y)
	{
            super.pointerReleased(x, y);
            if (mode == MODE_VIEWING) {
                suspendEvents=false;
            }
        }

        protected void pointerHeld(int x, int y) {
            goBible.showGotoScreen();
        }

	public void init()
	{
		// Get the alignment property
		String alignmentString = goBible.getAppProperty("Go-Bible-Align");
		if (alignmentString == null || alignmentString.equalsIgnoreCase("Left"))
		{
			align = ALIGN_LEFT;
			anchor = Graphics.TOP | Graphics.LEFT;
		}
		else if (alignmentString.equalsIgnoreCase("Right"))
		{
			align = ALIGN_RIGHT;
			anchor = Graphics.TOP | Graphics.RIGHT;
		}
                
		// Get the book number offset property
                try {
                    bookNumberOffset = Integer.parseInt(goBible.getAppProperty("Go-Bible-Book-Number-Offset"));
                } catch (Exception e) {
                    bookNumberOffset = 1;
                }
		

                // Issue 110: Extended UI Option
		String extendedUIString = goBible.getAppProperty("Go-Bible-Extended-UI");
                // Defualt is to use Extended UI unless otherwise stated, e.g. no translation available
		if (extendedUIString == null || extendedUIString.equalsIgnoreCase("true") || extendedUIString.equalsIgnoreCase("yes"))
		{
		}
                else {
                    for (int i=0; i < ExtendedUICommands.length; i++) {
                        for (int j=0; j < defaultCommands.length; j++) {
                            if (defaultCommands[j] == ExtendedUICommands[i]) {
                                defaultCommands[j] = null; // disable the option
                            }
                        }
                    }
                }

                // Faux Bold, Italics, Underline
                String p;
                TextStyle.fauxBold = ( (p=goBible.getAppProperty("Go-Bible-Render-Faux-Bold")) != null) && p.equals("yes");
                TextStyle.fauxItalic = ( (p=goBible.getAppProperty("Go-Bible-Render-Faux-Italic")) != null) && p.equals("yes");;
                TextStyle.fauxUnderlined = ( (p=goBible.getAppProperty("Go-Bible-Render-Faux-Underlined")) != null) && p.equals("yes");

                referenceStyle.setProperty(STYLE_CHAPTER_TITLE, true);

		if (GoBible.USE_MIDP20)
		{
                    // Only create the send SMS command if the WMA API is supported
                    try
                    {
                        Class.forName("javax.wireless.messaging.MessageConnection");
                    }
                    catch (ClassNotFoundException e)
                    {
                        System.err.println("SMS Disabled");
                        // Okay we just won't create the command
                        for (int j=0; j < defaultCommands.length; j++) {
                            if (defaultCommands[j] == sendSMSCommand) {
                                defaultCommands[j] = null; // disable the SMS option
                            }
                        }
                    }catch (Exception e){}
                    
                    // Only create the send MMS command if the WMA 2.0 API is supported
                    try
                    {
                        Class.forName("javax.wireless.messaging.MessagePart");
                    }
                    catch (ClassNotFoundException e)
                    {
                        System.err.println("MMS Disabled");
                        // Okay we just won't create the command
                        for (int j=0; j < defaultCommands.length; j++) {
                            if (defaultCommands[j] == sendMMSCommand) {
                                defaultCommands[j] = null; // disable the SMS option
                            }
                        }
                    }catch (Exception e){}
                }

		// Add commands for the main screen
		// If not in full screen mode then add the Go to command here

                // Amendment (10/10/10): Both full screen and non-fullscreen modes have
                // same list of commands. Command lists are managed by
                // remove...Commands() and add...Commands().
                // If special full screen mode command lists are desired, we
                // can amend addDefaultCommands() to detect full-screen status
//		if (!goBible.fullScreen)
//		{
//			addCommand(gotoCommand);
//		}
		
		addDefaultCommands();
	}

	public void addDefaultCommands()
	{
                setCommandListener(this);
                for (int i=0; i < defaultCommands.length; i++) {
                    if (defaultCommands[i] != null)
                        addCommand(defaultCommands[i]);
                }
//		// If in fullScreen mode add the Go to command here
//		if (goBible.fullScreen)
//		{
//			addCommand(gotoCommand);
//		}
	}
	public void removeDefaultCommands()
	{
                for (int i=0; i < defaultCommands.length; i++) {
                    if (defaultCommands[i] != null)
                        removeCommand(defaultCommands[i]);
                }
	}
        public void addGotoModeCommands()
	{
            if (goBible.fullScreen) {
                setCommandListener(null);
            }
            else {
                setCommandListener(this);
                for (int i=0; i < gotoModeCommands.length; i++) {
                    if (gotoModeCommands[i] != null)
                        addCommand(gotoModeCommands[i]);
                }
            }
	}
        public void removeGotoModeCommands()
	{
            if (goBible.fullScreen) {}
            else {
                for (int i=0; i < gotoModeCommands.length; i++) {
                    if (gotoModeCommands[i] != null)
                        removeCommand(gotoModeCommands[i]);
                }
            }
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
         * Called when the Canvas is hidden. We invalidate the cache because
         * we do not know what has changed (e.g. Preferences, Screen Size)
         */
        public void hideNotify() {
            invalidateCache();
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
                            else if (command == keySettingsCommand) {
                                    goBible.showKeySettingsScreen();
                            }
                            else if (command == aboutCommand)
                            {
                                    goBible.showAboutAlert();
                            }
                            else if (command == historyCommand)
                            {
                                    goBible.showHistoryScreen();
                            }
                            else if (command == sendSMSCommand)
                            {
                                    goBible.showSendSMSScreen();
                            }
                            else if (command == sendMMSCommand)
                            {
                                    goBible.showSendMMSScreen();
                            }
                            else if (command == searchCommand)
                            {
                                    goBible.showSearchScreen();
                            }
                            else if (command == searchNextCommand) {
                                    SearchingCanvas.quickSearch(goBible, goBible.lastSearchString, true);
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
                                    removeGotoModeCommands();
                                    addDefaultCommands();
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
                    synchronized (currentPassage) {
			if (keyCode == goBible.keySettings[KeySettingsForm.ACTION_PREV_CHAPTER])
                        {
                            if (currentPassage.verseIndex != 0 || currentPassage.lineOffset != 0) {
                                currentPassage.verseIndex = 0;
                                currentPassage.lineOffset = 0;
                            }
                            else {
                                currentPassage.previousChapter();
                            }
                            keyHandled = true;
                        }
                        else if (keyCode == goBible.keySettings[KeySettingsForm.ACTION_NEXT_CHAPTER])
                        {
                            currentPassage.nextChapter();
                            keyHandled = true;
                        }
                        else if (keyCode == goBible.keySettings[KeySettingsForm.ACTION_PREV_BOOK])
                        {
                            currentPassage.previousBook();
                            keyHandled = true;
                        }
                        else if (keyCode == goBible.keySettings[KeySettingsForm.ACTION_NEXT_BOOK])
                        {
                            currentPassage.nextBook();
                            keyHandled = true;
                        }
                        else if (keyCode == goBible.keySettings[KeySettingsForm.ACTION_PREV_SCREEN])
                        {
                            currentPassage.lineOffset -= linesPerScreen;
                            keyHandled = true;
                        }
                        else if (keyCode == goBible.keySettings[KeySettingsForm.ACTION_NEXT_SCREEN])
                        {
                            currentPassage.lineOffset += linesPerScreen;
                            keyHandled = true;
                        }
                        else if (keyCode == goBible.keySettings[KeySettingsForm.ACTION_PREV_3_LINES])
                        {
                            currentPassage.lineOffset -= 3;
                            keyHandled = true;
                        }
                        else if (keyCode == goBible.keySettings[KeySettingsForm.ACTION_NEXT_3_LINES])
                        {
                            currentPassage.lineOffset += 3;
                            keyHandled = true;
                        }
                        else if (keyCode == goBible.keySettings[KeySettingsForm.ACTION_PREV_1_LINE])
                        {
                            currentPassage.lineOffset -= 1;
                            keyHandled = true;
                        }
                        else if (keyCode == goBible.keySettings[KeySettingsForm.ACTION_NEXT_1_LINE])
                        {
                            currentPassage.lineOffset += 1;
                            keyHandled = true;
                        }
/*                        else if (keyCode == goBible.keySettings[KeySettingsForm.ACTION_PREV_SEARCH_RESULT])
                        {
                            // TODO: Search previous
                            keyHandled = true;
                        }*/
                        else if (keyCode == goBible.keySettings[KeySettingsForm.ACTION_NEXT_SEARCH_RESULT])
                        {
                            if (goBible.lastSearchString == null || goBible.lastSearchString.length() == 0) {
                                goBible.showSearchScreen();
                            } else {
                                SearchingCanvas.quickSearch(goBible, goBible.lastSearchString, true);
                            }
                            keyHandled = true;
                        }

			if (keyHandled)
			{
				repaint();
			}
			else
			{
				int gameAction = getGameAction(keyCode);

				if (gameAction == UP)
				{
                                    currentPassage.lineOffset -= linesPerScreen;
                                    repaint();
				}
				else if (gameAction == DOWN)
				{
                                    currentPassage.lineOffset += linesPerScreen;
                                    repaint();
				}
				else if (gameAction == FIRE)
				{
					enterGotoMode();
				}
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
                            startIndex = goBible.bibleSource.getStartChapter(selection[SELECTION_BOOK] - bookNumberOffset);
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
                    else if ((selectionIndex >= SELECTION_BOOK) && (keyCode >= KEY_NUM0) && (keyCode <= KEY_NUM9))
                    {
                        int value = keyCode - KEY_NUM0;
                        int index = selection[selectionIndex];

                        /* This section is slightly different from previous sections
                         * because we take into account the book offsets.
                         * We cannot cancel a selection because it is below the minimum book number,
                         * but have to validate it thereafter.
                         */

                        // If the selection isn't being edited then replace the current contents
                        // with the keyCode
                        if (!editingSelection)
                        {
                            // The selection can only be modified if the first number isn't zero
                            if (keyCode != KEY_NUM0)
                            {
                                editingSelection = true;
                                selection[selectionIndex] = value;
                            }
                        }
                        // Only add characters to the existing selection if the existing selection
                        // is less than 3 characters
                        else if (index < 100)
                        {
                            // Append the key code to the existing selection
                            selection[selectionIndex] = ( index * 10 ) + value;
                        }

                        int numberOfBooks = goBible.bibleSource.getNumberOfBooks();

                        if (selection[selectionIndex] >= numberOfBooks + bookNumberOffset)
                        {
                            selection[selectionIndex] = index;
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
                                enterGotoMode(); /* enterGotoMode() navigates to passage when in GOTO mode */
                                break;
                            }

                            // Cancel Go to mode
                            case GAME_B:
                            {
                                removeGotoModeCommands();
                                // Switch to viewing mode
                                // Add the removed commands
                                addDefaultCommands();

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
		
		if (selection[SELECTION_BOOK] >= numberOfBooks + bookNumberOffset)
		{
			selection[SELECTION_BOOK] = bookNumberOffset;
			correct = false;
		}
		else if (selection[SELECTION_BOOK] < bookNumberOffset)
		{
			selection[SELECTION_BOOK] = numberOfBooks + bookNumberOffset - 1;
			correct = false;
		}
		
		// Validate the chapter
		int numberOfChapters = goBible.bibleSource.getNumberOfChapters(selection[SELECTION_BOOK] - bookNumberOffset);

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
		int numberOfVerses = goBible.bibleSource.getNumberOfVerses(selection[SELECTION_BOOK] - bookNumberOffset, selection[SELECTION_CHAPTER]);

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
        
	/**
	 * enterViewingMode should be called if the current verse has changed.
	 * Will also repaint the canvas.
	 */
	public void update()
	{
		if (mode == MODE_GOTO);
		{
                        removeGotoModeCommands();
                        addDefaultCommands();
		}
	
		// Turn loading off
		mode = MODE_VIEWING;

                if (!isPainting) {
		// Reset the verse scrolling
                    repaint();
                    serviceRepaints();
                }
	}
	
	public void enterGotoMode()
	{
            if (mode == MODE_VIEWING)
            {
                mode = MODE_GOTO;

                synchronized (currentPassage) {
                    selection[SELECTION_BOOK] = currentPassage.bookIndex + bookNumberOffset;
                    selection[SELECTION_CHAPTER] = currentPassage.chapterIndex;
                    selection[SELECTION_VERSE] = currentPassage.verseIndex;
                }

                removeDefaultCommands();
                addGotoModeCommands();

                // If full screen mode then remove all commands including Go to command as well
                // as the command listener, if not add a Cancel command as well
//			if (goBible.fullScreen)
//			{
//				removeCommand(gotoCommand);
//				setCommandListener(null);
//			}
//			else
//			{
//				// Add cancel command
//				addCommand(cancelCommand);
//			}

                // Repaint the canvas to show the goto stuff
                repaint(); serviceRepaints();

                // Initially nothing is being edited
                editingSelection = false;
            }
            else if (mode == MODE_GOTO)
            {
                if (validateSelection())
                {
                    userRequestPassage(selection[SELECTION_BOOK] - bookNumberOffset, selection[SELECTION_CHAPTER], selection[SELECTION_VERSE]);
                    mode = MODE_VIEWING;
                    removeGotoModeCommands();
                    addDefaultCommands();
                    repaint();
                }
            }
	}
	
	public void keyRepeated(int keyCode)
	{
		keyPressed(keyCode);
	}

        private int linesPerScreen=0;
        private boolean isPainting = false;
        
	public void paint(Graphics g)
	{
		//long startTime = System.currentTimeMillis();

		// Store width and height to reduce method calls
		// We grab the width and height for each paint just in case
		// the canvas size has changed, eg. because of switching to full screen mode

                // because our paints are cached, we need to clear the cache if
                // the screen size has changed.
                isPainting = true;

                int xwidth = getWidth(), xheight = getHeight();
                if (xwidth != width || xheight != height) {
                    invalidateCache();
                    width = xwidth;
                    height = xheight;
                }

                if (mode == MODE_VIEWING)
                {
                    /* TODO: Use TextStyle instead? */
                    Font boldFont = Font.getFont(Font.FACE_SYSTEM, Font.STYLE_BOLD, goBible.FONT_SIZE_MAP[goBible.fontSize]);
                    Font plainFont = Font.getFont(Font.FACE_SYSTEM, goBible.fontStyle == GoBible.FONT_STYLE_BOLD ? Font.STYLE_BOLD : Font.STYLE_PLAIN, goBible.FONT_SIZE_MAP[goBible.fontSize]);

                    int wrapPoint = width - TEXT_BORDER_RIGHT;

                    this.barHeight = 2 * BAR_PADDING + boldFont.getHeight();
                    
                    PaintContext context = new PaintContext(
                            g,
                            plainFont,
                            boldFont,
                            0, barHeight + BAR_PADDING,
                            width, height,
                            wrapPoint);
                    
                    // Do some sanity checks
                    int numberOfChapters = goBible.bibleSource.getNumberOfChapters(
                            currentPassage.bookIndex);
                    if (currentPassage.chapterIndex >= numberOfChapters) {
                        currentPassage.chapterIndex = numberOfChapters - 1;
                        // hardline stance:
                        goBible.showStackTrace(new Exception("Chapter exceeds number of chapters"), "paint()", currentPassage.toString());
                    }
                    int numberOfVerses = goBible.bibleSource.getNumberOfVerses(
                            currentPassage.bookIndex,
                            currentPassage.chapterIndex);
                    if (currentPassage.verseIndex >= numberOfVerses) {
                        currentPassage.verseIndex = numberOfVerses - 1;
                        goBible.showStackTrace(new Exception("Verse exceeds number of verses"), "paint()", currentPassage.toString());
                    }
                    PassageReference pcontext = currentPassage.clone();
                    
                    linesPerScreen = (context.height - context.y) / TextStyle.fontHeight;
                    setStaticAllowance(plainFont.getHeight() / 3);

                    g.setColor(backColour);
                    g.fillRect(0, 0, width, height);

                    // For phones without touch-screens, disable the cache to
                    // minimize flickering.
                    if (! needCache ) {
                        normalizeReferences(context, pcontext);
                        pcontext.cloneInto(currentPassage);
                        paintReference(context, pcontext);
                        context.g.setClip(0, context.y, width, TextStyle.fontHeight * linesPerScreen);
                        paintVerses(context, pcontext);
                        context.g.setClip(0, 0, width, height);
                    }
                    else {
                        paintCached(context, pcontext);
                    }
		}
		else if (mode == MODE_GOTO)
		{
			paintGoto(g);
		}
                
                isPainting = false;
	}

        /**
         * Replaces a substring (needle) in a string (haystack) with another string
         * (replacement)
         * 
         * @param haystack
         * @param needle
         * @param replacement
         * @return The haystack with the substitutions made.
         */
        public static String replaceSubstring(String haystack, String needle, String replacement) {
            StringBuffer returnValue = new StringBuffer();
            int keywordIndex = 0, offset = 0;
            for (
                    keywordIndex = haystack.indexOf(needle, offset);
                    keywordIndex >= 0;
                    offset = keywordIndex + needle.length(),
                    keywordIndex = haystack.indexOf(needle, offset)
            ) {
                returnValue.append(haystack.substring(offset, keywordIndex));
                returnValue.append(replacement);
            }
            returnValue.append(haystack.substring(offset));
            return returnValue.toString();
        }

	public void paintGoto(Graphics g)
	{
		// Get the current chapter and verse
		String currentChapter = "" + (selection[SELECTION_CHAPTER] + goBible.bibleSource.getStartChapter(Math.max(0, selection[SELECTION_BOOK] - bookNumberOffset)));
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

                this.gotoRectangle.x = boxLeft;
                this.gotoRectangle.y = boxTop;
                this.gotoRectangle.width = boxWidth - 1;
                this.gotoRectangle.height = boxHeight - 1;
		
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
		g.drawString(bookNames[Math.max(0, selection[SELECTION_BOOK] - bookNumberOffset)], passageLeft, passageTop, Graphics.LEFT | Graphics.TOP);
		
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
        /** Paint a verse, given a verseIndex and lineOffset.
         *
         * to move a screen up/down, we simply subtract/add $numLinesPerScreen to lineOffset
         *
         */
        private void paintReference(PaintContext context, PassageReference ctx) {
            Graphics g = context.g;

            // Draw the background of the reference line
            g.setColor(highlightColour);
            g.fillRect(0, 0, width, barHeight);

            String bookName = goBible.bibleSource.getBookName(ctx.bookIndex);

            String referenceString = " " + goBible.bibleSource.getReferenceString(ctx.bookIndex, ctx.chapterIndex, ctx.verseIndex);

            String ofStringText  = null;
            StringBuffer ofStringBuffer = new StringBuffer();
            //" " + ofString + " " + goBible.localizeDigits("" + goBible.bibleSource.getNumberOfVerses(ctx.bookIndex, ctx.chapterIndex));
            if (TextStyle.reverseCharacters) { // reverse the order of appearance but not the digits
                ofStringBuffer.append(goBible.localizeDigits("" + goBible.bibleSource.getNumberOfVerses(ctx.bookIndex, ctx.chapterIndex)));
                ofStringBuffer.reverse();
                ofStringBuffer.insert(0, " ");
                ofStringBuffer.insert(0, ofString);
                ofStringBuffer.insert(0, " ");
            } else {
                ofStringBuffer.append(" ");
                ofStringBuffer.append(ofString);
                ofStringBuffer.append(" ");
                ofStringBuffer.append(goBible.localizeDigits("" + goBible.bibleSource.getNumberOfVerses(ctx.bookIndex, ctx.chapterIndex)));
            }
            ofStringText = ofStringBuffer.toString();

            int bookNameWidth = referenceStyle.stringWidth(bookName);
            int referenceStringWidth = referenceStyle.stringWidth(referenceString);
            int ofStringWidth = referenceStyle.stringWidth(ofStringText);

            int coreWidth = bookNameWidth + referenceStringWidth + 2 * BAR_PADDING;
            
            referenceStyle.setGraphics(g);
            // If there is room to draw the of string then draw it
            if (coreWidth + ofStringWidth <= width)
            {
                if (align == ALIGN_LEFT) {
                    // Draw the " of " string
                    //g.setGrayScale(64);
                    //g.setColor(textColour); // now set by referenceStyle
                    referenceStyle.drawString(ofStringText, width - BAR_PADDING, BAR_PADDING, Graphics.RIGHT | Graphics.TOP);
                } else {
                    referenceStyle.drawString(ofStringText, BAR_PADDING, BAR_PADDING, Graphics.LEFT | Graphics.TOP);
                }
            }
            else
            {
                // If there isn't room to draw it then set its width to zero
                ofStringWidth = 0;
            }

            //g.setGrayScale(0);
            g.setColor(textColour);

            String shortenedBookName = bookName;
            int lettersToUtilize = (bookName.length() - 2) / 2;
            
            // Draw the reference string
            if (align == ALIGN_LEFT) {
                referenceStyle.drawString(referenceString, width - BAR_PADDING - ofStringWidth, BAR_PADDING, Graphics.RIGHT | Graphics.TOP);
                // Clip the bookName if there is not enough room to display the reference

                while (coreWidth + ofStringWidth > width && lettersToUtilize > 0)
                {
                    shortenedBookName = bookName.substring(0, lettersToUtilize)
                            + "\u2026" // ellipsis
                            + bookName.substring(bookName.length() - lettersToUtilize, bookName.length());
                    coreWidth = referenceStyle.stringWidth(shortenedBookName) + referenceStringWidth + 2 * BAR_PADDING;
                    lettersToUtilize--;
                }
                if (lettersToUtilize == 0) {
                    g.clipRect(0, 0, width - (BAR_PADDING + referenceStringWidth + ofStringWidth), height);
                    referenceStyle.drawString(bookName, BAR_PADDING, BAR_PADDING, Graphics.LEFT | Graphics.TOP);
                }
                else {
                    /** if it is too long, shorten the book name until it is short enough */
                    referenceStyle.drawString(shortenedBookName, BAR_PADDING, BAR_PADDING, Graphics.LEFT | Graphics.TOP);
                }
            } else {
                referenceStyle.drawString(referenceString, BAR_PADDING + ofStringWidth, BAR_PADDING, Graphics.LEFT | Graphics.TOP);
                // Clip the bookName if there is not enough room to display the reference
//                if (coreWidth + ofStringWidth > width)
//                {
//                        g.clipRect(width - (BAR_PADDING + referenceStringWidth + ofStringWidth), 0, (BAR_PADDING + referenceStringWidth + ofStringWidth), height);
//                }
                while (coreWidth + ofStringWidth > width && lettersToUtilize > 0)
                {
                    shortenedBookName = bookName.substring(0, lettersToUtilize)
                            + "\u2026" // ellipsis
                            + bookName.substring(bookName.length() - lettersToUtilize, bookName.length());
                    coreWidth = referenceStyle.stringWidth(shortenedBookName) + referenceStringWidth + 2 * BAR_PADDING;
                    lettersToUtilize--;

//                        g.clipRect(0, 0, width - (BAR_PADDING + referenceStringWidth + ofStringWidth), height);
                }
                if (lettersToUtilize == 0) {
                    g.clipRect(0, 0, width - (BAR_PADDING + referenceStringWidth + ofStringWidth), height);
                    referenceStyle.drawString(bookName, width - BAR_PADDING, BAR_PADDING, Graphics.RIGHT | Graphics.TOP);
                }
                else {
                    // Draw the book name
                    referenceStyle.drawString(shortenedBookName, width - BAR_PADDING, BAR_PADDING, Graphics.RIGHT | Graphics.TOP);
                }
            }

        }

        /** Changes a passage reference (Book, Chapter, Verse, Line Offset)
         * to a reference with the smallest positive line offset.
         */
        private void normalizeReferences(PaintContext context, PassageReference ctx) {
            int vLines, new_y = context.y;

            System.out.println ("BEFORE normalize " + ctx);

            // Negative line offset. Scroll "UP"
            if (ctx.lineOffset < 0) {
                while (ctx.lineOffset < 0) {
                    ctx.verseIndex --;
                    if (ctx.verseIndex < 0) {
                        int oldOffset = ctx.lineOffset;

                        ctx.previousChapter();
                        ctx.lineOffset = oldOffset;
                        ctx.verseIndex = goBible.bibleSource.getNumberOfVerses(
                                ctx.bookIndex,
                                ctx.chapterIndex) - 1;
                    }
                    vLines = countLines(context, ctx);
                    System.out.println(Thread.currentThread().getName() + "[-] verse " + ctx.verseIndex + " lines " + vLines);
                    ctx.lineOffset += vLines;
                }
            }
            // Positive Line Offset. Scroll "Down"
            else {
                while (true) {
                    vLines = countLines(context, ctx);

                    //System.out.println("[+] verse " + ctx.verseIndex + " lines " + vLines);

                    if (ctx.lineOffset > vLines) {
                        ctx.verseIndex ++;
                        ctx.lineOffset -= vLines;
                    }
                    else { // enough lines have been skipped
                        break;
                    }

                    if (ctx.verseIndex >=
                            goBible.bibleSource.getNumberOfVerses(ctx.bookIndex, ctx.chapterIndex)) {
                        System.out.println(Thread.currentThread().getName() + "Verse exceed limit: " + ctx);

                        int oldOffset = ctx.lineOffset;
                        boolean newBook = (ctx.chapterIndex == goBible.bibleSource.getNumberOfChapters(ctx.bookIndex) - 1);
                        ctx.nextChapter();
                        //restore the line offset
                        ctx.lineOffset = oldOffset;
                    }
                }
            }

            //System.out.println( "context.y = " + context.y);
            //System.out.println ("AFTER Line offset: " + ctx.lineOffset + ", Verse Index: " + ctx.verseIndex);
        }
        private void paintVerses(PaintContext context, PassageReference ctx) {
            paintVerses(context, ctx, null);
        }
        private void paintVerses(PaintContext context, PassageReference ctx, Vector references) {
            int lineCount = 0;

            /* paint part */
            while ((context.y + TextStyle.fontHeight) <= context.height) {
                LinedReference r = null;
                if (references != null) {
                    r = new LinedReference(goBible);
                    ctx.cloneInto(r);
                    r.lineIndex = lineCount;
                }
                lineCount += paintVerse(context, ctx);
                System.err.println("(paintVerses) " + ctx + " LC: " + lineCount);
                if (Thread.currentThread() == cacheThread && lineCount % 5 == 0) repaint(); // since it's now partially painted?

                if (references != null) {
                    references.addElement(r);
                }
                ctx.nextVerse();
            }
        }
        private int countLines(PaintContext context, PassageReference ctx) {
            PassageReference ctx2 = ctx.clone();
            PaintContext context2 = context.clone();

            ctx2.lineOffset = 0;
            int i=1;
            //try {
                i = countAndPaintLines(false, context2, ctx2);
            //} catch (Exception e) {
            //    goBible.showStackTrace(e, "countLines", ctx2.toString());
            //}
            return i;
        }
        private int paintVerse(PaintContext context, PassageReference ctx) {
            int i = 1;
//            try {
                i = countAndPaintLines(true, context, ctx);
//            } catch (Exception e) {
//                goBible.showStackTrace(e, "paintVerse", ctx.toString());
//            }
            return i;
        }
        /**
         * This function:
         * a) Count lines in a verse if doPaint is false
         * b) Paint lines if doPaint is true
         * c) Need to account for style changes
         *
         * This function now evaluates each line first (inclusive of all style
         * changes) before painting. This is in anticipation of style changes
         * within a _word_, and it being undesirable to wrap a line in the
         * middle of a word.
         *
         * In addition, for Chinese text (although as of 3 Sep 2010 the Chinese
         * version does not make use of any formatting control characters), which
         * does not use spaces at all, it is incorrect behaviour to wrap lines
         * at the style change.
         *
         * Optimization notes: This function and its related functions make
         * egregious use of StringBuffer.toString().substring() etc. Can definitely
         * be optimized further
         *
         * @param doPaint Whether actual painting is required
         * @param context
         * @param ctx
         * @return
         */
        private int countAndPaintLines(boolean doPaint, PaintContext context, PassageReference ctx) {
            if (goBible.currentBookIndex != ctx.bookIndex || goBible.currentChapterIndex != ctx.chapterIndex) {
                System.out.println(Thread.currentThread().getName() + "UNOPTIMIZED Forced navigation");
                goBible.gotoPassage(ctx.bookIndex, ctx.chapterIndex, ctx.verseIndex);
            }

            int verseOffset = goBible.verseIndex[ctx.verseIndex * 2];
            int verseEnd = goBible.verseIndex[ctx.verseIndex * 2 + 1];
            char[] verse = goBible.verseData;

            // The line counter. No painting code will be called until currentLine >= 0
            int currentLine = - ctx.lineOffset;

            StringBuffer verseBuffer;

            boolean needChapterHeading = ctx.verseIndex == 0;
            String bookHeading = "\n\010" + goBible.bibleSource.getBookName(ctx.bookIndex) + "\010\n";

            String chapterNumber = goBible.localizeDigits(Integer.toString(ctx.chapterIndex + 1));
            if (TextStyle.reverseCharacters) {
                chapterNumber = new StringBuffer(chapterNumber).reverse().toString();
            }
            String chapterHeading = "\n\010" + replaceSubstring(
                    goBible.getString("UI-Chapter-Reference-%1"),
                    "%1",
                    chapterNumber
                    ) + "\010\n";
            boolean needBookHeading = needChapterHeading && ctx.chapterIndex == 0;
            boolean endOfBook = (ctx.chapterIndex == goBible.bibleSource.getNumberOfChapters(ctx.bookIndex) - 1) &&
                    (ctx.verseIndex == goBible.bibleSource.getNumberOfVerses(ctx.bookIndex, ctx.chapterIndex) - 1);
            int verseNumberOffset = 1;

            // the current style at the current character
            context.textStyle.setGraphics(context.g);
            context.textStyle.setStyleValue((short)0);
            context.x = TEXT_BORDER_LEFT;

            /* Can be optimized by doing the following:
             * 1. Check initial characters: needXXHeading?
             * 2. Gather all the characters until style change (without doing the above checks)
             * 3. Print the gathered characters accordingly
             *
             * Benefit: Reduces the number of comparisons that has to be done per character.
             */

            /* Step 0: Create verse buffer.
             * Step 1: If necessary, inject the replacement text into certain
             * style chars
             */
            verseBuffer = new StringBuffer("");
            verseBuffer.append(verse, verseOffset, verseEnd - verseOffset);
            
            /* Check initial characters */
            int charIndex = 0;
            
            /* increment or decrement the verseNumberOffset */
            while (true) {
                //System.err.println(","); // debug
                
                if (verseBuffer.charAt(0) == 0x0e) {
                    verseNumberOffset--;
                    verseBuffer.deleteCharAt(0);
                    continue;
                }
                else if(verseBuffer.charAt(0) == 0x0f) {
                    verseNumberOffset++;
                    verseBuffer.deleteCharAt(0);
                    continue;
                }
                else {
                    break;
                }
            }

            int verseNumberPosition = 0;

            //System.err.println("Reached A");

            // inject book name
            if (needBookHeading) {
                verseBuffer.insert(verseNumberPosition, bookHeading);
                verseNumberPosition += bookHeading.length();
            }
            // skip all section names e.g. Book I, Book II
            if (needChapterHeading) {
                while ( true ) {
                    char currentChar = verseBuffer.charAt(verseNumberPosition);
                    if (currentChar == STYLE_SECTION_TITLE) {
                        for (verseNumberPosition++;
                            verseNumberPosition < verseBuffer.length();
                            verseNumberPosition++) {
                            if (verseBuffer.charAt(verseNumberPosition) == STYLE_SECTION_TITLE) {
                                verseNumberPosition ++;
                                break;
                            }
                        }
                    } else {
                        break;
                    }
                }
                // inject chapter number
                verseBuffer.insert(verseNumberPosition, chapterHeading);
                verseNumberPosition += chapterHeading.length();
            }
            // skip all titles
            if (needChapterHeading) {
                while ( true ) {
                    char currentChar = verseBuffer.charAt(verseNumberPosition);
                    if (currentChar == STYLE_CHAPTER_TITLE) {
                        for (verseNumberPosition++;
                            verseNumberPosition < verseBuffer.length();
                            verseNumberPosition++) {
                            if (verseBuffer.charAt(verseNumberPosition) == '\r') {
                                verseBuffer.setCharAt(verseNumberPosition, STYLE_CHAPTER_TITLE); // TODO:
                            }
                            if (verseBuffer.charAt(verseNumberPosition) == STYLE_CHAPTER_TITLE) {
                                verseNumberPosition ++;
                                break;
                            }
                        }
                    } else {
                        break;
                    }
                }
            }
            // inject verse number
            String verseNumberString = goBible.localizeDigits((verseNumberOffset + ctx.verseIndex) + "");
            if (TextStyle.reverseCharacters) { // In general we do not reverse digits
                verseNumberString = new StringBuffer(verseNumberString).reverse().toString();
            }
            verseBuffer.insert(verseNumberPosition, "\010" + verseNumberString + "\010\u00A0");
            verseEnd = verseBuffer.length();
            //System.err.println("Finished A");

            /* Inject chapter header, section header and verse number into it... */
            /* use non-breaking space after verse number? */

            /* Step 1: Grab next word, noting where the styles are
             * Step 2: Test word width. If short enough, print
             * Step 3: If not short enough, binary search within word until it's
             * short enough, then print first segment. Repeat Step 2 for next segment
             *
             */

            PWord currentWord = new PWord();
            currentWord.canBreakBefore = false; // cannot break at the start of the verse

            for (; charIndex < verseEnd; charIndex++) {
                char currentChar = verseBuffer.charAt(charIndex);
                boolean charIsSpace = GBCToolkit.isSpace(currentChar);
                boolean isNewline = currentChar == '\n'
                            || currentChar == STYLE_SECTION_TITLE
                            || currentChar == STYLE_CHAPTER_TITLE;
                boolean endOfVerse = (charIndex == verseEnd - 1);
                
                if (currentChar >= 20) { // this is an ordinary letter.
                    currentWord.pushLetter(currentChar);
                }
                else if (currentChar < 10) { // this is a style.
                    currentWord.pushStyle(currentChar); // store the style
                }

                if (charIsSpace || isNewline || endOfVerse) { // conditions to paint word
                    int segmWidth = 0;

                    segmWidth = currentWord.calculateWidth(context.textStyle, currentWord.wordBuffer.length() - (charIsSpace?1:0));

                    WHILE_NEED_WRAP:
                    while (segmWidth + context.x > context.wrapPoint) {
                        if (currentWord.canBreakBefore) {
                            if (doPaint && currentLine >= 0) {
                                context.y += TextStyle.fontHeight;
                            }
                            currentWord.canBreakBefore = false;
                            currentLine++;
                            context.x = TEXT_BORDER_LEFT;
                        }
                        else { /* do binary search to print long words */
                            // In this method, each step takes 1/2 the time, but
                            // the number of interations is fixed,
                            // iterations = log_2 N;

                            // impt for Chinese. Need to test speed;

                            // if you compare current test string and test string + 1 char,
                            // each step twice the time, but (possibly) fewer number of iterations
                            int finalPoint = 0, upperLimit = currentWord.wordBuffer.length(), lowerLimit = 0;
                            while (true) {
                                int testGap = (upperLimit - lowerLimit);
                                int testPoint = testGap / 2;

                                segmWidth = currentWord.calculateWidth(context.textStyle, lowerLimit + testPoint);
                                //segmWidth2 = currentWord.calculateWidth(context.textStyle, lowerLimit + testPoint);

//                                if (
//                                        segmWidth + context.x > context.wrapPoint &&
//                                        segmWidth2 + context.x < context.wrapPoint
//                                        ) {
//                                    finalPoint = lowerLimit + testPoint;
//                                }
                                
                                if (segmWidth + context.x > context.wrapPoint) // above limit
                                {
                                    if (testGap <= 2) { // just above limit
                                        finalPoint = lowerLimit + testPoint;

                                        // test assertion
//                                        int width = currentWord.calculateWidth(context.textStyle, lowerLimit + testPoint - 1);
//                                        GBCToolkit.myAssert(width + context.x <= context.wrapPoint, "IS NOT JUST ABOVE LIMIT");
                                        break;
                                    }
                                    upperLimit = lowerLimit + testPoint;
                                }
                                else { // below limit
                                    if (testGap <= 2) { // just below limit
                                        finalPoint = upperLimit;
                                        
                                        // test assertion
//                                        int width = currentWord.calculateWidth(context.textStyle, lowerLimit + testPoint - 1);
//                                        GBCToolkit.myAssert(width + context.x <= context.wrapPoint, "IS NOT JUST BELOW LIMIT");
                                        break;
                                    }
                                    lowerLimit = lowerLimit + testPoint;
                                }
                            }

                            int wrapIndex = finalPoint - 1;
                            if (GBCToolkit.isSurrogatePair(currentWord.wordBuffer.charAt(wrapIndex)) == 1) {
                                wrapIndex --; // have to go to the preceeding character
                            }
                            // keep painting and dropping lines until we have finished the word in our buffer.
                            paintWord2(doPaint && currentLine >= 0, context, ctx, currentWord, wrapIndex);
                            System.out.println("Break word, printed: " + currentWord.wordBuffer.toString().substring(0, wrapIndex));

                            // delete painted words
                            currentWord.wordBuffer.delete(0, wrapIndex);
                            // delete used styles / set them to zero...
                            int stylesLength = currentWord.styles.size();
                            for (int i=0; i < stylesLength; i++) {
                                PStyle style = (PStyle) currentWord.styles.elementAt(i);
                                style.position -= wrapIndex;
                                if (style.position < 0) {
                                    style.position = 0;
                                }
                            }
                            // Break one line
                            if (doPaint && currentLine >= 0) {
                                context.y += TextStyle.fontHeight;
                            }
                            currentLine++;
                            context.x = TEXT_BORDER_LEFT;
                            // recalculate required width.
                            segmWidth = currentWord.calculateWidth(context.textStyle, currentWord.wordBuffer.length() - (charIsSpace?1:0));
                        }
                    }
                    
                    // print the word if it has not already been printed.
                    paintWord2(doPaint && currentLine >= 0, context, ctx, currentWord, currentWord.wordBuffer.length());
                    currentWord = new PWord();

                    if (isNewline) {
                        // Break one line
                        if (doPaint && currentLine >= 0) {
                            context.y += TextStyle.fontHeight;
                        }
                        currentLine++;
                        context.x = TEXT_BORDER_LEFT;
                        currentWord.canBreakBefore = false;
                    }
                    // end the verse
                    if (endOfVerse && endOfBook) {
                        if (doPaint && currentLine >= (2 - linesPerScreen)) {
                            context.y += TextStyle.fontHeight * (linesPerScreen - 2 + Math.min(0, currentLine));
                        }
                        currentLine += linesPerScreen - 2;
                    }
                }
            }
            if (doPaint && currentLine >= 0) {
                context.y += TextStyle.fontHeight;
            }
            currentLine ++;

            return (currentLine < 0)? 0: currentLine;
        }

        private void paintWord2
                (boolean doPaint,
                PaintContext context,
                PassageReference ctx,
                PWord word,
                int length) {
            int stylesCount = word.styles.size();
            int position = 0, segmWidth = 0, finalPosition = length;
            int x = context.x, y = context.y;
           
            for (int i = 0; i <= stylesCount;  i++) {
                PStyle nextStyle = null;

                finalPosition = length;
                if (i != stylesCount) {
                    nextStyle = (PStyle) word.styles.elementAt(i);

                    if (nextStyle.position <= position) { // turn on all the styles that need to be activated (i.e. before current position)
                        context.textStyle.toggleProperty(nextStyle.style);
                        continue;
                    }
                    else { // if there is a style ahead of us, set its position as the paint limit
                        finalPosition = nextStyle.position;
                    }
                }

                String s1 = word.wordBuffer.toString().substring(position, finalPosition);
                int width = context.textStyle.stringWidth(s1);

                if (doPaint) {
                    if (align == ALIGN_LEFT) {
                        context.textStyle.drawString(s1, x, y, anchor);
                        //System.out.println("Word painted: " + s1 + " with style " +context.textStyle.getStyleValue());
                    }
                    else if (align == ALIGN_RIGHT) {
                        context.textStyle.drawString(s1, context.width - x, y, anchor);
                    }
                }
                x += width;
                position = finalPosition;
                if (nextStyle != null) {
                    context.textStyle.toggleProperty(nextStyle.style);
                }
            }
            context.x = x;
        }
        private PaintCache caches[] = new PaintCache[2]; // TODO: increase number of caches
        private PassageReference lastDrawnPosition = null;
        
        private void paintCached(PaintContext context, PassageReference ctx) {
            boolean isRelativeMovement = false;
            boolean drawn = false;

            System.out.println(Thread.currentThread().getName() + "REQUEST PAINT " + ctx.toString());

            /* TH: Testing if data has been cached */

            if (caches[0] != null && caches[1] != null && lastDrawnPosition != null && (lastDrawnPosition.sameVerse(ctx))) {
                int lineDiff = (lastDrawnPosition.lineOffset - ctx.lineOffset);
                
                if ( Math.abs(lineDiff) <= Math.abs(linesPerScreen) ) {
                    isRelativeMovement = true;
                }
            }

            if (isRelativeMovement) {
                System.out.println(Thread.currentThread().getName() + "RELATIVE MOVEMENT");
            Y: for (int i=0; i<2; i++) {
                // cache[0] and cache[1] are not null because of (isRelativeMovement)
                        /*
                         * TH: NOT CACHED. Therefore,
                         * 1. Obsolete older caches
                         * 2. Build Cache
                         * 3. Has thread been started? // no need synchronized(thread) because... only this thread starts the paint thread?
                         *  a. No? Start new thread, add to task list.
                         * synchronized(taskList) {
                         * 4. Is the queue length zero?
                         *  a. No? Add to task list.
                         *  b. Yes? Start new thread, add to task list.
                         * }
                         * 5. Paint "Loading..." message
                         *
                         * Task.isObsolete -- once set to true, do not bother with this Task again.
                         */
                for (int j = 0; j < caches[i].verseReferences.size(); j++) {
                    LinedReference r = (LinedReference) caches[i].verseReferences.elementAt(j);

                    if (r.sameVerse(ctx)) {
                        System.out.println(Thread.currentThread().getName() + "Requested and matched verse " + ctx.verseIndex);
                        System.out.println(Thread.currentThread().getName() + "  at Cache " + i + " Ref #" + j);
                        // actual line offset from the first line of first cache.
                        int lineOffsetDifference = ctx.lineOffset - r.lineOffset + r.lineIndex + (i * linesPerScreen);
      
                        if (-linesPerScreen <= lineOffsetDifference && lineOffsetDifference < 0) {
                            //System.out.println("Scenario 2, net offset = " + lineOffsetDifference);
                            PassageReference temp = ((LinedReference)caches[0].verseReferences.elementAt(0)).clone();
                            temp.lineOffset -= linesPerScreen;
                            
                            caches[1].paintTask.isObsolete = true;
                            // first becomes second.
                            caches[1] = caches[0];
                            // build the first;
                            caches[0] = queueCache(temp);
                            lineOffsetDifference += linesPerScreen;
                        }
                        else if (linesPerScreen <= lineOffsetDifference && lineOffsetDifference <= 2 * linesPerScreen) {
                            //System.out.println("Scenario 3, net offset = " + lineOffsetDifference);
                            PassageReference temp = ((LinedReference)caches[1].verseReferences.elementAt(0)).clone();
                            temp.lineOffset += linesPerScreen;

                            caches[0].paintTask.isObsolete = true;
                            // second becomes first.
                            caches[0] = caches[1];
                            // build the second image
                            caches[1] = queueCache(temp);
                            lineOffsetDifference -= linesPerScreen;
                        }
                        else if (0 <= lineOffsetDifference && lineOffsetDifference < linesPerScreen){
                            // normal
                        }
                        else {
                            break Y;
                        }
                        // paint the picture
                        //System.out.println("net offset = " + lineOffsetDifference);
                        //context.g.setClip(0, context.y, width, height - context.y - BAR_PADDING);
                        // We do not show partial lines, because the flickering of the partial line
                        // can be annoying.
                        context.g.setClip(0, context.y, width, TextStyle.fontHeight * linesPerScreen);
                        
                        if (caches[0].image != null) {
                        context.g.drawImage(
                                caches[0].image,
                                0, context.y - TextStyle.fontHeight * lineOffsetDifference + pixelOffset,
                                Graphics.LEFT | Graphics.TOP);
                        }
                        if (caches[1].image != null) {
                        context.g.drawImage(
                                caches[1].image,
                                0, context.y - TextStyle.fontHeight * (lineOffsetDifference - linesPerScreen)  + pixelOffset,
                                Graphics.LEFT | Graphics.TOP);
                        }
                        context.g.setClip(0, 0, width, height);

                        // now, based on the references saved, find our exact position
                        // in terms of BK, VE, CH
                        System.out.println("LINE_OFFSET_DIFF = " + lineOffsetDifference);
                        LinedReference lastRef = null;
                        X: for (int l = 0; l < 2; l++) {
                            for (int k = 0; k < caches[l].verseReferences.size(); k++) {
                                LinedReference currentRef = (LinedReference)caches[l].verseReferences.elementAt(k);
                                //System.out.println("  k=" + k + " lineIndex: " + currentRef.lineIndex + " " + currentRef);

                                if (currentRef.lineIndex + (l * linesPerScreen) > lineOffsetDifference) {
                                    lastRef.cloneInto(lastDrawnPosition);
                                    lastDrawnPosition.lineOffset = lineOffsetDifference - lastRef.lineIndex + lastRef.lineOffset;
                                    break X;
                                }
                                lastRef = currentRef;
                            }
                        }
                        drawn = true;
                        break Y;
                    }
                    
                }
            }}

            if (!drawn) {
                for (int i=0; i < taskList.size(); i++) {
                    ((PaintTask)taskList.elementAt(i)).isObsolete = true;
                }
                if (isRelativeMovement) {
                    System.out.println(Thread.currentThread().getName() + "RELATIVE movement exceeding limit. Not using Cache");
                }
                else {
                    System.out.println(Thread.currentThread().getName() + "DIRECT movement. Not using Cache");
                }
                
                PassageReference temp = ctx.clone();
                temp.lineOffset += linesPerScreen;

                //System.out.println("Before first queueCache");
                caches[0] = queueCache(ctx);

                //System.out.println("Before second queueCache");
                caches[1] = queueCache(temp);
                
                //System.out.println("after queueCache");
                
                if (caches[0].image != null) {
                    context.g.drawImage(caches[0].image, 0, barHeight + BAR_PADDING, Graphics.LEFT | Graphics.TOP);
                }
                System.out.println(Thread.currentThread().getName() + "I");

                lastDrawnPosition = ctx.clone();
            }
            else {
                lastDrawnPosition.cloneInto(ctx);
            }
            lastDrawnPosition.cloneInto(currentPassage);
            paintReference(context, lastDrawnPosition);
            System.out.println("Thread: " + Thread.currentThread().getName() + " - LAST POSITION STORED " + lastDrawnPosition.toString());
        }
        private void invalidateCache() {
            if (caches[0] != null) {
                caches[0].paintTask.isObsolete = true;
            }
            if (caches[1] != null) {
                caches[1].paintTask.isObsolete = true;
            }
            caches[0] = caches[1] = null;
        }
        private void buildCache(PaintCache cache, PassageReference ctx) {
            int cacheWidth = width, cacheHeight = TextStyle.fontHeight * linesPerScreen;

            cache.image = Image.createImage(cacheWidth, cacheHeight);
            Graphics g = cache.image.getGraphics();

            g.setColor(backColour);
            g.fillRect(0, 0, cacheWidth, cacheHeight);
            
            Font boldFont = Font.getFont(Font.FACE_SYSTEM, Font.STYLE_BOLD, goBible.FONT_SIZE_MAP[goBible.fontSize]);
            Font plainFont = Font.getFont(Font.FACE_SYSTEM, goBible.fontStyle == GoBible.FONT_STYLE_BOLD ? Font.STYLE_BOLD : Font.STYLE_PLAIN, goBible.FONT_SIZE_MAP[goBible.fontSize]);

            PaintContext context = new PaintContext(g, plainFont, boldFont, 0, 0, cacheWidth, cacheHeight, cacheWidth - TEXT_BORDER_RIGHT);

            normalizeReferences(context, ctx);
            System.out.println(Thread.currentThread().getName() + "before paintVerses");
            paintVerses(context, ctx, cache.verseReferences);
            System.out.println(Thread.currentThread().getName() + "after paintVerses");
        }

        private final Vector taskList = new Vector();
        private Thread cacheThread = null;
        private PaintCache queueCache(PassageReference ctx) {
            PaintTask task = new PaintTask(goBible);

            System.out.println(Thread.currentThread().getName() + "QUEUED " + ctx);

            ctx.cloneInto(task);
            task.paintCache = new PaintCache();
            task.paintCache.paintTask = task;
            task.lineIndex = 0;
            task.paintCache.verseReferences.addElement(task);
            
            if (cacheThread == null || ! cacheThread.isAlive()) {
                cacheThread = new Thread(this);
                taskList.addElement( task );
                cacheThread.start();
                return task.paintCache;
            }
            else {
                synchronized(taskList) {
                    taskList.addElement( task );
                    taskList.notifyAll();
                }
                return task.paintCache;
            }
        }
        public void run() { // Runnable
            while (taskList != null) {
                synchronized(taskList) {
                    if (taskList.isEmpty()) {
                        try {
                            taskList.wait();
                        }catch(InterruptedException ie) {}
                        continue;
                    }
                }
                PaintTask task = (PaintTask)taskList.elementAt(0);
                if (task.isObsolete) {
                    taskList.removeElementAt(0);
                    continue;
                }
                // clone task for multithreading purposes. buildCache modifies
                // the reference pass to it.
                buildCache(task.paintCache, task.clone());

                // We know verseReferences[0] equivalent to verseReferences[1]
                // Any future tasks with the same verse as verseReferences[0] can be
                // simplified relative to verseReferences[1]
                LinedReference simplerRef = (LinedReference)task.paintCache.verseReferences.elementAt(1);

                for (int i = 1; i < taskList.size(); i++) {
                    PaintTask taskToUpdate = (PaintTask)taskList.elementAt(i);

                    synchronized (taskToUpdate) {
                        if (taskToUpdate.sameVerse(task)) {
                            int newLineOffset = taskToUpdate.lineOffset - task.lineOffset + simplerRef.lineOffset;

                            LinedReference lastRef = simplerRef;
                            for (int j = 2; j < task.paintCache.verseReferences.size(); j++) {
                                LinedReference r = (LinedReference)task.paintCache.verseReferences.elementAt(j);

                                if (r.lineIndex >= newLineOffset) {
                                    taskToUpdate.bookIndex = lastRef.bookIndex;
                                    taskToUpdate.chapterIndex = lastRef.chapterIndex;
                                    taskToUpdate.verseIndex = lastRef.verseIndex;
                                    taskToUpdate.lineOffset = newLineOffset - lastRef.lineIndex;
                                    break;
                                }
                                lastRef = r;
                            }
                        }
                    }
                }
                synchronized (currentPassage) {
                    if (currentPassage.sameVerse(task)) {
                        currentPassage.bookIndex = simplerRef.bookIndex;
                        currentPassage.chapterIndex = simplerRef.chapterIndex;
                        currentPassage.verseIndex = simplerRef.verseIndex;
                        currentPassage.lineOffset -= (task.lineOffset - simplerRef.lineOffset);
                    }
                }
                synchronized (lastDrawnPosition) {
                    if (lastDrawnPosition.sameVerse(task)) {
                        lastDrawnPosition.bookIndex = simplerRef.bookIndex;
                        lastDrawnPosition.chapterIndex = simplerRef.chapterIndex;
                        lastDrawnPosition.verseIndex = simplerRef.verseIndex;
                        lastDrawnPosition.lineOffset -= (task.lineOffset - simplerRef.lineOffset);
                    }
                }
                simplerRef.cloneInto(task);
                taskList.removeElementAt(0);

                if (! task.isObsolete ) {
                    repaint();
                }
            }
        }
        private class Rectangle {
            public int x, y, width, height;

            public Rectangle(int x, int y, int w, int h) {
                this.x=x;
                this.y=y;
                this.width = w;
                this.height = h;
            }
            public Rectangle() {
                this(0,0,0,0);
            }
            public boolean contains(int x, int y) {
                if (this.x <= x && x <= this.x + this.width &&
                        this.y <= y && y <= this.y + this.height)
                {
                    return true;
                }
                return false;
            }
        }

        private class PWord {
            public StringBuffer wordBuffer = new StringBuffer();
            public Vector styles = new Vector();
            public boolean canBreakBefore = true;

            public void pushStyle(char style) {
                styles.addElement(new PStyle(style, wordBuffer.length()));
            }
            public void pushLetter(char s) {
                wordBuffer.append(s);
            }
            public void popLetter() {
                wordBuffer.deleteCharAt(wordBuffer.length() - 1);
            }

            public int calculateWidth(TextStyle currentStyle, int length) {
                short saveStyle = currentStyle.getStyleValue();
                int stylesCount = this.styles.size();
                int position = 0, segmWidth = 0, finalPosition = 0;

                for (int i = 0; i <= stylesCount;  i++) {
                    PStyle nextStyle = null;

                    finalPosition = length;
                    if (i != stylesCount) {
                        nextStyle = (PStyle) this.styles.elementAt(i);

                        if (nextStyle.position <= position) {
                            currentStyle.toggleProperty(nextStyle.style);
                            continue;
                        }
                        else {
                            finalPosition = nextStyle.position;
                        }
                    }
                    segmWidth += currentStyle.substringWidth(this.wordBuffer.toString(), position, finalPosition - position);
                    position = finalPosition;
                }

                currentStyle.setStyleValue(saveStyle);
                return segmWidth;
            }
        }
        private class PStyle {
            char style;
            int position;
            public PStyle(char style, int position) {
                this.style=style;
                this.position=position;
            }
        }
}

class PaintContext {
    Graphics g;
    Font plainFont, boldFont;
    int x, y, wrapPoint, height, width;
    int actualVerse, actualLineOffset;
    TextStyle textStyle = new TextStyle();

    public PaintContext() {
        this(null, null, null, 0, 0, 0,0,0);
    }
    public PaintContext(Graphics g, Font pf, Font bf, int x, int y, int w, int h, int tw) {
        this.g=g;
        this.plainFont=pf;
        this.boldFont=bf;
        this.x=x;
        this.y=y;
        this.width =w;
        this.height = h;
        this.wrapPoint=tw;
    }
    public PaintContext clone() {
        PaintContext pc = new PaintContext();

        pc.g = this.g;
        pc.plainFont = this.plainFont;
        pc.boldFont = this.boldFont;
        pc.x = this.x;
        pc.y = this.y;
        pc.wrapPoint = this.wrapPoint;
        pc.height = this.height;
        pc.width = this.width;
        pc.actualLineOffset = this.actualLineOffset;
        pc.actualVerse = this.actualVerse;
        this.textStyle.cloneInto(pc.textStyle);
        
        return pc;
    }

    /**
     * If there is the linked cache after this, changes the current context
     * so that the next paint action takes place in the linked cache.
     *
     * @return false if there is no linked cache, ending the paint.
     * true if the context has been updated.
     */
    protected boolean nextCache() {
        return false;
    }
}

/**
 * A PassageReference is a reference to a passage + line numbers.
 *
 * This is contrasted with GoBible class, which is now used purely
 * as an abstraction for loading data.
 * @author Daniel
 */
class PassageReference {
    int verseIndex;
    int lineOffset;
    int bookIndex;
    int chapterIndex;
    private GoBible goBible;

    public PassageReference(GoBible goBible) {
        this.goBible = goBible;
    }
    public synchronized PassageReference clone() {
        PassageReference ctx = new PassageReference(this.goBible);
        ctx.verseIndex = verseIndex;
        ctx.lineOffset = lineOffset;
        ctx.bookIndex = bookIndex;
        ctx.chapterIndex = chapterIndex;
        return ctx;
    }
    public boolean sameChapter(PassageReference c) {
        return bookIndex == c.bookIndex && chapterIndex == c.chapterIndex;
    }
    public boolean sameBook(PassageReference c) {
        return bookIndex == c.bookIndex;
    }
    public boolean sameVerse(PassageReference c) {
        return bookIndex == c.bookIndex && chapterIndex == c.chapterIndex && verseIndex == c.verseIndex;
    }
    public synchronized void cloneInto(PassageReference ctx) {
        synchronized (ctx) {
            ctx.verseIndex = verseIndex;
            ctx.lineOffset = lineOffset;
            ctx.bookIndex = bookIndex;
            ctx.chapterIndex = chapterIndex;
        }
    }
    public String toString() {
        return "Bk " + bookIndex + " Ch " + chapterIndex + " Ve " + verseIndex + " Ln " + lineOffset;
    }

    /* Navigation functions */
    public void nextChapter() {
        chapterIndex ++;
        if (chapterIndex >= goBible.bibleSource.getNumberOfChapters(bookIndex))
        {
                nextBook();
        }
        lineOffset = verseIndex = 0;
    }
    public void previousChapter() {
        chapterIndex --;
        if (chapterIndex < 0)
        {
            previousBook();
            
            // Go to the last chapter in the previous book
            chapterIndex = goBible.bibleSource.getNumberOfChapters(bookIndex) - 1;
        }
        lineOffset = verseIndex = 0;
    }

    public void nextBook() {
        // Go to the next book
        bookIndex++;

        // Wrap
        if (bookIndex >= goBible.bibleSource.getNumberOfBooks())
        {
            bookIndex = 0;
        }
        lineOffset = chapterIndex = verseIndex = 0;
    }
    public void previousBook() {
        // Go to the previous book
        bookIndex--;

        // Wrap
        if (bookIndex < 0) {
            bookIndex = goBible.bibleSource.getNumberOfBooks() - 1;
        }

        lineOffset = chapterIndex = verseIndex = 0;
    }

    public void nextVerse() {
        verseIndex ++;
        if (verseIndex >= goBible.bibleSource.getNumberOfVerses(bookIndex, chapterIndex))
        {
                nextChapter();
        }
        lineOffset = 0;
    }
    public void previousVerse() {
        verseIndex --;
        if (verseIndex < 0)
        {
                previousChapter();
        }
        lineOffset = 0;   
    }
}
class LinedReference extends PassageReference {
    int lineIndex = 0;
    public LinedReference(GoBible goBible){
        super(goBible);
    }
}
class PaintTask extends LinedReference {
    boolean isObsolete = false;
    PaintCache paintCache = null;
    public PaintTask(GoBible goBible){
        super(goBible);
    }
}

class PaintCache {
    public Image image = null;
    public java.util.Vector verseReferences;
    public PaintTask paintTask;

    public PaintCache() {
        verseReferences = new java.util.Vector();
    }
}

/* Extra control chars:
 *
 * public static final String tags[] = {
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
 */
