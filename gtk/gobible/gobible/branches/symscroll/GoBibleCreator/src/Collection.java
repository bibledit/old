import java.util.*;

/**
 * Represents a Go Bible collection to be created.
 **/
public class Collection
{
	/** Name of the collection. **/
	String name;

	/** File name of the collection. Same as name but with spaces removed. **/
	String fileName;

	/** Books in the collection of type Book. **/
	Vector books = new Vector();

	/**
	 * Creates a collection object.
	 * @param name name of the collection.
	 **/
	public Collection(String fileName, String midletName)
	{
		this.name = midletName;

		// Remove spaces to form file name
		char[] nameChars = fileName.toCharArray();
		char[] fileNameChars = new char[nameChars.length];

		int j = 0;

		for (int i = 0; i < nameChars.length; i++)
		{
			if (nameChars[i] != ' ')
			{
				fileNameChars[j] = nameChars[i];
				j++;
			}
		}

		this.fileName = new String(fileNameChars, 0, j);
	}

	/**
	 * Adds a book to the collection.
	 * @param book book to add to the collection.
	 **/
	public void add(Book book)
	{
		books.add(book);
	}
}
