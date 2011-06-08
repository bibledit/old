//
//  BookmarksList.java
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

public class BookmarksList extends javax.microedition.lcdui.List implements CommandListener
{
	private GoBible goBible;
	private Command viewCommand = new Command(GoBible.getString("UI-View"), Command.ITEM, 0);
	private Command backCommand = new Command(GoBible.getString("UI-Back"), Command.CANCEL, 0);
	private Command cancelCommand = new Command(GoBible.getString("UI-Cancel"), Command.CANCEL, 0);
	private Command deleteBookmarkCommand = new Command(GoBible.getString("UI-Delete-Bookmark"), Command.SCREEN, 0);
	private Command deleteAllBookmarksCommand = new Command(GoBible.getString("UI-Delete-All"), Command.SCREEN, 0);
	private Command deleteCommand = new Command(GoBible.getString("UI-Delete"), Command.SCREEN, 0);
	private Command deleteAllCommand = new Command(GoBible.getString("UI-Delete-All"), Command.SCREEN, 0);

	private Alert deleteAlert;
	
	private int indexToDelete;
	
	/**
	 * Create the bookmarks list by adding the bookmarks.
	 */
	public BookmarksList(GoBible goBible)
	{
		super(GoBible.getString("UI-Bookmarks"), Choice.IMPLICIT);
		
		if (GoBible.USE_MIDP20)
		{
			setFitPolicy(Choice.TEXT_WRAP_ON);
		}
		
		this.goBible = goBible;
		
		// Add the bookmarks
		
		for (Enumeration e = goBible.bookmarks.elements(); e.hasMoreElements(); )
		{
			BookmarkEntry bookmark = (BookmarkEntry) e.nextElement();
			
			append(goBible.bibleSource.getBookName(bookmark.bookIndex) + " " + goBible.bibleSource.getReferenceString(bookmark.bookIndex, bookmark.chapterIndex, bookmark.verseIndex) + " \"" + bookmark.excerpt + "\"", null);
		}
		
		
		// Don't show Delete commands if there are no Bookmarks
		if (!goBible.bookmarks.isEmpty())
		{
			setSelectCommand(viewCommand);
			addCommand(deleteBookmarkCommand);
			// Don't show the Delete All command if there is only one bookmark
			if (goBible.bookmarks.size() > 1)
			{
				addCommand(deleteAllBookmarksCommand);
			}
		}
		addCommand(backCommand);
		
		setCommandListener(this);
	}

	public void commandAction(Command command, Displayable display)
	{
		if (display == this)
		{
			if (command == viewCommand)
			{
				// View this verse
				
				// Grab the reference
				int selectedIndex = getSelectedIndex();
				
				if (selectedIndex != -1)
				{
					// Get the bookmark entry
					BookmarkEntry bookmark = (BookmarkEntry) goBible.bookmarks.elementAt(selectedIndex);

                                        goBible.bibleCanvas.userRequestPassage(
                                                bookmark.bookIndex & 0xff,
                                                bookmark.chapterIndex & 0xff,
                                                bookmark.verseIndex & 0xff);
					goBible.display.setCurrent(goBible.bibleCanvas);
                                        goBible.showMainScreen();
				}
			}
			else if (command == deleteBookmarkCommand)
			{
				// Get the selected index
				indexToDelete = getSelectedIndex();
				
				if (indexToDelete != -1)
				{
					// Confirm with the user to remove 
					deleteAlert = new Alert(GoBible.getString("UI-Delete-Bookmark"), GoBible.getString("UI-Delete-Bookmark-Alert"), null, AlertType.CONFIRMATION);
					
					deleteAlert.addCommand(cancelCommand);
					deleteAlert.addCommand(deleteCommand);
					
					deleteAlert.setCommandListener(this);
					
					goBible.display.setCurrent(deleteAlert, this);
				}
			}
			else if (command == deleteAllBookmarksCommand)
			{
					// Confirm with the user to remove
					deleteAlert = new Alert(GoBible.getString("UI-Delete-Bookmark"), GoBible.getString("UI-Delete-All-Bookmarks-Alert"), null, AlertType.CONFIRMATION);
					deleteAlert.addCommand(cancelCommand);
					deleteAlert.addCommand(deleteAllCommand);

					deleteAlert.setCommandListener(this);

					goBible.display.setCurrent(deleteAlert, this);
			}
			else
			{
				switch (command.getCommandType())
				{
					case Command.OK:
					case Command.BACK:
					case Command.CANCEL:
					{
						// Revert to the previously displayed verse
						goBible.display.setCurrent(goBible.bibleCanvas);
						goBible.showMainScreen();
						break;
					}
				}
			}
		}
		else if (display == deleteAlert)
		{
			boolean shouldReturn = false;

			if (command == deleteCommand)
			{
				// Remove the bookmark from the List
				delete(indexToDelete);
				
				// Remove the bookmark from the Vector
				goBible.bookmarks.removeElementAt(indexToDelete);

				// if after deleting a bookmark, if the count is at zero, just return
				// if the count is one, then remove the delete all command
				if (goBible.bookmarks.isEmpty())
					shouldReturn = true;
				if (goBible.bookmarks.size() == 1)
				{
					if (GoBible.USE_MIDP20)
					{
						removeCommand(deleteAllBookmarksCommand);
					}
				}

			}
			else if (command == deleteAllCommand)
			{
				// Remove the bookmarks from the List
				deleteAll();

				// Remove the bookmarks from the Vector
				goBible.bookmarks.removeAllElements();

				shouldReturn = true;
			}

			if (shouldReturn)
			{
				// Revert to the previously displayed verse
				goBible.display.setCurrent(goBible.bibleCanvas);
				goBible.showMainScreen();
				return;
			}
			
			// Display the BookmarksList again
			goBible.display.setCurrent(this);
		}
	}
}
