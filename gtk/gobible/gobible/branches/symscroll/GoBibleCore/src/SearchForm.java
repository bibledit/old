//
//  SearchForm.java
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

public class SearchForm extends Form implements CommandListener, ItemCommandListener, ItemStateListener
{
    private GoBible goBible;

    private TextField searchTextField;
    private ChoiceGroup bookChoiceGroup;
    private ChoiceGroup fromBookChoiceGroup, toBookChoiceGroup;

    private Command searchNextCommand = new Command(GoBible.getString("UI-Search-Next"), Command.ITEM, 0);
    private Command searchCommand = new Command(GoBible.getString("UI-Search"), Command.OK, 0);
    private Command cancelCommand = new Command(GoBible.getString("UI-Cancel"), Command.CANCEL, 0);

    // Book Index Constants
    private final static int GENESIS = 0;
    private final static int DEUTERONOMY = 4;
    private final static int JOSHUA = 5;
    private final static int ESTHER = 16;
    private final static int JOB = 17;
    private final static int SONG_OF_SOLOMON = 21;
    private final static int ISAIAH = 22;
    private final static int DANIEL = 26;
    private final static int HOSEA = 27;
    private final static int MALACHI = 38;
    private final static int MATTHEW = 39;
    private final static int JOHN = 42;
    private final static int ROMANS = 44;
    private final static int HEBREWS = 57;
    private final static int JAMES = 58;
    private final static int JUDE = 64;
    private final static int REVELATION = 65;

    private final static int[][] BOOK_RANGES = new int[][]
    {
            {GENESIS, REVELATION}, // All
            {-1, -1}, // Current book
            {MATTHEW, REVELATION}, // New Testament
            {GENESIS, MALACHI}, // Old Testament
            {MATTHEW, JOHN}, // Gospels
            {ROMANS, HEBREWS}, // Paul's Letters
            {JAMES, JUDE}, // Other Letters
            {GENESIS, DEUTERONOMY}, // Books of Moses
            {JOSHUA, ESTHER}, // Historical
            {JOB, SONG_OF_SOLOMON}, // Poetic
            {ISAIAH, DANIEL}, // Major Prophets
            {HOSEA, MALACHI}, // Minor Prophets
    };

    private String[] bookNames;

    /**
     * Creates a search for with search criteria and from and to books to search in.
     */
    public SearchForm(GoBible goBible)
    {
            super(GoBible.getString("UI-Search"));

            this.goBible = goBible;

            fromBookChoiceGroup = new ChoiceGroup(GoBible.getString("UI-From-Book") + ":", Choice.POPUP);
            toBookChoiceGroup = new ChoiceGroup(GoBible.getString("UI-To-Book") + ":", Choice.POPUP);

            // Add all of the books to the from and to book popups
            bookNames = goBible.bibleSource.getBookNames();

            for (int i = 0; i < bookNames.length; i++)
            {
                    fromBookChoiceGroup.append(bookNames[i], null);
                    toBookChoiceGroup.append(bookNames[i], null);
            }

            // If no last from and to books are set then set them to the first and last books
            if (goBible.lastFromBook == -1)
            {
                    goBible.lastFromBook = 0;
                    goBible.lastToBook = bookNames.length - 1;
            }

            fromBookChoiceGroup.setSelectedIndex(goBible.lastFromBook, true);
            toBookChoiceGroup.setSelectedIndex(goBible.lastToBook, true);

            searchTextField = new TextField(GoBible.getString("UI-Search-Text") + ":", goBible.lastSearchString, 30, TextField.ANY);
            searchTextField.addCommand(searchNextCommand);
            searchTextField.setItemCommandListener(this);

            bookChoiceGroup = new ChoiceGroup(GoBible.getString("UI-Books") + ":", Choice.EXCLUSIVE);
            bookChoiceGroup.append(GoBible.getString("UI-All"), null);
            bookChoiceGroup.append(bookNames[goBible.bibleCanvas.currentBook()], null);

            // If this collection contains the entire Bible then add options for
            // the testaments and other combinations
            if (bookNames.length == 66)
            {
                    bookChoiceGroup.append(GoBible.getString("UI-New-Testament"), null);
                    bookChoiceGroup.append(GoBible.getString("UI-Old-Testament"), null);
                    bookChoiceGroup.append(GoBible.getString("UI-Gospels"), null);
                    bookChoiceGroup.append(GoBible.getString("UI-Pauls-Letters"), null);
                    bookChoiceGroup.append(GoBible.getString("UI-Other-Letters"), null);
                    bookChoiceGroup.append(GoBible.getString("UI-Books-Of-Moses"), null);
                    bookChoiceGroup.append(GoBible.getString("UI-Historical"), null);
                    bookChoiceGroup.append(GoBible.getString("UI-Poetic"), null);
                    bookChoiceGroup.append(GoBible.getString("UI-Major-Prophets"), null);
                    bookChoiceGroup.append(GoBible.getString("UI-Minor-Prophets"), null);
            }

            bookChoiceGroup.append(GoBible.getString("UI-Set-Range"), null);
            this.setItemStateListener(this);

            // Default to All
            bookChoiceGroup.setSelectedIndex(0, true);

            append(searchTextField);
            append(bookChoiceGroup);
            /*append(fromBookChoiceGroup);
            append(toBookChoiceGroup);*/

            addCommand(searchCommand);
            addCommand(cancelCommand);

            setCommandListener(this);
    }

    public void commandAction(Command command, Item i) {
        if (command == searchNextCommand) {
            goBible.lastSearchString = searchTextField.getString();
            SearchingCanvas.quickSearch(goBible, goBible.lastSearchString, true);
        }

    }

    public void commandAction(Command command, Displayable display)
    {
            switch (command.getCommandType())
            {
                    case Command.OK:
                    {
                            if (command == searchCommand)
                            if (!searchTextField.getString().equals(""))
                            {
                                    goBible.lastSearchString = searchTextField.getString();

                                    int selectedIndex = bookChoiceGroup.getSelectedIndex();
                                    int from = 0, to = 0;

                                    if (selectedIndex == 0)
                                    {
                                            // All
                                            to = bookNames.length - 1;
                                    }
                                    else if (selectedIndex == 1)
                                    {
                                            // Current Book
                                            from = to = goBible.bibleCanvas.currentBook();
                                    }
                                    else if (selectedIndex > 1 && selectedIndex < bookChoiceGroup.size() - 1)
                                    {
                                            // Look up the range
                                            from = BOOK_RANGES[selectedIndex][0];
                                            to = BOOK_RANGES[selectedIndex][1];
                                    }
                                    else if (selectedIndex == bookChoiceGroup.size() - 1) {
                                        /* if for some reason, the order of books is reversed. The search algo
                                         supports "wrapping-around" from Revelation to Genesis. Not sure if we
                                         want this feature. */
                                        if (toBookChoiceGroup.getSelectedIndex() < fromBookChoiceGroup.getSelectedIndex()) {
                                            from = toBookChoiceGroup.getSelectedIndex();
                                            toBookChoiceGroup.setSelectedIndex(fromBookChoiceGroup.getSelectedIndex(), true);
                                            fromBookChoiceGroup.setSelectedIndex(from, true);
                                        }
                                        
                                        from = fromBookChoiceGroup.getSelectedIndex();
                                        to = toBookChoiceGroup.getSelectedIndex();
                                        goBible.lastFromBook = fromBookChoiceGroup.getSelectedIndex();
                                        goBible.lastToBook = toBookChoiceGroup.getSelectedIndex();
                                        
                                    }


                                    goBible.display.setCurrent(new SearchingCanvas(goBible, from, to, goBible.lastSearchString));
                            }
                            break;
                    }

                    case Command.BACK:
                    case Command.CANCEL:
                    {
                            goBible.showMainScreen();
                            break;
                    }
            }
    }

    public void itemStateChanged(Item i) {
        if (i == bookChoiceGroup) {
            if (bookChoiceGroup.getSelectedIndex() == bookChoiceGroup.size() - 1) { // the Define Range... choice
                this.append(fromBookChoiceGroup);
                this.append(toBookChoiceGroup);
            }
            else {
                while (this.size() > 2) {
                    this.delete(this.size() - 1);
                }
            }
        }
        else if (i == fromBookChoiceGroup) {
            if (toBookChoiceGroup.getSelectedIndex() < fromBookChoiceGroup.getSelectedIndex()) {
                toBookChoiceGroup.setSelectedIndex(fromBookChoiceGroup.getSelectedIndex(), true);
            }
        }
        else if (i == toBookChoiceGroup) {
            if (toBookChoiceGroup.getSelectedIndex() < fromBookChoiceGroup.getSelectedIndex()) {
                fromBookChoiceGroup.setSelectedIndex(toBookChoiceGroup.getSelectedIndex(), true);
            }
        }
    }
}
