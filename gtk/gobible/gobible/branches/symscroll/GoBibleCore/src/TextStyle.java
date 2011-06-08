import javax.microedition.lcdui.*;

/**
 * Class to store and manage the text styles. The properties are booleans
 * stored in an 8-bit (byte) value, so there are a maximum of 8 properties.
 *
 * After toggling the property, obtain the new font using getFont() and
 * update the Graphics by calling updateGraphis(g) (to set the colour).
 *
 * This class also has several stubs for functions that are meant to
 * be used in place of functions in Graphics and Font. This class will
 * then handle all drawing requests according to the properties set.
 * The reason for this is to support text styles that are not support by the
 * host platform.
 *
 * For example,
 * 1.   Faux bold, by drawing twice, shifted horizontally by 1 pixel
 * 2.   Faux underline, by drawing a 1px-wide solid line below the text baseline
 * 3.   Faux dotted-underline. Similar to the faux underline, but with a dotted
 *      line instead.
 *
 * @author Sim
 */
public class TextStyle {
    private short property = 0;

    private short lastProp = 0;
    private Font lastFont = null;

    public final static char STYLE_RED = 1;
    public final static char STYLE_ADD = 2;
    public final static char STYLE_ND = 3;
    public final static char STYLE_QUOTE = 4;
    public final static char STYLE_PROPERNOUN = 5;
    public final static char STYLE_CHAPTER_TITLE = 6;
    public final static char STYLE_SECTION_TITLE = 7;
    public final static char STYLE_BOLD = 8;

    public static boolean fauxBold = false;
    public static boolean fauxUnderlined = false;
    public static boolean fauxItalic = false;

    public static int christWordsColour = 0xD00000;
    public static int textColour = 0x000000;
    public static int fauxItalicColour = 0x666666;
    public static int fontHeight;
    public static boolean reverseCharacters = false;
    public static boolean redLetter = true;

    public static int fontSize = Font.SIZE_MEDIUM;
    public static boolean defaultFontBold = false;

    private Graphics graphics;
    
    /**
     * @param g
     */
    public void setGraphics(Graphics g) {
        this.graphics = g;
    }
    /**
     * 
     */
    public Graphics getGraphics() {
        return graphics;
    }
    /**
     * Possibly add 1px for faux underlines
     */
    public int getHeight() {
        return ((int) GoBible.lineHeight * getFont().getHeight());
    }

    /**
     * Stub
     * @param g
     */
    public int stringWidth(String str) {
        return getFont().stringWidth(
                reverseCharacters ? GBCToolkit.reverseCharacters(str) : str)
                + ((fauxBold && isBold())?1:0);
    }
        /**
         *
         * @param ch
         * @param offset
         * @param length
         * @return
         */
        public int charsWidth(char[] ch, int offset, int length) {
            return stringWidth( new String(ch, offset, length) );
        }
        /**
         *
         * @param ch
         * @return
         */
        public int charWidth(char ch) {
            return stringWidth( new String( new char[] {ch}, 0, 1) );
        }
        /**
         * Stub
         * @param g
         */
        public int substringWidth(String str, int offset, int len) {
            // note difference in specification
            // String.substring(startIndex, endIndex);
            return stringWidth(str.substring(offset, len + offset));
        }
    /**
     * 
     * @param str
     * @param x
     * @param y
     * @param anchor
     */
    public void drawString(String str, int x, int y, int anchor) {
        if (TextStyle.reverseCharacters) {
            str = GBCToolkit.reverseCharacters(str);
        }
        updateGraphics(graphics);
        graphics.setFont(getFont());
        graphics.drawString(str, x, y, anchor);
        if (fauxBold && isBold()) {
            graphics.drawString(str, x + 1, y, anchor);
        }
        if (fauxItalic && isItalic() && (anchor & Graphics.TOP) != 0) {
            graphics.setStrokeStyle(Graphics.DOTTED);
            /* TODO: Optimize this code by saving the value of stringWidth() */
            graphics.drawLine(
                    x,
                    y + getFont().getBaselinePosition(),
                    x + stringWidth(str) * (((anchor & Graphics.RIGHT) != 0)? -1 : 1),
                    y + getFont().getBaselinePosition()
                    );
        }
    }
        /**
         *
         * @param str
         * @param offset
         * @param len
         * @param x
         * @param y
         * @param anchor
         */
        public  void drawSubstring(String str, int offset, int len, int x, int y, int anchor) {
            drawString(str.substring(offset, offset+len), x, y, anchor);
        }
        /**
         *
         * @param character
         * @param x
         * @param y
         * @param anchor
         */
        public void drawChar(char character, int x, int y, int anchor) {
            drawString(new String(new char[]{character}, 0, 1), x, y, anchor);
        }
        /**
         *
         * @param data
         * @param offset
         * @param length
         * @param x
         * @param y
         * @param anchor
         */
        public void drawChars(char[] data, int offset, int length, int x, int y, int anchor) {
            drawString( new String(data,offset, length), x, y, anchor);
        }

    /**
     * Resets the text style to default settings. Also resets the lastProp
     * so that fonts may be reloaded (e.g. after size change)
     */
    public void reset() {
        lastProp = property = 0;
    }
    /**
     * Toggles the property.
     *
     * @param prop 0 <= prop < 8, to manage any one of 8 possible properties.
     */
    public void toggleProperty(int prop) {
        if (0 < prop && prop <= 8) {
            this.property ^= 1 << (prop - 1);
        }
    }
    /**
     * Sets a property to a value.
     *
     * @param prop 0 <= prop < 8, to manage any one of 8 possible properties.
     */
    public void setProperty(int prop, boolean value) {
        if (0 < prop && prop <= 8) {
            byte mask = (byte) ( 1 << (prop - 1));

            if (value) {
                this.property |= mask;
            }
            else {
                this.property &= ~mask;
            }
        }
    }
    /**
     * Tests whether a property is active
     *
     * @param prop
     * @return true if the property is active, false otherwise.
     */
    public boolean isActive(int prop) {
        if (0 < prop && prop <= 8) {
            return (this.property & (1 << (prop - 1)) ) > 0;
        }
        return false;
    }
    /**
     * Returns the font to be used for the current text style
     *
     * @return
     */
    public Font getFont() {
        /* // doesn't work if font size has changed
        if (lastProp == property && lastFont != null) {
            return lastFont;
        } */

        boolean italic = false, bold = false, underlined = false;

        italic = isItalic();
        bold = isBold();
        underlined = isUnderlined();

        lastProp = property;
        lastFont = Font.getFont(
                Font.FACE_SYSTEM,
                (
                    ( (italic && !fauxItalic)?Font.STYLE_ITALIC:0) |
                    ( (bold && !fauxBold) ?Font.STYLE_BOLD:0) |
                    ( (underlined && !fauxUnderlined)?Font.STYLE_UNDERLINED:0)
                ),
                TextStyle.fontSize);
        return lastFont;
    }
    public boolean isBold() {
        return defaultFontBold || isActive(STYLE_SECTION_TITLE) || isActive(STYLE_CHAPTER_TITLE) || isActive(STYLE_BOLD);
    }
    public boolean isItalic() {
        return isActive(STYLE_ADD);
    }
    public boolean isUnderlined() {
        return isActive(STYLE_PROPERNOUN);
    }
    public boolean isDottedUnderline() {
        return isActive(STYLE_ADD);
    }
    public boolean isSmallCaps() {
        return isActive(STYLE_ND);
    }
    /**
     * Updates the Graphics according to the current text style.
     *
     * @param g
     */
    public void updateGraphics(Graphics g) {
        if ( TextStyle.redLetter && isActive(STYLE_RED) ) {
            g.setColor(christWordsColour);
        }
        else if (isItalic() && fauxItalic) {
            /* We don't have 2D transformation capabilities, so we paint the text in
             * a lighter colour instead.
             */
            g.setColor(fauxItalicColour);
        }
        else {
            g.setColor(textColour);
        }
    }
    /**
     * Creates a copy of this text style
     */
    public void cloneInto(TextStyle ts) {
        ts.property = this.property;
    }
    public TextStyle clone() {
        TextStyle ts = new TextStyle();
        ts.property = this.property;

        return ts;
    }

    public short getStyleValue() {
        return property;
    }
    public void setStyleValue(short s) {
        property = s;
    }
}
