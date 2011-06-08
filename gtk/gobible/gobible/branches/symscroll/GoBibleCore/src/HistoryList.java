//
//  HistoryList.java
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

public class HistoryList extends List implements CommandListener
{
    /* TODO:
     * Delete History entry
     * Delete all History entries
     */
    private Command viewCommand = new Command(GoBible.getString("UI-View"), Command.ITEM, 0);
    private Command backCommand = new Command(GoBible.getString("UI-Back"), Command.CANCEL, 0);
    private Command cancelCommand = new Command(GoBible.getString("UI-Cancel"), Command.CANCEL, 0);
    private Command deleteHistoryCommand = new Command(GoBible.getString("UI-Delete-History-Entry"), Command.SCREEN, 0);
    private Command deleteAllHistoryCommand = new Command(GoBible.getString("UI-Delete-All-History-Entries"), Command.SCREEN, 0);
    private Command deleteCommand = new Command(GoBible.getString("UI-Delete-History-Entry"), Command.SCREEN, 0);
    private Command deleteAllCommand = new Command(GoBible.getString("UI-Delete-All-History-Entries"), Command.SCREEN, 0);

    private Alert deleteAlert;

    private int indexToDelete;

    // maps the item index to the excerpt index
    private int indices[] = new int[GoBible.HISTORY_SIZE];

    private GoBible goBible;

    public HistoryList(GoBible goBible)
    {
        super(GoBible.getString("UI-History"), Choice.IMPLICIT);

        for (int i=0; i<indices.length; i++) {
            indices[i] = -1;
        }

        if (GoBible.USE_MIDP20)
        {
            setFitPolicy(Choice.TEXT_WRAP_ON);
        }

        this.goBible = goBible;

        // Add the history
        int i = goBible.historyStartIndex;
        do
        {
            if (goBible.historyExcerpts[i] != null && !goBible.historyExcerpts[i].equals(""))
            {
                int bookIndex = goBible.historyVerseIndices[i * 3] & 0xFF;
                int chapterIndex = goBible.historyVerseIndices[i * 3 + 1] & 0xFF;
                int verseIndex = goBible.historyVerseIndices[i * 3 + 2] & 0xFF;
                
                int assignedIndex = append(goBible.bibleSource.getBookName(bookIndex) + " " + goBible.bibleSource.getReferenceString(bookIndex, chapterIndex, verseIndex) + " \"" + goBible.historyExcerpts[i] + "\"", null);
                indices[assignedIndex] = i;
            }

            i++;

            if (i >= GoBible.HISTORY_SIZE)
            {
                i = 0;
            }
        }
        while (i != goBible.historyStartIndex);

        addCommands();
    }

    private void addCommands() {
        if (this.size() > 0)
        {
            setSelectCommand(viewCommand);
            addCommand(deleteHistoryCommand);
            // Don't show the Delete All command if there is only one bookmark
            if (this.size() > 1)
            {
                addCommand(deleteAllHistoryCommand);
            }
        }
        addCommand(backCommand);
        setCommandListener(this);
     
    }

    public void commandAction(Command command, Displayable display)
    {
        if (display == this) {
            if (command == viewCommand)
            {
                // View this verse

                // Grab the reference
                int selectedIndex = getSelectedIndex();

                if (selectedIndex != -1)
                {
                    // Convert to cyclic history list
                    selectedIndex += goBible.historyStartIndex;

                    if (selectedIndex >= GoBible.HISTORY_SIZE)
                    {
                        selectedIndex -= GoBible.HISTORY_SIZE;
                    }

                    goBible.bibleCanvas.userRequestPassage(
                        goBible.historyVerseIndices[selectedIndex * 3] & 0xff,
                        goBible.historyVerseIndices[selectedIndex * 3 + 1] & 0xff,
                        goBible.historyVerseIndices[selectedIndex * 3 + 2] & 0xff);

                    goBible.display.setCurrent(goBible.bibleCanvas);
                    goBible.showMainScreen();
                }
            }
            else if (command == deleteHistoryCommand) {
                // Get the selected index
                indexToDelete = getSelectedIndex();

                if (indexToDelete != -1)
                {
                    // Confirm with the user to remove
                    deleteAlert = new Alert(GoBible.getString("UI-Delete-History-Entry"), GoBible.getString("UI-Delete-History-Entry-Alert"), null, AlertType.CONFIRMATION);

                    deleteAlert.addCommand(cancelCommand);
                    deleteAlert.addCommand(deleteCommand);

                    deleteAlert.setCommandListener(this);

                    goBible.display.setCurrent(deleteAlert, this);
                }
            }
            else if (command == deleteAllHistoryCommand) {
                deleteAlert = new Alert(GoBible.getString("UI-Delete-History-Entry"), GoBible.getString("UI-Delete-All-History-Entries-Alert"), null, AlertType.CONFIRMATION);

                deleteAlert.addCommand(cancelCommand);
                deleteAlert.addCommand(deleteAllCommand);

                deleteAlert.setCommandListener(this);

                goBible.display.setCurrent(deleteAlert, this);

            }
            else
            {
                switch (command.getCommandType())
                {
                    case Command.SCREEN:
                        /* TODO */
                        break;
                    case Command.OK:
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

        else if (display == deleteAlert) {
            boolean shouldReturn = false;
            if (command == deleteCommand) {
                // Remove the history from the List
                delete(indexToDelete);

                for (int i = 0; i < indices.length; i++) {
                    System.err.print(indices[i] + ", ");
                }
                System.err.println("");

                goBible.historyExcerpts[indices[indexToDelete]] = null;

                // now that the entry at this index has been deleted, we
                // need to shift all references above it.
                for (int i=indexToDelete; i < indices.length - 1; i++) {
                    indices[i] = indices[i + 1];
                }
                indices[indices.length - 1] = -1; // not really necessary


                for (int i = 0; i < indices.length; i++) {
                    System.err.print(indices[i] + ", ");
                }
                System.err.println("");
                
                if (size() == 0)
                    shouldReturn = true;
                
                if (size() == 1)
                {
                    removeCommand(deleteAllHistoryCommand);
                }

            }
            else if (command == deleteAllCommand) {
                // Remove all history from the list
                this.deleteAll();

                for (int i=0; i < goBible.historyExcerpts.length; i++) {
                    goBible.historyExcerpts[i] = null;
                }

                shouldReturn = true;

            }
            if (shouldReturn) {
                    // Revert to the previously displayed verse
                    goBible.display.setCurrent(goBible.bibleCanvas);
                    goBible.showMainScreen();
                    return;
            }

            // Display the HistoryList again
            goBible.display.setCurrent(this);
        }
    }
}
