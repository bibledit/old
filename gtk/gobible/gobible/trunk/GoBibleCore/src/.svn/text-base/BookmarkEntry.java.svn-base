//
//  BookmarkEntry.java
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

import java.io.*;

public class BookmarkEntry 
{
	int bookIndex, chapterIndex, verseIndex;
	String excerpt;
	
	/**
	 * Create a new bookmark.
	 */
	public BookmarkEntry(int bookIndex, int chapterIndex, int verseIndex, String excerpt)
	{
		this.bookIndex = bookIndex;
		this.chapterIndex = chapterIndex;
		this.verseIndex = verseIndex;
		this.excerpt = excerpt;
	}
	
	/**
	 *  Read in the bookmark from the record store.
	 */
	public BookmarkEntry(DataInputStream input) throws IOException
	{
		bookIndex = input.read();
		chapterIndex = input.read();
		verseIndex = input.read();
		excerpt = input.readUTF();
	}
	
	/**
	 * Write the bookmark to the record store.
	 */
	public void write(DataOutputStream output) throws IOException
	{
		output.writeByte(bookIndex);
		output.writeByte(chapterIndex);
		output.writeByte(verseIndex);
		output.writeUTF(excerpt);
	}
}
