//
//  SearchResultsList.java
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

public class SearchResultsList extends javax.microedition.lcdui.List implements CommandListener
{
	private GoBible goBible;
	private Vector searchResults;

    public SearchResultsList(GoBible goBible, Vector searchResults, SearchOptions searchSnapshot)
    {
//		String title = GoBible.getString("UI-Results");
//		title += " - " + Integer.toString(searchResults.size());
//		title += " " + GoBible.getString("UI-Found");
        super(GoBible.getString("UI-Results")+" - " + Integer.toString(searchResults.size())+" " + GoBible.getString("UI-Found"), Choice.IMPLICIT);

        if (searchSnapshot == null) {
            searchSnapshot = goBible.searchSnapshot;
        }
        else {
            goBible.searchSnapshot = searchSnapshot;
        }

        if (GoBible.USE_MIDP20)
        {
            setFitPolicy(Choice.TEXT_WRAP_ON);
        }

        this.goBible = goBible;
        this.searchResults = searchResults;

        // Add the results

        for (Enumeration e = searchResults.elements(); e.hasMoreElements(); )
        {
                Object[] entry = (Object[]) e.nextElement();
                int index = ((Integer) entry[0]).intValue();

                int bookIndex = (index >> 16) & 0xff;
                int chapterIndex = (index >> 8) & 0xff;
                int verseIndex = (index) & 0xff;

                append(goBible.bibleSource.getBookName(bookIndex) + " " + goBible.bibleSource.getReferenceString(bookIndex, chapterIndex, verseIndex) + " \"" + ((String) entry[1]) + "\"", null);
        }
        if (goBible.lastSearchIndex >= 0 && goBible.lastSearchIndex < this.size())
        {
                setSelectedIndex(goBible.lastSearchIndex, true);	// restore to the last selected index
                if (isShown() == false)
                {
                        ;
                }
        }
        addCommand(new Command(GoBible.getString("UI-Cancel"), Command.CANCEL, 0));
        if (searchSnapshot != null && searchSnapshot.exhausted == false) {
            addCommand(new Command(GoBible.getString("UI-Search-Next"), Command.SCREEN, 0));
        }

        setCommandListener(this);
    }
	
    public void commandAction(Command command, Displayable display)
    {
        if (command == javax.microedition.lcdui.List.SELECT_COMMAND)
        {
            // View this verse

            // Grab the reference
            int selectedIndex = getSelectedIndex();
            goBible.lastSearchIndex = selectedIndex;	// save this point for future ref
            int index = ((Integer) ((Object[]) searchResults.elementAt(selectedIndex))[0]).intValue();

            goBible.bibleCanvas.requestPassage(
                    (index >> 16) & 0xff,
                    (index >> 8) & 0xff,
                    (index) & 0xff
                    );
            goBible.display.setCurrent(goBible.bibleCanvas);
            goBible.showMainScreen();
        }
        else
        {
            switch (command.getCommandType())
            {
                case Command.OK:
                case Command.CANCEL:
                {
                        // Revert to the previously displayed verse
                        goBible.display.setCurrent(goBible.bibleCanvas);
                        goBible.showMainScreen();
                        break;
                }
                case Command.SCREEN: // search next
                    if (goBible.searchSnapshot == null) {
                        goBible.showSearchScreen();
                    }
                    else {
                        goBible.display.setCurrent(new SearchingCanvas(goBible, goBible.searchSnapshot));
                    }
            }
        }
    }
}
