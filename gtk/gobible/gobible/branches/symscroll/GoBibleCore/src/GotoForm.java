//
//  GotoForm.java
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

public class GotoForm extends Form implements CommandListener
{
	private GoBible goBible;

        private ChoiceGroup bookField;
        private TextField chapterField;
        private TextField verseField;
	
	private Command gotoCommand = new Command(GoBible.getString("UI-Goto"), Command.OK, 0);
	private Command cancelCommand = new Command(GoBible.getString("UI-Cancel"), Command.CANCEL, 0);
	
	/**
	 * Creates a search for with search criteria and from and to books to search in.
	 */
	public GotoForm(GoBible goBible)
	{
		super(GoBible.getString("UI-Goto"));

		this.goBible = goBible;
                
                bookField = new ChoiceGroup(GoBible.getString("UI-Book") + ":", Choice.POPUP);
                String[] bookNames = goBible.bibleSource.getBookNames();
                for (int i=0; i < bookNames.length; i++) {
                    bookField.append(bookNames[i], null);
                }
                
                chapterField = new TextField(GoBible.getString("UI-Chapter"), "", 3, TextField.NUMERIC);
                verseField = new TextField(GoBible.getString("UI-Verse"), "", 3, TextField.NUMERIC);


                this.append(bookField);
                this.append(chapterField);
                this.append(verseField);

		addCommand(gotoCommand);
		addCommand(cancelCommand);
		
		setCommandListener(this);
	}
        protected void gotoPassage(int bk, int ch, int ve) {
            bookField.setSelectedIndex(bk, true);
            chapterField.setString("" + (ch + 1));
            verseField.setString("" + (ve + 1));
        }
	
	public void commandAction(Command command, Displayable display)
	{
		switch (command.getCommandType())
		{
			case Command.OK:
			{
                            int ch = 0;
                            int ve = 0;
                            try {
                                ch = Integer.parseInt(chapterField.getString());
                            }
                            catch (NumberFormatException nfe) {}
                            try {
                                ve = Integer.parseInt(verseField.getString());
                            }catch (NumberFormatException nfe) {}
                                goBible.bibleCanvas.requestPassage(bookField.getSelectedIndex(), ch - 1, ve - 1, true);
                                goBible.showMainScreen();
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
}
