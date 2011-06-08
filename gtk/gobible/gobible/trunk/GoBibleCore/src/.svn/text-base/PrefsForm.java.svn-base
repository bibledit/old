//
//  PrefsForm.java
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

public class PrefsForm extends Form implements CommandListener, ItemCommandListener
{
	private GoBible goBible;
	private ChoiceGroup fontSizeChoice;
	private ChoiceGroup fontStyleChoice;
	private ChoiceGroup fullScreenChoice;
	private ChoiceGroup themeChoice;
	private ChoiceGroup reverseColoursChoice;
	private ChoiceGroup reverseCharactersChoice;
	
	private ColourItem[] themeItems = new ColourItem[]
	{
		new ColourItem(0, GoBible.getString("UI-Theme-Paper"), GoBible.THEME_TEXT_COLOUR[0], GoBible.THEME_BACK_COLOUR[0]),
		new ColourItem(1, GoBible.getString("UI-Theme-Computer"), GoBible.THEME_TEXT_COLOUR[1], GoBible.THEME_BACK_COLOUR[1]),
		new ColourItem(2, GoBible.getString("UI-Theme-Floral"), GoBible.THEME_TEXT_COLOUR[2], GoBible.THEME_BACK_COLOUR[2]),
		new ColourItem(3, GoBible.getString("UI-Theme-Natural"), GoBible.THEME_TEXT_COLOUR[3], GoBible.THEME_BACK_COLOUR[3]),
		new ColourItem(4, GoBible.getString("UI-Theme-Blue"), GoBible.THEME_TEXT_COLOUR[4], GoBible.THEME_BACK_COLOUR[4]),
		new ColourItem(5, GoBible.getString("UI-Theme-Sunshine"), GoBible.THEME_TEXT_COLOUR[5], GoBible.THEME_BACK_COLOUR[5]),
	};
	
	private ColourItem themeItem;

	private Command saveCommand = new Command(GoBible.getString("UI-Save"), Command.OK, 0);
	private Command cancelCommand = new Command(GoBible.getString("UI-Cancel"), Command.CANCEL, 0);
	private Command selectCommand = new Command(GoBible.getString("UI-Select"), Command.ITEM, 0);
	
	// Theme Form
	Form themeForm;
	
	/**
	 * Setup the Preferences form. 
	 */
	public PrefsForm(GoBible goBible)
	{
		super(GoBible.getString("UI-Preferences"));
		
		this.goBible = goBible;

		append(new StringItem(GoBible.getString("UI-Theme") + ":", null));

		ColourItem storedThemeItem = themeItems[goBible.theme];

		themeItem = new ColourItem(storedThemeItem.id, storedThemeItem.text, storedThemeItem.textColour, storedThemeItem.backColour);
		
		themeItem.addCommand(new Command(GoBible.getString("UI-Change"), Command.ITEM, 0));
		
		themeItem.setItemCommandListener(this);
		
		append(themeItem);

		// Setup the commands and listeners for the themes form
		for (int i = 0; i < themeItems.length; i++)
		{
			themeItems[i].addCommand(selectCommand);
			themeItems[i].setItemCommandListener(this);
		}
		
		fontSizeChoice = new ChoiceGroup(GoBible.getString("UI-Font-Size") + ":", Choice.EXCLUSIVE, new String[]{GoBible.getString("UI-Small"), GoBible.getString("UI-Medium"), GoBible.getString("UI-Large")}, null);
		fontSizeChoice.setFont(0, Font.getFont(Font.FACE_SYSTEM, Font.STYLE_PLAIN, Font.SIZE_SMALL));
		fontSizeChoice.setFont(1, Font.getFont(Font.FACE_SYSTEM, Font.STYLE_PLAIN, Font.SIZE_MEDIUM));
		fontSizeChoice.setFont(2, Font.getFont(Font.FACE_SYSTEM, Font.STYLE_PLAIN, Font.SIZE_LARGE));
		fontSizeChoice.setSelectedIndex(goBible.fontSize, true);
		append(fontSizeChoice);

		fontStyleChoice = new ChoiceGroup(GoBible.getString("UI-Font-Style") + ":", Choice.EXCLUSIVE, new String[]{GoBible.getString("UI-Plain"), GoBible.getString("UI-Bold")}, null);
		fontStyleChoice.setFont(0, Font.getFont(Font.FACE_SYSTEM, Font.STYLE_PLAIN, Font.SIZE_MEDIUM));
		fontStyleChoice.setFont(1, Font.getFont(Font.FACE_SYSTEM, Font.STYLE_BOLD, Font.SIZE_MEDIUM));
		fontStyleChoice.setSelectedIndex(goBible.fontStyle, true);
		append(fontStyleChoice);
		
		if (GoBible.USE_MIDP20)
		{
			fullScreenChoice = new ChoiceGroup(GoBible.getString("UI-Full-Screen") + ":", Choice.EXCLUSIVE, new String[]{GoBible.getString("UI-On"), GoBible.getString("UI-Off")}, null);
			fullScreenChoice.setSelectedIndex(goBible.fullScreen ? 0 : 1, true);
			append(fullScreenChoice);
		}

		reverseColoursChoice = new ChoiceGroup(GoBible.getString("UI-Reverse-Colours") + ":", Choice.EXCLUSIVE, new String[]{GoBible.getString("UI-On"), GoBible.getString("UI-Off")}, null);
		reverseColoursChoice.setSelectedIndex(goBible.reverseColours ? 0 : 1, true);		
		append(reverseColoursChoice);

		reverseCharactersChoice = new ChoiceGroup(GoBible.getString("UI-Reverse-Characters") + ":", Choice.EXCLUSIVE, new String[]{GoBible.getString("UI-On"), GoBible.getString("UI-Off")}, null);
		reverseCharactersChoice.setSelectedIndex(goBible.bibleCanvas.reverseCharacters ? 0 : 1, true);		
		append(reverseCharactersChoice);
		
		addCommand(saveCommand);
		addCommand(cancelCommand);
		setCommandListener(this);
	}
	
	/**
	 * Part of CommandListener.
	 */
	public void commandAction(Command command, Displayable display)
	{
		// If the prefs form is being displayed then process accordingly
		// otherwise the commands are coming from the themes form
		if (display == this)
		{
			switch (command.getCommandType())
			{
				case Command.OK:
				{
					// Save has been pressed so save the prefs
					goBible.fontSize = fontSizeChoice.getSelectedIndex();
					goBible.fontStyle = fontStyleChoice.getSelectedIndex();

					if (GoBible.USE_MIDP20)
					{
						goBible.fullScreen = (fullScreenChoice.getSelectedIndex() == 0);
					}
					
					goBible.theme = themeItem.id;

					goBible.reverseColours = (reverseColoursChoice.getSelectedIndex() == 0);
					
					goBible.bibleCanvas.reverseCharacters = (reverseCharactersChoice.getSelectedIndex() == 0);
					
					// Go back to the main screen
					goBible.showMainScreen();
					break;
				}
				
				// Go back to the main screen without saving
				case Command.BACK:
				case Command.CANCEL:
				{
					goBible.showMainScreen();
				}
			}
		}
		else if (display == themeForm)
		{
			// The command is coming from the themes form
			switch (command.getCommandType())
			{
				// Go back to the prefs screen without changing the theme
				case Command.BACK:
				case Command.CANCEL:
				{
					goBible.display.setCurrent(this);
				}
			}
			
		}
	}
	
	/**
	 * Called when the user selects the theme to change it or when
	 * a new theme has been selected in the theme form.
	 */
	public void commandAction(Command command, Item item)
	{
		if (item == themeItem)
		{
			displayThemeForm();
		}
		else if (item instanceof ColourItem)
		{
			// A new theme has been selected so update the themeItem
			ColourItem newThemeItem = (ColourItem) item;
			themeItem.id = newThemeItem.id;
			themeItem.setText(newThemeItem.text);
			themeItem.textColour = newThemeItem.textColour;
			themeItem.backColour = newThemeItem.backColour;
			themeItem.notifyStateChanged();
			
			// Display the prefs form
			goBible.display.setCurrent(this);
		}
	}
	
	/**
	 * Display a list of themes for the user to select from.
	 */
	private void displayThemeForm()
	{
		// Display the theme list
		
		// Create a new form if not already created
		// We need to reuse the form if already created because
		// Items can't be reused in a different form, an
		// IllegalStateException is thrown
		if (themeForm == null)
		{
			themeForm = new Form(GoBible.getString("UI-Themes"), themeItems);
			themeForm.addCommand(cancelCommand);
			themeForm.setCommandListener(this);
		}
		
		// Display the theme form
		goBible.display.setCurrent(themeForm);	
	}
}


