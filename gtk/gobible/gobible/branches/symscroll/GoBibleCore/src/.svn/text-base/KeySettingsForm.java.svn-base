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

public class KeySettingsForm extends Form implements CommandListener, ItemCommandListener
{
    // Program takes reference from here.
    public static final int ACTION_PREV_SCREEN = 0;
    public static final int ACTION_NEXT_SCREEN = 1;
    public static final int ACTION_PREV_CHAPTER = 2;
    public static final int ACTION_NEXT_CHAPTER = 3;
    public static final int ACTION_PREV_BOOK = 4;
    public static final int ACTION_NEXT_BOOK = 5;
    public static final int ACTION_PREV_VERSE = 6;
    public static final int ACTION_NEXT_VERSE = 7;
    public static final int ACTION_PREV_3_LINES = 8;
    public static final int ACTION_NEXT_3_LINES = 9;
    public static final int ACTION_PREV_1_LINE = 10;
    public static final int ACTION_NEXT_1_LINE = 11;
    //public static final int ACTION_PREV_SEARCH_RESULT = 12;
    public static final int ACTION_NEXT_SEARCH_RESULT = 12; //13;
    public static final int ACTION_GOTO = 13; // 14;
    
    public static final String[] ACTIONS = {
        "UI-Action-Previous-Screen",
        "UI-Action-Next-Screen",
        "UI-Action-Previous-Chapter",
        "UI-Action-Next-Chapter",
        "UI-Action-Previous-Book",
        "UI-Action-Next-Book",
        "UI-Action-Previous-Verse",
        "UI-Action-Next-Verse",
        "UI-Action-Previous-3-Lines",
        "UI-Action-Next-3-Lines",
        "UI-Action-Previous-1-Line",
        "UI-Action-Next-1-Line",
        /*"Search Before",*/
        "UI-Search-Next",
        "UI-Goto"
    };
    private KeySelectionItem keySettings[];

    private GoBible goBible;

    private ColourItem themeItem;

    private Command saveCommand = new Command(GoBible.getString("UI-Save"), Command.OK, 0);
    private Command assignCommand = new Command(GoBible.getString("UI-Assign-Key"), Command.ITEM, 0);
    private Command unassignCommand = new Command(GoBible.getString("UI-Unassign-Key"), Command.ITEM, 0);
    private Command cancelCommand = new Command(GoBible.getString("UI-Cancel"), Command.CANCEL, 0);

    // Theme Form
    Form themeForm;

    /**
     * Setup the Preferences form.
     */
    public KeySettingsForm(GoBible goBible)
    {
        super(GoBible.getString("UI-Preferences"));
        this.goBible = goBible;

        keySettings = new KeySelectionItem[ACTIONS.length];
        
        for (int i=0; i < ACTIONS.length ; i++) {
            keySettings[i] = new KeySelectionItem(goBible.getString(ACTIONS[i]), this);
            this.append(keySettings[i]);

            if (goBible.keySettings.length <= i) {
                keySettings[i].setKey(0);
            }
            else {
                keySettings[i].setKey(goBible.keySettings[i]);
            }
            keySettings[i].addCommand(assignCommand);
            keySettings[i].addCommand(unassignCommand);
            keySettings[i].setItemCommandListener(this);
        }

        // Setup the commands and listeners for the themes form
        addCommand(saveCommand);
        addCommand(cancelCommand);
        setCommandListener(this);
    }

    /**
     * Part of ItemCommandListener
     */
    public void commandAction(Command command, Item item) {
        if (command == assignCommand) {
            if (item instanceof KeySelectionItem) {
                ((KeySelectionItem)item).enterReceiveMode();
            }
        }
        else if (command == unassignCommand) {
            ((KeySelectionItem)item).setKey(0);
        }
        else if (command == cancelCommand) {
            if (item instanceof KeySelectionItem) {
                ((KeySelectionItem)item).exitReceiveMode();
            }
        }
    }

    /**
     * Part of CommandListener.
     */
    public void commandAction(Command command, Displayable display)
    {
        switch (command.getCommandType())
        {
            case Command.OK:
            {
                // save values
                if (goBible.keySettings.length != ACTIONS.length) {
                    goBible.keySettings = new int[goBible.keySettings.length];
                }
                for (int i=0; i < ACTIONS.length ; i++) {
                    goBible.keySettings[i] = keySettings[i].getKey();
                }

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

    void notifyKeyChanged(KeySelectionItem ksi) {
        if (ksi.getKey() == 0) {
            return;
        }

        for (int i = 0; i < keySettings.length; i++) {
            if (keySettings[i] == ksi || keySettings[i] == null) {
                continue;
            }
            if (keySettings[i].getKey() == ksi.getKey()) {
                keySettings[i].setKey(0);
            }
        }
    }
}

class KeySelectionItem extends CustomItem {
    private String description;
    private Font font;
    private boolean receiveMode = false;
    private String prompt = "Press a key to assign";
    private KeySettingsForm form;

    private int currentKey = 0;

    public static String getKeyString(int key) {
        switch (key) {
            case Canvas.DOWN:
                return "<\u2193>";
            case Canvas.UP:
                return "<\u2191>";
            case Canvas.LEFT:
                return "<\u2190>";
            case Canvas.RIGHT:
                return "<\u2192>";
            case Canvas.KEY_POUND:
                return "<#>";
            case Canvas.KEY_STAR:
                return "<*>";
            case 0:
                return "";
            default:
                if (key >= Canvas.GAME_A && key <= Canvas.GAME_D) {
                    return "<" + new Character( (char) ('A' + key - Canvas.GAME_A) ).toString() + ">";
                }
                else if (key >= Canvas.KEY_NUM0 && key <= Canvas.KEY_NUM9) {
                    return "<" + new Character( (char) ('0' + key - Canvas.KEY_NUM0) ).toString() + ">";
                }
                else {
                    return "[" + key + ":" + (char)key + "]";
                }
        }
       
    }
    
    public KeySelectionItem(String desc, KeySettingsForm form) {
        super (desc);

        this.form = form;
        this.description = desc;
        this.font = Font.getFont(Font.FONT_INPUT_TEXT);
        this.setLabel(desc);
        this.setLayout(Item.LAYOUT_EXPAND | Item.LAYOUT_NEWLINE_AFTER);
    }
    public void paint(Graphics g, int width, int height) {
        String keyString = getKeyString(currentKey);

        g.setColor(255,255,255);
        g.fillRect(0,0, width, height);

        g.setColor(0,0,0);
        if (receiveMode) {
            g.drawString(prompt, 0, 0, Graphics.TOP | Graphics.LEFT);
        }
        else {
            g.drawString(keyString, 0, 0, Graphics.TOP | Graphics.LEFT);
        }
    }
    public int getPrefContentWidth(int height) {
        return font.stringWidth(prompt);
    }
    public int getPrefContentHeight(int width) {
        return font.getHeight();
    }
    public int getMinContentWidth() {
        return font.stringWidth("CANCEL KEY");
    }
    public int getMinContentHeight() {
        return font.getHeight();
    }
    public void setKey(int i) {
        currentKey = i;

        form.notifyKeyChanged(this);
        repaint();
    }
    public void enterReceiveMode() {
        receiveMode = true;
        repaint();
    }
    public void exitReceiveMode() {
        receiveMode = false;
        repaint();
    }
    public int getKey() {return currentKey;}
    protected void keyPressed(int key) {
        if (receiveMode) {
            switch (key) {
                case Canvas.LEFT:
                case Canvas.RIGHT:
                case Canvas.UP:
                case Canvas.DOWN:
                case Canvas.FIRE:
                    break;
                default:
                    currentKey = key;
                    form.notifyKeyChanged(this);
            }
            receiveMode = false;
        }
        repaint();
    }
}

class ActionSelectionItem extends ChoiceGroup {
    public ActionSelectionItem(String label) {
        super(label, POPUP);
        for (int i = 0; i < KeySettingsForm.ACTIONS.length; i++) {
            this.append(KeySettingsForm.ACTIONS[i], null);
        }
    }
}