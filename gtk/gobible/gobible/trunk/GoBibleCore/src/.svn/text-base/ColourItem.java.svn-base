//
//  ColourItem.java
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

/**
 * Essentially a StringItem that allows the text and background colours to be set.
 */
public class ColourItem extends CustomItem
{
	String text;
	int textColour;
	int backColour;
	Font font;
	int fontHeight;
	int stringWidth;
	int id;

	/**
	 * @param id a generic id that can be used for tracking.
	 */
	public ColourItem(int id, String text, int textColour, int backColour)
	{
		super("");
		this.id = id;
		this.textColour = textColour;
		this.backColour = backColour;
		font = Font.getFont(Font.FACE_SYSTEM, Font.STYLE_PLAIN, Font.SIZE_MEDIUM);
		setText(text);
	}
	
	public void setText(String text)
	{
		this.text = text;
		fontHeight = font.getHeight();
		stringWidth = font.stringWidth(text);		
	}
	
	public void paint(Graphics g, int width, int height)
	{
		// Fill the background
		g.setColor(backColour);
		
		g.fillRect(0, 0, width, height);
		
		// Set the font
		g.setFont(font);
		
		g.setColor(textColour);
		
		// Draw the string
		g.drawString(text, (width - stringWidth) / 2, (height - fontHeight) / 2, Graphics.LEFT | Graphics.TOP);
	}

	public int getMinContentHeight()
	{
		return fontHeight + 2;
	}
	
	public int getPrefContentHeight(int width)
	{
		return fontHeight * 2;
	}
	
	public int getMinContentWidth()
	{
		return stringWidth + 2;
	}
	
	public int getPrefContentWidth(int height)
	{
		return 128;
	}

}




