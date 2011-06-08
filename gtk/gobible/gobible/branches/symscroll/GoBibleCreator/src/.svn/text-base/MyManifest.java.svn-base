/* JarUtils.java -- Utility methods for reading/writing Manifest[-like] files
   Copyright (C) 2006 Free Software Foundation, Inc.

This file is part of GNU Classpath.

GNU Classpath is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Classpath is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Classpath; see the file COPYING.  If not, write to the
Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301 USA.

Linking this library statically or dynamically with other modules is
making a combined work based on this library.  Thus, the terms and
conditions of the GNU General Public License cover the whole
combination.

As a special exception, the copyright holders of this library give you
permission to link this library with independent modules to produce an
executable, regardless of the license terms of these independent
modules, and to copy and distribute the resulting executable under
terms of your choice, provided that you also meet, for each linked
independent module, the terms and conditions of the license of that
module.  An independent module is a module which is not derived from
or based on this library.  If you modify this library, you may extend
this exception to your version of the library, but you are not
obligated to do so.  If you do not wish to do so, delete this
exception statement from your version. */


import java.util.jar.*;
import java.util.*;
import java.io.*;
public class MyManifest extends Manifest {
	/* WARNING: This is a very dumb class. It does not conform to any spec except the 72-byte spec
		and makes provisions to avoid breaking UTF sequences by stupidly breaking 4 bytes before limit. */
	/* Some code copied from GNU Classpath (LGPL with exception) */
	
	private static final byte CRLF [] = { '\r', '\n' };
	
	public MyManifest() {
	
	}
	public MyManifest(InputStream is) throws IOException {
		super(is);
	}
	
    public void write(OutputStream out) throws IOException {
		writeMFManifest(getMainAttributes(), getEntries(), out);
    }
	
	private static void writeAttributeEntry(Map.Entry entry, OutputStream out)
	throws IOException
	{
		String name = entry.getKey().toString();
		String value = entry.getValue().toString();
		
		if (name.equalsIgnoreCase("Name"))
			throw new JarException("Attributes cannot be called 'Name'");

		if (name.startsWith("From"))
			throw new JarException("Header cannot start with the four letters 'From'"
			+ name);

		writeHeader(name, value, out);
	}
  
	private static void writeHeader(String name, String value, OutputStream out)
	throws IOException
	{
		String target = name + ": ";
		byte[] b = target.getBytes("UTF-8");
		if (b.length > 72)
			throw new IOException("Attribute's name already longer than 70 bytes");

		if (b.length == 72)
		{
			out.write(b);
			out.write(CRLF);
			target = " " + value;
		}
		else
			target = target + value;

		int n;
		while (true)
		{
			b = target.getBytes("UTF-8");
			
			if (b.length < 73)
			{
				out.write(b);
				break;
			}

			// find an appropriate character position to break on
			n = 72;
			if (n > target.length()) {
				n = target.length();
			}
			while (true)
			{
				b = target.substring(0, n).getBytes("UTF-8");
				if (b.length < 73)
					break;

				n--;
				if (n < 1)
					throw new IOException("Header is unbreakable and longer than 72 bytes");
			}

			out.write(b);
			out.write(CRLF);
			target = " " + target.substring(n);
		}

		out.write(CRLF);
	}
	public static void
		writeMFManifest(Attributes attr, Map entries, OutputStream stream)
		throws IOException
	{
		BufferedOutputStream out = stream instanceof BufferedOutputStream
						   ? (BufferedOutputStream) stream
						   : new BufferedOutputStream(stream, 4096);

		// version info first
		//writeVersionInfo(attr, out);
		out.write("Manifest-Version: 1.0\r\n".getBytes("UTF-8"));
		
		// then the rest
		for (Map.Entry<Object, Object> e: attr.entrySet())
		{
			// Don't print the manifest version again
			if (!"Manifest-Version".equals(e.getKey()))
				writeAttributeEntry(e, out);
		}
		out.write(CRLF);

		// then the ordinary values
		for (Map.Entry e: (Set<Map.Entry>)entries.entrySet())
		{
			writeHeader("Name", e.getKey().toString(), out);
			Attributes eAttr = (Attributes) e.getValue();
			for (Map.Entry<Object, Object> e2: eAttr.entrySet())
			{
				writeAttributeEntry(e2, out);
			}
			out.write(CRLF);
		}

		out.flush();
	}

}
