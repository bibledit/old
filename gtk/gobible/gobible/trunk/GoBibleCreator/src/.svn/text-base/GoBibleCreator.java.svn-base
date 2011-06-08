//
//  GoBibleCreator.java
//  GoBibleCreator
//
//  Created by Jolon Faichney on Sat Oct 30 2004.
//  For the glory of our Lord Jesus Christ and the furtherance of His Kingdom.
//  This file is placed into the public domain.
//
import java.awt.*;
import java.awt.font.*;
import java.awt.geom.*;
import java.awt.image.*;
import java.io.*;
import java.util.*;
import java.util.jar.*;
import javax.imageio.*;
import jolon.xml.*;
import java.util.zip.*;

/**
 * Creates Go Bible JAR files from XML Bible texts. Supports both OSIS and
 * ThML formats. See OsisConverter and ThmlConverter classes for format
 * specific information.
 * <p>
 * Usage: java -jar GoBibleCreator.jar CollectionsFilePath
 * </p>
 * <p>
 * The collections file specifies the name of the collection, the 
 * contents of the collection, and the location of the source text.
 * </p>
 * It has the following format:
 * <p>
 * <pre>
 * Source-Text: Source.thml
 * Collection: Collection Name
 * Book: Book Name, Start Chapter, End Chapter
 * Book: Book Name, Start Chapter, End Chapter
 * Collection: Collection Name
 * Book: Book Name, Start Chapter, End Chapter
 * </pre>
 * The book name is that which is contained within the div2 title tag. 
 * The start and end chapters can be omitted if the entire book is to be 
 * included.
 * An example would be:
 * <pre>
 * Source-Text: KJV.thml
 * Collection: Gospels
 * Book: Matthew
 * Book: Mark
 * Book: Luke
 * Book: John
 * Collection: Psalms I
 * Book: Psalms, 1, 41
 * Collection: Psalms II
 * Book: Psalms, 42, 72
 * </pre> 
 * </p>
 **/
public abstract class GoBibleCreator 
{
	/** 
	 * Version of Go Bible to be written to JAR and JAD files. Major version will
	 * be the MIDP version. eg. MIDP 2.0 version will be 2.x.x.
	 **/
	public final static String SUB_VERSION = "4.1";	// "3.6";

	/** Style changes are written out as flags in a single byte. **/
	public final static char STYLE_RED = 1;
	
	/** Text that will be appended to every collection name. **/
	public final static String NAME_APPENDAGE = " Go Bible";

	/** Combines multiple chapters into a single file. File size will be approximately MAX_FILE_SIZE. **/
	public final static boolean COMBINED_CHAPTERS = true;
	
	/** If COMBINED_CHAPTERS = true then this represents the approximate file size of the combined chapters. **/
	public final static int MAX_FILE_SIZE_MIDP_2 = 24 << 10;
	public final static int MAX_FILE_SIZE_MIDP_1 = 4 << 10;
	
	/** Go Bible text alignment: LEFT. **/
	public final static int ALIGN_LEFT = 0;
	
	/** Go Bible text alignment: RIGHT. **/	
	public final static int ALIGN_RIGHT = 1;
	
	/** GoBible-Align JAD property values. **/
	public final static String[] ALIGN_TEXT = {"Left", "Right"};

	/** File name of the UI properties file. **/
	public final static String UI_PROPERTIES_FILE_NAME = "ui.properties";

	/** MIDP version in the manifest and JAD. **/
	public static String midpVersion = "MIDP-2.0";

	/** MIDP version in the manifest and JAD. **/
	public static String versionString = "2." + SUB_VERSION;
	
	public static int MAX_FILE_SIZE = MAX_FILE_SIZE_MIDP_2;
	
	/**
	 * By default GoBibleCreator parses the source text and generates the verse
	 * data which is stored in the JAR files. This is a time consuming process
	 * and rarely needs to be repeated. If "-u" is specifed on the command-line
	 * then this variable is set to true which means to only update the existing
	 * JAR/JAD files instead of reparsing the source text and generating the
	 * verse data. However, it is off by default to ensure that the files are
	 * generated.
	 */
	protected static boolean updateOnly = false;
	
	/** Location of WAP site where JAD files will be placed. 
		If no 'Wap-site:' attribute is specified in the Collections file then no WAP files will be 
		produced. **/
	protected static String wapSite = null;
	
	/** URL to be used in MIDlet-Info-URL **/
	protected static String midlet_infoURL = null;

	/** value to be used in MIDlet-Vendor **/
	protected static String midlet_vendor = null;

	/** Info string. **/
	protected static String infoString = null;

	/** Custom font string. **/
	protected static String customFontString = null;
	
	/** Text alignment. **/
	protected static int align = ALIGN_LEFT;
	
	/** Language Code appended to Collection names. eg. Ar, Zh, En, etc. **/
	protected static String languageCode = "";
	public enum languageCodePositionType
	{
		prefix,
		suffix,
	};
	protected static languageCodePositionType languageCodePosition;

	/** Contains the contents of the English UI properties file and
		non-English UI strings specified in the Collections file.
        **/	
	protected static HashMap uiProperties = new HashMap();
	
	/** Identical to the keys used in the books HashMap except retains the order
	    of books from the XML file (the HashMap does not retain order).
		@see parseBooks **/
	protected static Vector<String> bookNames = new Vector<String>();
	
	/**
	 * This will be prepended to the Source-Text path in the Collections.txt file.
	 * Can be set through the -d parameter.
	 */
	protected static String baseSourceDirectory = null;

        // the extension that is used by the USFM files
        // introduced in version 2.3
        protected static String usfmSourceFileExtension = null;        
        // the file path (relative to the baseSource directory) to an alternate 
        // icon that will display on the phone
        // introduced in version 2.3
        protected static String phoneIconFilepath = null;        
        //the USFM file codepage for the source files
        // introduced in version 2.3
        protected static String fileCodepage = null;
        // alternate name for the application that will be displayed in the 
        // phone's title
        // introduced in version 2.3
        protected static String applicationName = null;
        // flag to determine if red lettering is used
        // introduced in version 2.3
	protected static boolean useRedLettering = true;
        // listing of the possible types of supported source file formats
        // introduced in version 2.3
        protected static String sTitleTag = "\\h";
        public enum SourceFormatType
        {
            osis,
            thml,
            usfm,
            unknown  
        };
        // introduced in version 2.3
        protected static SourceFormatType sourceFormatType;
        
	/** Empty-Verse-Text string. **/
	protected static String EmptyVerseString = null;
        
	/** 
	 * The starting point. 
	 * @param args The first argument is optional and specifies whether MIDP 1.0
	 * or MIDP 2.0 files are produced, 
	 * the second argument is the XML file (OSIS or ThML),
     * and the third argument is the collections file as specified above.
	 **/
    public static void main(String args[]) throws IOException
    {
        System.out.println("----------------------------------");
        System.out.println("GoBibleCreator Version: " + versionString);
        System.out.println("----------------------------------");

        if (args.length < 1)
		{
			System.out.println("Usage: java -jar CollectionsFilePath");
		}
		else 
		{
			// Turns on temporarily the flag to indicate that the next argument
			// will be a base directory for the Source-Text, activated by a "-d" argument.
			boolean baseSourceDirectoryFlag = false;
			
			for (String arg: args)
			{
				if (arg.compareToIgnoreCase("-dh") == 0)
				{
					// show some internal debug context information
					String properties[] = {"java.version", "java.vendor", "java.class.path",
											"os.name", "os.arch", "os.version",
											//"java.ext.dirs", "java.library.path",
											//"user.name", "user.home",
											"user.dir"};
					System.out.println("***************************************************");
					System.out.println("*** Listing some debug type information here:");
					try
					{
						for (int i=0; i<properties.length; i++)
						{
							System.out.println("*** "+properties[i]+" ==> " + System.getProperty(properties[i]));
						}
					}
					catch (Exception e) {}
					System.out.println("***************************************************");
					System.out.println();
				}
				else if (arg.equals("-u"))
				{
					// Update-only flag has been specified which means we won't be
					// parsing the source text or updating the text in the JAR file.
					// Instead we will be only modifying the existing JAR/JAD files
					// which should already exist
					updateOnly = true;
					System.out.println(" ** The \"Update-only\"(-u) flag has been found: updating the JAR/JAD files.");
					System.out.println(" ** The original JAR files[s] are renamed with the file extension .TMP");
				}
				else if (arg.equals("-d"))
				{
					// If -d is specified then the next argument will be a directory
					// for which all Source-Text paths will be considered relative to.
					// See the if statement below.
					baseSourceDirectoryFlag = true;
					System.out.println(" ** The \"Directory\"(-d) flag has been found.");
				}
				else if (baseSourceDirectoryFlag)
				{
					// This argument is the base directory for Source-Text, we can
					// switch the flag off now and considered the other arguments
					// as normal.
					baseSourceDirectory = arg;
					baseSourceDirectoryFlag = false;
					System.out.println(" ** Using <"+baseSourceDirectory+"> as the base Source Directory.");
				}
				// If a thml or xml file has been specified then generate a collections file
				// from it and do nothing else, otherwise we assume a collections file
				// has been specified and will generate the collections from it
				else if ((arg.toLowerCase().endsWith(".xml")
						|| arg.toLowerCase().endsWith(".thm")
						|| arg.toLowerCase().endsWith(".thml")))
				{
					// No Collections file specified so generate 
					// Collections file from XML
					File xmlFile = new File(args[0]);
					HashMap books = parseXml(xmlFile);
					
					if (books != null)
					{
						generateCollectionsFile(new File(xmlFile.getParent(), "Collections.txt"), xmlFile.getName(), books);
					}
				}
				else
				{
					create(new File(arg));
				}
			}
		}
    }

	/**
	 * Added since the method was needed for two paths through the code. This
	 * was added so the UpdateOnly path can also benifit from the relative
	 * path name in the collections file.
	 * 
	 * @param collectionsFile
	 */
	private static void UpdateBaseSourceDirectory(File collectionsFile)
	{
		// Base source directory can be overridden with the -d argument
		if (baseSourceDirectory == null)
		{
			// By default the sourceTextPath is relative to the collectionsFile
			//baseSourceDirectory = "" + collectionsFile.getParent();
			baseSourceDirectory = collectionsFile.getAbsolutePath();
			baseSourceDirectory = baseSourceDirectory.substring(0, baseSourceDirectory.length() - collectionsFile.getName().length());
		}
	}

	/**
	 * Parses the XML and collection files and writes out the Go Bible data
	 * files in the same directory as the collection files.
	 * @param collectionsFile Collections File
	 */
	public static void create(File collectionsFile) throws IOException
	{		
		HashMap books = null;
		
		// If updateOnly is true then we are only updating the existing
		// JAD/JAR files and won't be parsing the source text
		if (!updateOnly)
		{
			// Extract xml file from the collectionsFile property: Source-Text
			String sourceTextPath = extractSourceTextPath(collectionsFile);

            // Extract the source file type from the collectionsFile property: SourceFormat
            sourceFormatType = extractSourceFormatType(collectionsFile);
            if (sourceFormatType.equals(SourceFormatType.unknown))
            {
                //do extra testing on the file to see if the format can be determined
                //this is for backward compatibility for GBC 2.2.6 version and prior
                //which did not specify the Source-Format property
                sourceFormatType = SourceFormatTypeRetry(baseSourceDirectory, sourceTextPath);
            }

			// Base source directory can be overridden with the -d argument
			UpdateBaseSourceDirectory(collectionsFile);
			/*if (baseSourceDirectory == null)
			{
				// By default the sourceTextPath is relative to the collectionsFile
				//baseSourceDirectory = "" + collectionsFile.getParent();
                baseSourceDirectory = collectionsFile.getAbsolutePath();
                baseSourceDirectory = baseSourceDirectory.substring(0, baseSourceDirectory.length() - collectionsFile.getName().length());
			}*/
			if (sourceFormatType == SourceFormatType.osis
                || sourceFormatType == SourceFormatType.thml)
            {
                File xmlFile = new File(baseSourceDirectory, sourceTextPath);
                books = parseXml(xmlFile);
            }
            else if (sourceFormatType == SourceFormatType.usfm)
            {
                extractCommonFields(collectionsFile);
                //make sure that the file extension is retrieved
                extractUSFMfields(collectionsFile);
                books = parseUSFM(baseSourceDirectory, sourceTextPath, sTitleTag);
            }
            else
            {
                //dump out message saying that it cannot determine the file format type
                System.out.println("Error: Could not determine Bible format type, " + sourceFormatType + ".  Please use the 'Source-Format'");
                System.out.println("property in your collections file.");
                return;
            }
		}
		else
		{
			// Allow the relative path in the collections file to work for updateing
			// the icon as well.
			UpdateBaseSourceDirectory(collectionsFile);
		}
		
		if (books != null || updateOnly)
		{
			parseUiProperties();
			
			// Parse the collections file
			Vector collections = parseCollectionsFile(collectionsFile, books);
			
			if (customFontString != null)
			{
				generateCustomFont(customFontString, collectionsFile, books);
			}
			
			// Open GoBibleCore based on MIDP version 
			String goBibleJarFileName = "GoBibleCore/GoBibleCore2.jar";
			
			// Set some fields based on the MIDP version
			// Version 2.4.0 has removed MIDP-1.0 as an option
//			if (midpVersion.equals("MIDP-1.0"))
//			{
//				versionString = "1." + SUB_VERSION;
//				goBibleJarFileName = "GoBibleCore/GoBibleCore1.jar";
//			}
			
			// Work out the current directory of the GoBibleCreator.jar
			File jarDirectory = getJarDirectory();
			File jarFile = new File(jarDirectory, goBibleJarFileName);
			
			JarFile goBibleJar = new JarFile(jarFile);
			
			// Write out the Go Bible data files into the same directory as the collections file
			writeCollections(collectionsFile.getParentFile(), collections, books, goBibleJar);
		}
	}
	
	/**
	 * @return The directory containing the GoBibleCreator.jar file that is being executed.
	 */
	public static File getJarDirectory()
	{
		String classPath = System.getProperty("java.class.path");
		String pathSeparator = System.getProperty("path.separator");
		
		String[] paths = classPath.split(pathSeparator);
		
		// Find the path which contains GoBibleCreator.jar
		for (String path: paths)
		{
			if (path.contains("GoBibleCreator.jar"))
			{
				File file = new File(path);
				return file.getParentFile();
			}
		}
		
		System.out.println("Error: couldn't find the path to GoBibleCreator.jar");
		
		return null;
	}
	
	public static String extractSourceTextPath(File collectionsFile) throws IOException
	{
		String sourceTextPath = null;
		
		// Open the file for reading one line at a time in UTF-8 character encoding
		LineNumberReader reader = new LineNumberReader(new InputStreamReader(new FileInputStream(collectionsFile), "UTF-8"));
		
		String line = null;
		String sourceTextPropertyName = "Source-Text:";
		
		// Read the collections in the file
		while ((line = reader.readLine()) != null)
		{
			// Test if line contains the source text property
			if (line.startsWith(sourceTextPropertyName))
			{
				sourceTextPath = line.substring(sourceTextPropertyName.length()).trim();
                break;
			}
		}
		
		reader.close();
		
		if (sourceTextPath == null)
		{
			System.err.println("Error parsing collections file: No Source-Text specified.");
			System.err.println("The source text is either a ThML or OSIS XML file.");
			System.err.println("It must be specified as follows:");
			System.err.println("Source-Text: Source.xml");
			System.err.println("For example:");
			System.err.println("Source-Text: kjv.thml");
		}
		
		return sourceTextPath;
	}

       /*
	 * @return the enum of the source file format (i.e., Thml/osis/usfm)
	 */
	public static SourceFormatType extractSourceFormatType(File collectionsFile) throws IOException
	{
		SourceFormatType srcFormatType = SourceFormatType.unknown;
		
		// Open the file for reading one line at a time in UTF-8 character encoding
		LineNumberReader reader = new LineNumberReader(new InputStreamReader(new FileInputStream(collectionsFile), "UTF-8"));
		
		String line = null;
		String sourceTextPropertyName = "Source-Format:";
		
		// Read the collections in the file
		while ((line = reader.readLine()) != null)
		{
			// Test if line contains the source text property
			if (line.startsWith(sourceTextPropertyName))
			{
				String sFormat =  line.substring(sourceTextPropertyName.length()).trim().toLowerCase();

				if (sFormat.equals("osis"))
					{ srcFormatType = SourceFormatType.osis; }
				else if (sFormat.equals("thml"))
					{ srcFormatType = SourceFormatType.thml; }
				else if (sFormat.equals("usfm"))
					{ srcFormatType = SourceFormatType.usfm; }
				else
					{ srcFormatType = SourceFormatType.unknown; }
				break;
           	}
		}
		reader.close();
		return srcFormatType;
	}

        /*
	 * @return the extension of the the USFM files
	 */
	public static void extractUSFMfields(File collectionsFile) throws IOException
	{
		// Open the file for reading one line at a time in UTF-8 character encoding
		LineNumberReader reader = new LineNumberReader(new InputStreamReader(new FileInputStream(collectionsFile), "UTF-8"));

		String line = null;
		// Read the collections in the file
		while ((line = reader.readLine()) != null)
		{
                    // Test if line contains the source text property
                    if (line.startsWith("Source-FileExtension:"))
                    {
                        usfmSourceFileExtension =  line.substring("Source-FileExtension:".length()).trim().toLowerCase();
                        //check for starting period and strip off
                        usfmSourceFileExtension = usfmSourceFileExtension.replaceAll("\\.", "");
                    }
                    if (line.startsWith("USFM-TitleTag:"))
                    {
                        sTitleTag  =  line.substring("USFM-TitleTag:".length()).trim().toLowerCase();
                    }
                    if (line.startsWith("RedLettering:"))
                    {
                        //defaults to true
                        if (line.toLowerCase().indexOf("false") > 0 || line.toLowerCase().indexOf("no") > 0)
                        {
                            useRedLettering = false;
                        }
                    }
		}
		reader.close();

		return;
	}

    /*
	 * This is a common place where globals / common elements can be read from
	 * the collections file that apply to all data types. It does mean the file
	 * is opened and read an additional time, but this process isn't meant to be
	 * fast of efficient - but rather correct.
	 *
	 */
	public static void extractCommonFields(File collectionsFile) throws IOException
	{
		// Open the file for reading one line at a time in UTF-8 character encoding
		LineNumberReader reader = new LineNumberReader(new InputStreamReader(new FileInputStream(collectionsFile), "UTF-8"));

		String line = null;
		// Read the collections in the file
		while ((line = reader.readLine()) != null)
		{
			if (line.startsWith("RedLettering:"))
			{
				//defaults to true
				if (line.toLowerCase().indexOf("false") > 0 || line.toLowerCase().indexOf("no") > 0)
				{
					useRedLettering = false;
				}
			}
			// Empth verse text
			else if (line.startsWith("Empty-Verse-Text:"))
			{
				// Grab text to display where the verse text is missing
				EmptyVerseString = line.substring(17).trim();
			}
		}
		reader.close();

		return;
	}


	/*
	 * tries one last time to parse the source file to determine the file format
	 */
	public static SourceFormatType SourceFormatTypeRetry(String baseSourceDirectory, String sourceTextPath) throws IOException
	{
            SourceFormatType srcFormatType = SourceFormatType.unknown;
            try
            {
                //convert source text to file
                File fIn = new File(baseSourceDirectory, sourceTextPath);
                //read in up to the first 200 lines and see if you can spot the tags
                BufferedReader readerIn = new BufferedReader(new FileReader(fIn));
                String lineIn;
                int iRowCount = 0;
                while ((lineIn = readerIn.readLine()) != null || iRowCount < 200)
                {
                    lineIn = lineIn.toLowerCase();
                    if (lineIn.indexOf(OsisConverter.OSIS_TAG.toLowerCase()) > 0)
                    {
                        srcFormatType = SourceFormatType.osis;
                        break;
                    }
                    if (lineIn.indexOf(ThmlConverter.THML_TAG.toLowerCase()) > 0)
                    {
                        srcFormatType= SourceFormatType.thml;
                        break;
                    }
                    iRowCount++;
                }
                readerIn.close();
            }
            catch(Exception e)
            {
                System.out.println("Error: " + e.getMessage());
            }
            return srcFormatType;
        }
        
        /**
	 * Parses the XML file as OSIS or ThML format and extracts and returns
	 * books as a HashMap.
	 */
	public static HashMap parseXml(File xmlFile) throws IOException
	{
		System.out.println("Parsing " + xmlFile.getName() + "...");
	
		// Parse the XML file
		XMLObject xml = XMLParser.parseFile(xmlFile);

		System.out.println("Parsing " + xmlFile.getName() + " complete.");
		
		// Determine whether the XML file contains OSIS content.
		XMLObject parent = xml.getChild(OsisConverter.OSIS_TAG);
		
		GoBibleCreator creator = null;
		
		if (parent != null)
		{
			System.out.println("Converting " + xmlFile.getName() + " from OSIS format.");
			creator = new OsisConverter();
		}
		else
		{
			System.out.println("Converting " + xmlFile.getName() + " from ThML format.");

			parent = xml.getChild(ThmlConverter.THML_TAG);

			for (Enumeration e = xml.getChildren(); e.hasMoreElements(); )
			{
				XMLObject child = (XMLObject) e.nextElement();

				//System.out.println("child: " + child.getTag());
			}

			if (parent != null)
			{
				creator = new ThmlConverter();
			}
			else
			{
				System.out.println("Error: XML file does not contain OSIS or ThML content: " + xmlFile.getName());
				return null;
			}
		}
		
		HashMap books = creator.parse(parent);
		
		return books;
	}

        /*
	 * Parses the USFM files format and extracts and returns
	 * books as a HashMap.
	 */
	public static HashMap parseUSFM(String baseSourceDirectory, String sourceTextPath, String sTitleTag) throws IOException
	{
            //iterate through the source file directory and get the filenames present
            //USFM uses a predefined file format of:
            // 01GENxxxx.ptx
            HashMap books = new HashMap();
            if (baseSourceDirectory == null)
            {
                baseSourceDirectory = "";
            }
            
            try
            {
                System.out.println("Base Dir: " + baseSourceDirectory);
                System.out.println("Source Path: " + sourceTextPath);
                
                //get the list of files in the directory
                File folder = new File(baseSourceDirectory, sourceTextPath);
                File[] listOfFiles = folder.listFiles();

                for (int i = 0; i < listOfFiles.length; i++) 
                {
                  if (listOfFiles[i].isFile()) 
                  {
                      String sFileName = listOfFiles[i].getName();
                      if (sFileName.toLowerCase().endsWith("." + usfmSourceFileExtension))
                      {
                          System.out.println("File " + listOfFiles[i].getName());
                          String sFilename = folder.toString() + "\\" + listOfFiles[i].getName();
                          // Create a new book
                          Book book = new Book(sFilename, STYLE_RED, fileCodepage, useRedLettering, sTitleTag);
                          book.fileName = listOfFiles[i].getName();

                          // Add book to the lookup table
                          try
                          {
                            books.put(book.name, book);
                          }
                          catch (Exception e)
                          {
                              System.out.println("Error: " + e.getMessage());
                          }
                          bookNames.add(book.name);
                      }
                  } 
                }
            }
            catch (Exception e)
            {
              System.out.println("Error: " + e.getMessage());
            }
            return books;
	}
        
        
	/**
	 * No collections file was specified so generating collections file.
	 */
	public static void generateCollectionsFile(File collectionsFile, String xmlFileName, HashMap books) throws IOException
	{
		System.out.println("No Collections file specified, generating: " + collectionsFile.getPath());
		
		PrintWriter writer = new PrintWriter(new FileWriter(collectionsFile));
		
		// Make the collection name the file name without extension
		int dotIndex = xmlFileName.lastIndexOf('.');
		String collectionName = xmlFileName;
		
		if (dotIndex > 0)
		{
			collectionName = collectionName.substring(0, dotIndex);
		}

		writer.println("Collection: " + collectionName);
		
		// Print out books
        for (String bookName: bookNames)
		{
			writer.println("Book: " + bookName);
		}
		
		writer.close();
		
		System.out.println("Collections file generated.");
	}

	/**
	 * Converts XML into a HashMap of Book objects indexed by book name.
	 * @param xml Parsed XML.
	 * @return HashMap of Book objects.
	 */
	public HashMap parseBooks(XMLObject xml, String bodyTag, String testamentTags[], String bookTag, /*String bookNameAttribute,*/ String primaryBookNameAttribute, String secondaryBookNameAttribute)
	{
		HashMap books = new HashMap();

		// Flag to count how many books have no verses
		int numBooksWithOutVerses = 0;

		// The ThML body (as opposed to the head) contains the actual contents of the Bible
		XMLObject body = xml.getChild(bodyTag);

		// Go through the testaments of the ThML body looking for books
		for (Enumeration t = body.getChildren(); t.hasMoreElements();)
		{
			XMLObject testament = (XMLObject) t.nextElement();

			// We are only interested in testaments
			boolean found = false;
			for (int i=0; i < testamentTags.length; i++)
			{
				if (testament.getTag().equals(testamentTags[i]))
				{
					found = true;
					break;
				}
			}

			// only process if we found one of the given testament tags
			if (found)
			{
				// Find all of the books in each testament
				for (Enumeration e = testament.getChildren(); e.hasMoreElements(); )
				{
					XMLObject xmlBook = (XMLObject) e.nextElement();

					// See if it is a book tag
					if (xmlBook.getTag().equals(bookTag))
					{
						// Create a new book
						Book book = new Book(xmlBook, this, primaryBookNameAttribute, secondaryBookNameAttribute);

						// Check to see if the book has any verses in it, log an error if it doesn't'
						if (!book.hasVerses())
						{
							System.out.println("Error: Book (" + book.name+") doesn't contain any verses.");
							numBooksWithOutVerses++;
						}
						// Add book to the lookup table
						books.put(book.name, book);

						bookNames.add(book.name);
					}
				}
			}
		}
		
		if (numBooksWithOutVerses > 0)
		{
			//System.err.println("Error: " + numBooksWithOutVerses + ");
		}
		return books;
	}
	
	/**
	 * Parses the English UI properties file. The English definitions
	 * can be overridden in the Collections file.
	 */
	public static void parseUiProperties() throws IOException
	{
                
        File uiFile = new File(getJarDirectory(), "GoBibleCore/ui.properties");
        boolean bRet = uiFile.exists();
        String sFileExists = "False";
        if (bRet)
            sFileExists = "True";
        System.out.println(uiFile.getAbsolutePath() + "  exists:" + sFileExists);
		// Open the file for reading one line at a time in UTF-8 character encoding
		LineNumberReader reader = new LineNumberReader(new InputStreamReader(new FileInputStream(uiFile), "UTF-8"));
		
		String line = null;
		
		// Read the properties in the file
		while ((line = reader.readLine()) != null)
		{
			if (line.startsWith("UI-"))
			{
				int index = line.indexOf(':');
				uiProperties.put(line.substring(0, index), line.substring(index + 1));
			}
			// If line isn't empty then report that we don't know what it is
			else if (!line.trim().equals(""))
			{
				System.out.println("Error parsing ui.properties file. Can't understand line:\n" + line);
			}
		}
		
		// Close the file
		reader.close();
	}
	
	/**
	 * Creates a vector of collections from the specified file.
	 * @param collectionsFile Collections file with the format specified above.
	 * @param books HashMap of books indexed by book name.
	 * @return Vector of Collection objects.
	 **/
	public static Vector parseCollectionsFile(File collectionsFile, HashMap books) throws IOException
	{
		// Open the file for reading one line at a time in UTF-8 character encoding
		LineNumberReader reader = new LineNumberReader(new InputStreamReader(new FileInputStream(collectionsFile), "UTF-8"));
		
		Vector collections = new Vector();
		Collection collection = null;
		String line = null;
		int lineCount = 0;
		// Read the collections in the file
		while ((line = reader.readLine()) != null)
		{
			lineCount++;
			//ignore blank lines
			if (!line.trim().equals(""))
			{
			//test to see if the line is a comment line
			if (line.startsWith("//") || line.startsWith("rem") || line.startsWith("REM"))
			{
				//do nothing - ignore line
			}
			// Test if line specifies a WAP site for the JAD files
			else if (line.startsWith("Wap-site:"))
			{
				wapSite = line.substring(9).trim();
			}
			// read in the Info URL if it is given
			else if (line.startsWith("MIDlet-Info-URL:"))
			{
				midlet_infoURL = line.substring("MIDlet-Info-URL:".length()).trim();
			}
			// read in the Vendor if it is given
			else if (line.startsWith("MIDlet-Vendor:"))
			{
				midlet_vendor = line.substring("MIDlet-Vendor:".length()).trim();
			}
			// Source-Text is ignored here but is retrieved earlier from within extractSourceTextPath()
			else if (line.startsWith("Source-Text:"))
			{
			}		
			// Source-FileExtension is the extension of the USFM files in the format ptx,ltn,uz..
                        // retrieved earlier
			else if (line.startsWith("Source-FileExtension:"))
			{
 			}		
			// Source-Format is ignored here but is retrieved earlier
                        // Added in version > 2.2.6
			else if (line.startsWith("Source-Format:"))
			{
			}			
			// USFM-TitleTag is ignored here but is retrieved earlier
                        // Added in version > 2.2.6
			else if (line.startsWith("USFM-TitleTag:"))
			{
			}			
			// Phone-Icon-Filepath is the filepath to the alternate icon
                        // to be displayed by the application
                        // Added in version > 2.2.6
			else if (line.startsWith("Phone-Icon-Filepath:"))
			{
                            phoneIconFilepath = line.substring("Phone-Icon-Filepath:".length()).trim();
                        }
                        else if (line.startsWith("Codepage:"))
			{
                            fileCodepage = line.substring("Codepage:".length()).trim();
                        }
                        // alternate name for the application that will be displayed in the 
                        // phone title
                        // introduced in version > 2.2.6
  			else if (line.startsWith("Application-Name:"))
			{
                            applicationName = line.substring("Application-Name:".length()).trim();
			}
                        // Check to see if the user wants to use red lettering
                        // brought in earlier
                        else if (line.startsWith("RedLettering:"))
                        {
                            //defaults to true
                            if (line.toLowerCase().indexOf("false") > 0 || line.toLowerCase().indexOf("no") > 0)
                            {
                                useRedLettering = false;
                            }
                        }
			// Test if line specifies Info property
			else if (line.startsWith("Info:"))
			{
				infoString = line.substring(5).trim();
			}
			// Test if line specifies Custom-Font property
			else if (line.startsWith("Custom-Font:"))
			{
				customFontString = line.substring(12).trim();
			}
			// Test if line specifies Language-Code property
			else if (line.startsWith("Language-Code:"))
			{
				int commaIndex = line.indexOf(',');
				if (commaIndex == -1)
				{
					languageCode = line.substring(14).trim();
					languageCodePosition = languageCodePositionType.suffix;	// default value
				}
				else
				{
					languageCode = line.substring(14, commaIndex);
					String langPosition = line.substring(commaIndex + 1).trim();
					if (langPosition.equalsIgnoreCase("prefix"))
					{
						languageCodePosition = languageCodePositionType.prefix;
					}
					else if (langPosition.equalsIgnoreCase("suffix"))
					{
						languageCodePosition = languageCodePositionType.suffix;
					}
					else
					{
						languageCodePosition = languageCodePositionType.suffix;	// default value
						System.out.println("Error parsing collections file. Unsupported Language-Code position: " + langPosition + ", use 'prefix' or 'suffix'. (EX: Language-Code: en, prefix)");
					}
				}
			}
			// Test if line specifies MIDP property
			else if (line.startsWith("MIDP:"))
			{
				String midpVersionString = line.substring(5).trim();

				// Release 2.4.0 will no longer support MIDP-1.0
				//if (midpVersionString.equals("MIDP-1.0"))
				//{
				//	midpVersion = midpVersionString;
				//	MAX_FILE_SIZE = MAX_FILE_SIZE_MIDP_1;
				//}
				//else
				if (!midpVersionString.equals(midpVersion)) //"MIDP-2.0"))
				{
					// System.out.println("Error parsing collections file. Unsupported MIDP version: " + midpVersionString + ", try MIDP-1.0 or MIDP-2.0");
					System.out.println("Error parsing collections file. Unsupported MIDP version: " + midpVersionString + ", use " + midpVersion);//MIDP-2.0");
				}
			}
			// Test if this is the alignment property
			else if (line.startsWith("Align:"))
			{
				// Set the alignment
				String alignString = line.substring(6).trim().toLowerCase();
				if (alignString.equals("left"))
				{
					align = ALIGN_LEFT;
				}
				else if (alignString.equals("right"))
				{
					align = ALIGN_RIGHT;
				}
				else
				{
					System.out.println("Error passing collections file. Did not understand align property: '" + alignString + "'. Must be either 'Left' or 'Right'.");
				}
			}
			// Test if this is a book name map
			else if (line.startsWith("Book-Name-Map:"))
			{
				// Grab the book short name
				String bookShortName = line.substring(14);
				
				int commaIndex = bookShortName.indexOf(',');
				
				String bookLongName = bookShortName.substring(commaIndex + 1).trim();
				
				bookShortName = bookShortName.substring(0, commaIndex).trim();
				
				if (!updateOnly)
				{
					// Find the book
					Book book = (Book) books.get(bookShortName);
					
					if (book == null)
					{
						System.out.println("Warning: can't find book name: " + bookShortName);
						System.out.println("Check Book-Name-Map entries with source text file[s].");
					}
					else
					{
						// Set the book's long name
						book.name = bookLongName;
					}
				}
			}
			// Test if this is a new collection
			else if (line.startsWith("Collection:"))
			{
				String collectionName = line.substring(11).trim();
				
				String fileName = collectionName;
				
				// If there is a comma then the first name is the file name
				// and the second name is the collection name
				int commaIndex = collectionName.indexOf(',');
				
				if (commaIndex != -1)
				{
					fileName = collectionName.substring(0, commaIndex);
					collectionName = collectionName.substring(commaIndex + 1);
				}
				
				// Create a new collection with the name after "Collection:"
				if (languageCodePosition == languageCodePositionType.suffix)
				{
					collection = new Collection(fileName + " " + languageCode, collectionName + " " + languageCode);
				}
				else
				{
					collection = new Collection(languageCode + " " + fileName, languageCode + " " + collectionName);
				}
				collections.add(collection);
			}
			else if (line.startsWith("Book:"))
			{
				// If an existing collection doesn't exist then it is an error
				if (collection == null)
				{
					System.out.println("'Book:' without collection on line " + reader.getLineNumber());
				}
				
				// The book line may contain commas if start and end chapters are specified
				String bookName = line.substring(5);
				int startChapter = -1;
				int endChapter = -1;
				int commaIndex = bookName.indexOf(',');
				
				if (commaIndex >= 0)
				{
					// Grab the chapter numbers after the comma
					String startChapterString = bookName.substring(commaIndex + 1);
					
					// Remove the commas and chapter numbers from the book name
					bookName = bookName.substring(0, commaIndex);
					
					// Grab the second comma
					commaIndex = startChapterString.indexOf(',');
					
					if (commaIndex >= 0)
					{
						String endChapterString = startChapterString.substring(commaIndex + 1).trim();
						startChapterString = startChapterString.substring(0, commaIndex).trim();
						startChapter = Integer.parseInt(startChapterString);
						endChapter = Integer.parseInt(endChapterString);
					}
					else
					{
						System.out.println("Start chapter specified without end chapter on line " + reader.getLineNumber());
					}
				}
				
				// Trim whitespace from around the book name
				bookName = bookName.trim();
                                //clean up the extranous characters in the book name
                                if(sourceFormatType == SourceFormatType.usfm)
                                {
                                    bookName = USFM_Utilities.CleanBookName(bookName);
                                }
				
				// If the start and end chapters aren't specified then get them from the XML book.
				// We can only do this if updateOnly isn't set, otherwise the source text
				// won't have been parsed.
				if (startChapter == -1 && !updateOnly)
				{
                                        Book xmlBook = (Book) books.get(bookName);
					if (xmlBook == null)
					{
                                            System.out.println("Couldn't find book: " + bookName);
					}
					
					startChapter = xmlBook.startChapter;
					endChapter = xmlBook.chapters.size() + startChapter - 1;
				}
				
				// Create a new book and add it to the current collection
				Book book = new Book(bookName, startChapter, endChapter);
				collection.add(book);
			}
			else if (line.startsWith("UI-"))
			{
				// Override English definition
				int index = line.indexOf(':');
				uiProperties.put(line.substring(0, index), line.substring(index + 1));
			}
			// Empth verse text
			else if (line.startsWith("Empty-Verse-Text:"))
			{
				// Grab text to display where the verse text is missing
				EmptyVerseString = line.substring(15).trim();
			}
			// If line isn't empty then report that we don't know what it is
			else if (!line.trim().equals(""))
			{
				System.out.println("Error parsing collections file. Can't understand line<"+lineCount+">:\n" + line);
			}
                    }
		} // While
		
		// Close the file
		reader.close();
		
            return collections;
	}

	/**
	 * Writes out the collections as Go Bible data files in the specified directory.
	 * A directory with the name of the collection will be created for each collection
	 * and the Go Bible data files will be placed therein.
	 * @param directory Directory to place data files.
	 * @param collections Vector of collections that have been read from the collec`tions file.
	 * @param books HashMap of books that have been read from the XML file.
	 * @param goBibleJar JAR containing the Go Bible classes.
	 **/
	public static void writeCollections(File directory, Vector collections, HashMap books, JarFile goBibleJar) throws IOException
	{
		/*StringBuffer wapPage = new StringBuffer();
	
		wapPage.append("<html>\n<head>\n<title>Go Bible</title>\n</head>\n<body>\n\n");*/
	
		for (Enumeration e = collections.elements(); e.hasMoreElements(); )
		{
			Collection collection = (Collection) e.nextElement();
			
			System.out.println("Writing Collection " + collection.fileName + ": ");
			
			writeCollection(directory, collection, books, goBibleJar/*, wapPage*/);
		}
		
		/*if (wapSite != null)
		{
			wapPage.append("</body>\n</html>");
			
			// Write out wap page
			File wapFile = new File(directory, "wap/Welcome.html");
			
			PrintWriter writer = new PrintWriter(new BufferedWriter(new FileWriter(wapFile)));
			
			writer.print(wapPage.toString());
			writer.close();
		}*/
	}
	
	/**
	 * Creates a new directory within the specified directory and writes out the specified
	 * collection therein. Also creates a wap subdirectory where WAP JAD files are placed. Adds a new line
	 * to the wapPage StringBuffer creating a link to the WAP JAD file.
	 * @param directory Directory to place the collection in.
	 * @param collection Collection to create.
	 * @param books HashMap of books read in from XML file.
	 * @param goBibleJar JAR containing the Go Bible classes.
	 * @param wapPage StringBuffer containing the contents of the WAP page where a new line will be added for
	 * the current collection.
	 **/
	public static void writeCollection(File directory, Collection collection, HashMap books, JarFile goBibleJar/*, StringBuffer wapPage*/) throws IOException
	{
		// Create a manifest file for the new JAR
		MyManifest manifest = new MyManifest();
		
		Attributes attributes = manifest.getMainAttributes();
		
		attributes.putValue("Manifest-Version", "1.0");
                if (applicationName != null)
                {
        		attributes.putValue("MIDlet-Name", collection.name + " " + applicationName);
                        attributes.putValue("MIDlet-1", collection.name + " " + applicationName + ", Icon.png, GoBible");
                }
                else
                {
           		attributes.putValue("MIDlet-Name", collection.name + NAME_APPENDAGE);
                        attributes.putValue("MIDlet-1", collection.name + NAME_APPENDAGE + ", Icon.png, GoBible");
                }
                //attributes.putValue("MIDlet-Name", collection.name + NAME_APPENDAGE);
                //attributes.putValue("MIDlet-1", collection.name + NAME_APPENDAGE + ", Icon.png, GoBible");
		attributes.putValue("MIDlet-Icon", "Icon.png");
		if (midlet_vendor == null)
			midlet_vendor = "Jolon Faichney";
		attributes.putValue("MIDlet-Vendor", midlet_vendor);
		// attributes.putValue("MIDlet-Vendor", "Jolon Faichney");
		attributes.putValue("MIDlet-Version", versionString);/*
		attributes.putValue("TestString",
"\u0661\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0"
+ "\u0662\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0"
+ "\u0663\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0"
+ "\u0664\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0"
+ "\u0665\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0"
+ "\u0666\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0"
+ "\u0667\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0"
+ "\u0668\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0"
+ "\u0669\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0"
+ "\u066a\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0"
+ "\u066b\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0\u06d0"
);*/
		attributes.putValue("MIDlet-Data-Size", "100");
		attributes.putValue("MicroEdition-Configuration", "CLDC-1.0");
		attributes.putValue("MicroEdition-Profile", midpVersion);

		// Write out info property
		if (infoString != null)
		{
			attributes.putValue("Go-Bible-Info", infoString);
		}
		
		// Write out the alignment property
		attributes.putValue("Go-Bible-Align", ALIGN_TEXT[align]);

		// Create a String to contain the UI properties
		String uiPropertiesString = "";
		Set uiMappings = uiProperties.entrySet();
		
		for (Iterator i = uiMappings.iterator(); i.hasNext(); )
		{
			Map.Entry entry = (Map.Entry) i.next();
			uiPropertiesString += entry.getKey() + ":" + entry.getValue() + "\n";
		}

		// Create a new JAR file using the contents of the Go Bible JAR and the new manifest
		File jarFile = new File(directory, collection.fileName + ".jar");

		// If updateOnly is true then we haven't parsed the source text and hence
		// won't be generating any books or verse data so we are going to
		// source the Bible Data from the existing JAR file
		if (updateOnly)
		{
			updateCollectionJar(directory, jarFile, collection, goBibleJar, manifest, uiPropertiesString);	
		}
		else
		{
			writeCollectionJar(directory, jarFile, collection, books, goBibleJar, manifest, uiPropertiesString);
		}
				
		// Create the JAD file that will go in the zip
		createJadFile(directory, collection, jarFile.length(), collection.fileName + ".jar");
		
		if (wapSite != null)
		{
			File wapDirectory = new File(directory, "wap");
			
			if (!wapDirectory.exists())
			{
				wapDirectory.mkdir();
			}	

			// Create the JAD file for the WAP site
			createJadFile(wapDirectory, collection, jarFile.length(), wapSite + "/" + directory.getName() + "/" + collection.fileName + ".jar");
			
			// Add a line to the wapPage
			/*wapPage.append("<object declare=\"declare\" id=\"" + collection.fileName + "\" data=\"" + wapSite + collection.fileName + ".jad\" type=\"text/vnd.sun.j2me.app-descriptor\"></object>\n");
			wapPage.append("<a jad=\"#" + collection.fileName + "\" href=\"" + wapSite + collection.fileName + ".jad" + "\">" + collection.name + " " + (jarFile.length() >> 10) + "k</a><br/>\n\n");*/
		}
	}
	
	/**
	 * 1. Rename existing jar file appending ".tmp"
	 * 2. Create new jar file with initial name
	 * 3. Copy "Bible Data" entry of tmp jar file to new jar file
	 * 4. Delete tmp jar file.
	 */
	public static void updateCollectionJar(File directory, File jarFile, Collection collection, JarFile goBibleJar, Manifest manifest, String uiPropertiesString) throws IOException
	{
		// Make sure the existing JAR exists
		if (jarFile.exists())
		{
			File tmpFile = new File(directory, collection.fileName + ".jar" + ".tmp");
			
			jarFile.renameTo(tmpFile);
			
			// Recreate the JAR file using the tmp as a base
			jarFile = new File(directory, collection.fileName + ".jar");
			
			JarFile tmpJar = new JarFile(tmpFile);

			JarOutputStream jarOutputStream = new JarOutputStream(new BufferedOutputStream(new FileOutputStream(jarFile)), manifest);
			
			// Copy in the Bible Data from the original JAR
			copyInContentsOfJar(tmpJar, jarOutputStream, "Bible Data");
			
			// Copy in GoBibleCore
			copyInContentsOfJar(goBibleJar, jarOutputStream, null);

			// Create a JAR entry for the UI properties
			jarOutputStream.putNextEntry(new JarEntry(UI_PROPERTIES_FILE_NAME));
			jarOutputStream.write(uiPropertiesString.getBytes("UTF-8"));
			
			jarOutputStream.close();
			
			// Remove the tmp file
			tmpFile.delete();
		}
		else
		{
			System.out.println("Error: existing JAR file does not exist: " + jarFile.getAbsolutePath());
			System.out.println("An existing JAR file must exist when the -u option is used.");
		}
	}
	
	/**
	 * Creates a JAR file containing all of the Books specified in the Collections.txt file.
	 */
	public static void writeCollectionJar(File directory, File jarFile, Collection collection, HashMap books, JarFile goBibleJar, Manifest manifest, String uiPropertiesString) throws IOException
	{
		JarOutputStream jarOutputStream = new JarOutputStream(new BufferedOutputStream(new FileOutputStream(jarFile)), manifest);

		// Copy from Go Bible Jar
		copyInContentsOfJar(goBibleJar, jarOutputStream, null);

		// Create a JAR entry for the UI properties
		jarOutputStream.putNextEntry(new JarEntry(UI_PROPERTIES_FILE_NAME));
		jarOutputStream.write(uiPropertiesString.getBytes("UTF-8"));
	
		writeMultipleIndex(jarOutputStream, collection, books);
		writeMultipleBooks(jarOutputStream, collection, books);

		jarOutputStream.close();

		System.out.println(collection.books.size() + " book(s) written.");
	}
	
	/**
	 * Copies the contents of one JAR into another already open JAR.
	 * @param jar The source JAR to copy from.
	 * @param jarOutputStream The destination JAR to copy to.
	 * @param name Will only copy entries that begin with this name, can be null to copy everything.
	 */
	public static void copyInContentsOfJar(JarFile jar, JarOutputStream jarOutputStream, String name) throws IOException
	{
		// Copy contents of Go Bible JAR into new JAR
		for (Enumeration e = jar.entries(); e.hasMoreElements(); )
		{
			JarEntry jarEntry = (JarEntry) e.nextElement();
			
			//System.out.println("Reading entry from GoBible.jar: " + jarEntry.getName());
			
			String entryName = jarEntry.getName();
			String sFilepath = "";
			// Ignore existing manifest, and ui.properties file if
			// the Collections file has specified UI properties
			if (!entryName.startsWith("META-INF") && !entryName.equals(UI_PROPERTIES_FILE_NAME) && (name == null || entryName.startsWith(name)))
			{
                            boolean bNewIcon = false;
                            if (entryName.equals("Icon.png") && phoneIconFilepath != null)
                            {
                                //check for file existance first
                                File oFile = new File(baseSourceDirectory, phoneIconFilepath);
                                if (oFile.exists())
                                {
                                    bNewIcon = true;
                                    sFilepath = oFile.getPath();
                                }
								else
									System.out.println("Error: Icon file doesn't exist <"+phoneIconFilepath+">.");

                            }
                            
                            InputStream inputStream;
                            if (!bNewIcon)
                            {
                                 // Add entry to new JAR file
                                jarOutputStream.putNextEntry(jarEntry);
                               //copy over the resource from the jar
                                inputStream = new BufferedInputStream(jar.getInputStream(jarEntry));
                                // Read all of the bytes from the Go Bible JAR file and write them to the new JAR file
                                byte[] buffer = new byte [100000];
                                int length;

                                while ((length = inputStream.read(buffer)) != -1)
                                {
                                jarOutputStream.write(buffer, 0, length);
                                }

                                // Close the input stream
                                inputStream.close();
                           }
                            else
                            {
                                //add in the replacement icon
                                byte[] buffer = new byte[100000]; 
                                FileInputStream fi = new FileInputStream(sFilepath); 
                                inputStream = new BufferedInputStream(fi, 100000) ; 
                                ZipEntry entry = new ZipEntry("Icon.png"); 
                                jarOutputStream.putNextEntry ( entry ) ; 

                                int count; 
                                while ((count=inputStream.read(buffer, 0, 100000)) != -1 )
                                {  
                                   jarOutputStream.write ( buffer, 0, count  ) ; 
                                }  
                                inputStream.close(); 
                            }
                        }
		}		
	}
	
	/**
	 * Creates a JAD file in the specified location, for the specified collection, with the specified JAR file length,
	 * with the specified URL.
	 * @param directory Location to place the JAD file.
	 * @param collection Collection for which the JAD file is being created for.
	 * @param jarFileLength Length in bytes of the JAR file.
	 * @param url The string to place in the MIDlet-Jar-URL property.
	 **/
	public static void createJadFile(File directory, Collection collection, long jarFileLength, String url) throws IOException
	{
		// Create a JAD file
		
		File jadFile = new File(directory, collection.fileName + ".jad");
		
		PrintWriter writer = new PrintWriter(new BufferedWriter(new OutputStreamWriter(new FileOutputStream(jadFile), "UTF-8")));
		
                if (applicationName != null)
                {
        		writer.println("MIDlet-1: " + collection.name + " " + applicationName + ", Icon.png, GoBible");
                	writer.println("MIDlet-Name: " + collection.name + " " + applicationName);
                }
                else
                {
        		writer.println("MIDlet-1: " + collection.name + NAME_APPENDAGE + ", Icon.png, GoBible");
        		writer.println("MIDlet-Name: " + collection.name + NAME_APPENDAGE);
                }
       		//writer.println("MIDlet-1: " + collection.name + NAME_APPENDAGE + ", Icon.png, GoBible");
                //writer.println("MIDlet-Name: " + collection.name + NAME_APPENDAGE);
 		writer.println("MIDlet-Icon: Icon.png");
		writer.println("MIDlet-Jar-Size: " + jarFileLength);
		writer.println("MIDlet-Jar-URL: " + url);
		// writer.println("MIDlet-Info-URL: http://wap.jolon.org");
		// writer.println("MIDlet-Vendor: Jolon Faichney");
		if (midlet_infoURL == null)
			midlet_infoURL = "http://wap.jolon.org";
		writer.println("MIDlet-Info-URL: " + midlet_infoURL);
		if (midlet_vendor == null)
			midlet_vendor = "Jolon Faichney";
		writer.println("MIDlet-Vendor: " + midlet_vendor);
		writer.println("MIDlet-Version: " + versionString);
		writer.println("MIDlet-Data-Size: 100");
		writer.println("MicroEdition-Profile: " + midpVersion);

		// Write out info property
		if (infoString != null)
		{
			writer.println("Go-Bible-Info: " + infoString + " Ver. " + versionString);
		}
		
		// Write out the alignment property
		writer.println("Go-Bible-Align: " + ALIGN_TEXT[align]);

		writer.close();
	}
	
	/**
	 * Writes the index of the collection.
	 * @param directory Directory to place index.
	 * @param collection Collection to create.
	 * @param books Books parsed from ThML file.
	 **/
	public static void writeMultipleIndex(/*File directory,*/ JarOutputStream jarOutputStream, Collection collection, HashMap books) throws IOException
	{
		// Write out the main bible index
		
//		DataOutputStream output = new DataOutputStream(new BufferedOutputStream(new FileOutputStream(new File(directory, "Index"))));

		ByteArrayOutputStream byteArrayOutputStream = new ByteArrayOutputStream();
		DataOutputStream output = new DataOutputStream(byteArrayOutputStream);
		
		// Write out the number of books
		output.write(collection.books.size());
		
		// Write the number of chapters and verses for each book
		for (Enumeration e = collection.books.elements(); e.hasMoreElements(); )
		{
			Book collectionBook = (Book) e.nextElement();
			Book thmlBook = (Book) books.get(collectionBook.name);
			// Write the book name
			output.writeUTF(thmlBook.name);
			
			// Write the book's file name
			output.writeUTF(thmlBook.fileName);

			// Write the start chapter and number of chapters
			output.writeShort(collectionBook.startChapter);
			output.writeShort(collectionBook.endChapter - collectionBook.startChapter + 1);
			
			int fileNumber = 0;
			int fileLength = 0;
			
			for (int i = collectionBook.startChapter; i <= collectionBook.endChapter; i++ )
			{
				Chapter chapter = (Chapter) thmlBook.chapters.elementAt(i - thmlBook.startChapter);

				if (COMBINED_CHAPTERS)
				{
					// If this isn't the first chapter for the file and the length of the
					// next chapter will be greater than the maximum allowed file length
					// then put this chapter into the next file
					if ((fileLength != 0) && ((fileLength + chapter.allVerses.length() - MAX_FILE_SIZE) > (MAX_FILE_SIZE - fileLength)))
					{
						fileNumber++;
						fileLength = 0;
					}
									
					fileLength += chapter.allVerses.length();
					chapter.fileNumber = fileNumber;

					output.write(fileNumber);
					output.writeInt(chapter.allVerses.length());
				}
				
				output.write(chapter.verses.size());
			}
		}
		
		output.close();
		
		// Get the bytes of the index so that they can be written to the JAR file
		byte[] byteArray = byteArrayOutputStream.toByteArray();
		
		jarOutputStream.putNextEntry(new JarEntry("Bible Data/Index"));
		jarOutputStream.write(byteArray, 0, byteArray.length);
	}
	
	/**
	 * Writes out the actual book data.
	 * @param directory Directory to place the Go Bible data.
	 * @param collection Collection to create.
	 * @param books Books from the XML file.
	 **/
	public static void writeMultipleBooks(JarOutputStream jarOutputStream, Collection collection, HashMap books) throws IOException
	{
		// Write the number of chapters and verses for each book
		for (Enumeration e = collection.books.elements(); e.hasMoreElements(); )
		{
			Book collectionBook = (Book) e.nextElement();
			
			// Get the book from the XML file
			Book thmlBook = (Book) books.get(collectionBook.name);
			
			System.out.print(thmlBook.fileName + ", ");
			
			// Write out the book index
			writeMultipleBookIndex(jarOutputStream, collectionBook, thmlBook);

			DataOutputStream dataOutputStream = new DataOutputStream(jarOutputStream);

			int fileNumber = 0;

			StringBuffer buffer = new StringBuffer();

			for (int chapterNumber = collectionBook.startChapter; chapterNumber <= collectionBook.endChapter; chapterNumber++)
			{
				Chapter chapter = (Chapter) thmlBook.chapters.elementAt(chapterNumber - thmlBook.startChapter);
				
				// Write out the chapter
				
				if (COMBINED_CHAPTERS)
				{
					// If the file number has changed then write out the file
					if (chapter.fileNumber != fileNumber)
					{
						//byte[] byteArray = buffer.toString().getBytes("UTF-8");

						//System.out.println("Writing " + thmlBook.name + " " + fileNumber + " - " + byteArray.length + " bytes");
						
						jarOutputStream.putNextEntry(new JarEntry("Bible Data/" + thmlBook.fileName + "/" + thmlBook.fileName + " " + fileNumber));
						//jarOutputStream.write(byteArray, 0, byteArray.length);
						//dataOutputStream.writeUTF(buffer.toString());
						
						// Convert the StringBuffer to bytes
						byte[] verseBytes = buffer.toString().getBytes("UTF-8");
						dataOutputStream.writeInt(verseBytes.length);
						dataOutputStream.write(verseBytes, 0, verseBytes.length);

						fileNumber = chapter.fileNumber;
						buffer = new StringBuffer();
					}

					buffer.append(chapter.allVerses.toString());
				}
				else
				{
					ByteArrayOutputStream byteArrayOutputStream = new ByteArrayOutputStream();
					DataOutputStream output = new DataOutputStream(byteArrayOutputStream);
					
					String allVerses = chapter.allVerses.toString();
					
					//System.out.println("Writing " + thmlBook.name + " " + chapterNumber + " - " + allVerses.length() + " bytes");
					
					output.writeUTF(allVerses);

					output.close();
					
					byte[] byteArray = byteArrayOutputStream.toByteArray();
					
					jarOutputStream.putNextEntry(new JarEntry("Bible Data/" + thmlBook.fileName + "/" + thmlBook.fileName + " " + chapterNumber));
					jarOutputStream.write(byteArray, 0, byteArray.length);
				}
			}
			
			if (COMBINED_CHAPTERS)
			{
				// Write out the final file
				//byte[] byteArray = buffer.toString().getBytes("UTF-8");

				//System.out.println("Writing " + thmlBook.name + " " + fileNumber + " - " + byteArray.length + " bytes");
				
				jarOutputStream.putNextEntry(new JarEntry("Bible Data/" + thmlBook.fileName + "/" + thmlBook.fileName + " " + fileNumber));
				//jarOutputStream.write(byteArray, 0, byteArray.length);
				//dataOutputStream.writeUTF(buffer.toString());

				// Convert the StringBuffer to bytes
				byte[] verseBytes = buffer.toString().getBytes("UTF-8");
				dataOutputStream.writeInt(verseBytes.length);
				dataOutputStream.write(verseBytes, 0, verseBytes.length);
			}
		}	
	}
	
	/**
	 * Writes out the index for a book.
	 * @param collectionBook Contains the book start and end chapters to write out.
	 * @param xmlBook Contains the book data to write out.
	 **/
	public static void writeMultipleBookIndex(/*File directory,*/ JarOutputStream jarOutputStream, Book collectionBook, Book xmlBook) throws IOException
	{
		// Create index file
		//DataOutputStream output = new DataOutputStream(new BufferedOutputStream(new FileOutputStream(new File(directory, "Index"))));
		
		ByteArrayOutputStream byteArrayOutputStream = new ByteArrayOutputStream();
		DataOutputStream output = new DataOutputStream(byteArrayOutputStream);
		
		// Write the verse sizes for every chapter
		for (int chapterNumber = collectionBook.startChapter; chapterNumber <= collectionBook.endChapter; chapterNumber++)
		{
			Chapter chapter = (Chapter) xmlBook.chapters.elementAt(chapterNumber - xmlBook.startChapter);
			
			for (Enumeration e = chapter.verses.elements(); e.hasMoreElements(); )
			{
				String verse = (String) e.nextElement();
				output.writeShort(verse.length());
			}
		}		
		
		output.close();		
		
		byte[] byteArray = byteArrayOutputStream.toByteArray();
		
		jarOutputStream.putNextEntry(new JarEntry("Bible Data/" + xmlBook.fileName + "/Index"));
		
		jarOutputStream.write(byteArray, 0, byteArray.length);
	}

	/**
	 * Goes through all of the characters in the verses and generates font bitmaps for them.
	 * This is currently experimental and shouldn't be used for any production collections.
	 **/
	public static void generateCustomFont(String customFontString, File collectionsFile, HashMap books) throws IOException
	{
		System.out.println("Generating fonts...");
		
		// Create a glyph directory
		File glyphDirectory = new File(collectionsFile.getParent(), "glyphs");
		
		if (!glyphDirectory.exists())
		{
			glyphDirectory.mkdir();
		}
		
		Font font = new Font(customFontString, Font.PLAIN, 14);
		
		BufferedImage testFontImage = new BufferedImage(1, 1, BufferedImage.TYPE_INT_ARGB);
		
		Graphics2D testGraphics = (Graphics2D) testFontImage.getGraphics();
		testGraphics.setFont(font);
		FontMetrics fontMetrics = testGraphics.getFontMetrics();
		int ascent = fontMetrics.getAscent();
		
		FontRenderContext testFontRenderContext = testGraphics.getFontRenderContext();
		
		HashMap<Integer, BufferedImage> glyphs = new HashMap<Integer, BufferedImage>();
		
		for (Object bookObject: books.values())
		{
			Book book = (Book) bookObject;
			
			for (Object chapterObject: book.chapters)
			{
				Chapter chapter = (Chapter) chapterObject;
				
				char[] charArray = chapter.allVerses.toString().toCharArray();
				
				int startIndex = 0;
				int c = 0;
				
				// Go through verse data and convert characters not yet converted
				for (int i = 0; i < charArray.length; i++)
				{
					c |= charArray[i];
					
					// If next character will be a Tamil vowel then skip to next character
					if (i < charArray.length - 1 && charArray[i + 1] >= '\u0BBE' && charArray[i + 1] <= '\u0BCD')
					{
						startIndex = i;
						c <<= 16;
						continue;
					}
					
					if (!glyphs.containsKey(c))
					{
						Rectangle2D stringBounds = font.getStringBounds(charArray, startIndex, i + 1, testFontRenderContext);
						
						String hexString = Integer.toHexString(c);
						
						System.out.println(hexString + ": " + ((char) c) + ", height: " + fontMetrics.getHeight());
						
						BufferedImage fontImage = new BufferedImage((int) stringBounds.getWidth(), fontMetrics.getHeight(), BufferedImage.TYPE_INT_ARGB);
						
						Graphics2D graphics = (Graphics2D) fontImage.getGraphics();

						graphics.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
						graphics.setColor(Color.BLACK);
						graphics.setFont(font);

						// Convert character to a bitmap
						graphics.drawString(new String(charArray, startIndex, i - startIndex + 1), 0, ascent);
						
						//ImageIO.write(fontImage, "PNG", new File(glyphDirectory, hexString + ".png"));
						
						glyphs.put(c, fontImage);
					}
					
					startIndex = i + 1;
					c = 0;
				}
			}
		}
		
		// Go through HashMap and generate new HashMap of glyph indexes.
		HashMap<Integer, Integer> glyphIndexes = new HashMap<Integer, Integer>();
		
		int index = 0;
		for (int key: glyphs.keySet())
		{
			glyphIndexes.put(key, index);
			ImageIO.write(glyphs.get(key), "PNG", new File(glyphDirectory, index + ".png"));
			index++;
		}
		
		System.out.println("Fonts generated.");

	}
	
	public abstract HashMap parse(XMLObject xml);

	/**
	 * Returns true if XMLObject is a chapter.
	 **/
	public abstract boolean isChapter(XMLObject xml);
	
	/**
	 * Extracts the chapter number from the chapter.
	 **/
	public abstract int getChapterNumber(XMLObject xml);
	
	/**
	 * Parses the contents of the chapter into the Chapter object.
	 **/
	public abstract void parseChapter(XMLObject xml, Chapter chapter);

	// UTILITY FUNCTIONS THAT CAN BE USED BY SUBCLASSES

	/**
	 * Performs essentially the same as String.trim() but leaves style characters
	 * intact.
	 */
	public String trim(String string)
	{
		String result = string;
		
		if (string.length() > 0)
		{
			// Find first non-white space character
			char[] charArray = string.toCharArray();
			
			int charIndex = 0;
			char currentChar = charArray[charIndex];
			
			while ((charIndex < charArray.length) && (charArray[charIndex] > STYLE_RED) && (charArray[charIndex] <= ' '))
			{
				charIndex++;
			}
			
			int startIndex = charIndex;
			
			if (startIndex < charArray.length)
			{
				// Find the last non-white space character
				charIndex = charArray.length - 1;
				
				while ((charIndex >= 0) && (charArray[charIndex] > STYLE_RED) && (charArray[charIndex] <= ' '))
				{
					charIndex--;
				}
				
				result = string.substring(startIndex, charIndex + 1);
			}
			else
			{
				result = "";
			}
		}
		
		return result;
	}
	
	/**
	 * HTML text may contain ampersand characters such as &quot; which need to be
	 * replaced with the " character.
	 **/
	public String convertAmpersands(String text)
	{
		String result = "";

		// Search for the next ampersand
		int semiColonIndex = -1;
		int ampersandIndex = -1;

		do
		{
			ampersandIndex = text.indexOf('&', semiColonIndex + 1);
			
			if (ampersandIndex != -1)
			{
				// Add the text up to the ampersand to the result
				result += text.substring(semiColonIndex + 1, ampersandIndex);
				
				// Find the end of the ampersand
				semiColonIndex = text.indexOf(';', ampersandIndex + 1);
				
				// Test the type of ampersand
				String type = text.substring(ampersandIndex + 1, semiColonIndex);
				
				if (type.equals("quot"))
				{
					result += '\"';
				}
			}
		}
		while (ampersandIndex != -1);
		
		// Add the final string
		result += text.substring(semiColonIndex + 1);
	
		return result;
	}

	public static String extractCDATA(XMLObject xml)
	{
		String result = "";
		
		for (Enumeration e = xml.getChildren(); e.hasMoreElements(); )
		{
			XMLObject child = (XMLObject) e.nextElement();
			
			if (child instanceof CDATA)
			{
				// Add the CDATA to the result
				result += child.getTag();
			}
			// Don't include CDATA within the sup, reference, or title tags
			else if (!child.getTag().equals("sup") && !child.getTag().equals("reference") && !child.getTag().equals("title"))
			{
				// The child itself may contain CDATA so check it
				result += extractCDATA(child);
			}
		} 
		
		return result;
	}
}

/**
 * Represents a Go Bible collection to be created.
 **/
class Collection
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

/**
 * Represents a book from the XML file or a book to be created as part of a collection.
 * Use the XML constructor to create a book from the ThML file. This parse the entire book.
 * Use the name, startChapter, endChapter constructor to create a book that represents
 * a book in a collection that is to be written out as a Go Bible data file.
 **/
class Book
{
	/** Book name. **/
	public String name;
	
	/** Short book name used for file names as it is guaranteed to be 7-bit ASCII. **/
	public String fileName;
	
	/** Chapters parsed from ThML file of type Chapter. **/
	public Vector chapters = new Vector();
	
	/** Start chapter if not 1, else -1. **/
	public int startChapter = -1;
	
	/** End chapter if not 1, else -1. **/
	public int endChapter = -1;
	
	/**
	 * Constructor for creating a book that represents a book in a collection to be created.
	 * @param name Short name of book.
	 * @param startChapter The chapter to start the book, otherwise -1 for whole chapter.
	 * @param endChapter The chapter to end the book, otherwise -1 for whole chapter.
	 **/
	public Book(String name, int startChapter, int endChapter)
	{
		this.name = name;
		this.startChapter = startChapter;
		this.endChapter = endChapter;
	}
	
        //used by osis & thml file format
	public Book(XMLObject book, GoBibleCreator creator, String primaryBookNameAttribute, String secondaryBookNameAttribute)
	{
		// Grab the book's name
		//name = book.getAttribute(bookNameAttribute, "*Error no book name*");

		// Grab the book's short name
		name = book.getAttributeAsString(primaryBookNameAttribute, null);
		
		if (name == null)
		{
			if (secondaryBookNameAttribute != null)
			{
				name = book.getAttribute(secondaryBookNameAttribute, "*Error no book name*");
			}
			else
			{
				System.out.println("Error: no book name");
			}
		}
		
		// Convert the short name to US-ASCII
		try
		{
			byte[] shortNameBytes = name.getBytes("US-ASCII");
			fileName = new String(shortNameBytes, "US-ASCII");
			
			// Replace '?' with '_' a question mark indicates an unrepresentable character
			String temp = fileName.replace('?', '_');
            fileName = temp;
			
			//System.out.println("Short name: " + shortName);
		}
		catch (UnsupportedEncodingException e)
		{
			System.out.println(e.toString());
		}
		
		//System.out.println("Writing " + name + "...");
		int chapterNumber = 0;
		
		// Find each chapter
		for (Enumeration e = book.getChildren(); e.hasMoreElements(); )
		{
			XMLObject xml = (XMLObject) e.nextElement();
			
			// See if it is a chapter tag
			if (creator.isChapter(xml))
			{
				// The chapter tags title attribute will indicate the chapter number
				// It will be in the format "Chapter 107". We need to extract
				// the chapter number as books may be split up.
				if (startChapter == -1)
				{
					chapterNumber = startChapter = creator.getChapterNumber(xml);
				}
			
				Chapter chapter = new Chapter();
				creator.parseChapter(xml, chapter);
				
				//System.out.println("Processing " + name + " " + chapterNumber + " with " + chapter.verses.size() + " verses.");
				chapters.addElement(chapter);
				chapterNumber++;
			}
		}
	}

        //USFM file format
	public Book(String sFilename, char cSTYLE_RED, String fileCodepage, boolean useRedLettering, String sTitleTag)
	{
            //open up the filename for parsing through line-by-line
            try
            {
                //convert source text to file
                File fIn = new File(sFilename);
                if (fileCodepage == null)
                {
                    fileCodepage = "UTF-8";
                }
                fileName = fIn.getName();
                //read in up to the first 200 lines and see if you can spot the tags
                BufferedReader readerIn = new BufferedReader(new InputStreamReader(new FileInputStream(fIn), fileCodepage));
                String lineIn;
                StringBuffer strBuff = new StringBuffer();
                
                while ((lineIn = readerIn.readLine()) != null)
                {
                    //check if it is line we want to keep or not
                    if(USFM_Utilities.CheckIfWeKeepLine(lineIn))
                    {
                        if (lineIn.startsWith("\\"))
                        {
                            //some line that starts with a verse
                            strBuff.append(lineIn);
                        }
                        else
                        {
                            //probably a verse continuation line so append a space
                            strBuff.append(" " + lineIn);
                        }
                    }
                }
                readerIn.close();
                
                //find the book name from the tags
                String sHtag = "";
                int iStart = strBuff.indexOf(sTitleTag);
                if (iStart != -1)
                {
                    //grab a section of text
                    if(strBuff.length() >= iStart + 200)
                    {
                        sHtag = strBuff.substring(iStart + 2, iStart + 200);
                    }
                    else
                    {
                        sHtag = strBuff.substring(iStart + 2);
                    }
                    //parse out the actual information by finding the next tag
                    //and grabbing the text between
                    iStart = sHtag.indexOf("\\");
                    name = USFM_Utilities.CleanBookName(sHtag.substring(1, iStart).trim());
                }
                
                if (sHtag.equals(""))
                {
                    System.out.println("ERROR: Can not process the following book as there are no " + sTitleTag + " tags");
                    System.out.println("Filename:" + sFilename);
                    System.out.println("Please remove the book from the collection or fix the tags...");
                }
                //split up the book into chapters
                String[] sChapters = strBuff.toString().split("\\\\c ");
                //the [0] index should be header information that we can
                //just ignore - we can check for sure by finding the first
                //index that starts with "1" 
                for (int i = 0; i < sChapters.length; i++)
                {
                    if (sChapters[i].trim().startsWith("1"))
                    {
                        iStart = i;
                        break;
                    }
                }
                
                //loop through each chapter pulling out the verses
                int iChapter = 0;
                for (int i = iStart; i < sChapters.length; i++)
                {
                    if (startChapter == -1)
                    {
                        startChapter = i;
                    }
                    iChapter++;
                    //create a new chapter
                    Chapter chapter = new Chapter();
                    String sTmp = sChapters[i].toString();
                    sTmp = USFM_Utilities.RemoveChapterHeaders(sTmp);
                    //remove all the double tags that we can just blow away
                    sTmp = USFM_Utilities.RemoveDoubleMarkerTags(sTmp);
                    //remove all the double tags that we need to blow away
                    //the text between the verses
                    sTmp = USFM_Utilities.RemoveDoubleMarkerTagsFull(sTmp);
                    //get rid of all the singular tags in the text that do
                    //not have an ending tag e.g. \p
                    sTmp = USFM_Utilities.RemoveSingularMarkerTags(sTmp);
                    //remove all the double tags that we can just blow away
                    sTmp = USFM_Utilities.ReplaceWordsOfJesus(sTmp, cSTYLE_RED, useRedLettering);
                    
                    //we should have just verse tags left
                    //parse out all the verses into individual ones
                    String[] sVerses = sTmp.split("\\\\v ");
                    String verseString = "";
                    for (int j = 0; j < sVerses.length; j++)
                    {
                        //System.out.println("Chap:[" + i + "]  Verse:[" + j + "]");
                        verseString = sVerses[j];
                        if (!verseString.equals(""))
                        {
                            //parse out the verse number in the beginning
                            //of the verse
                            verseString = USFM_Utilities.RemoveVerseNumbering(verseString);
                            //check for any remaining USFM tags...
                            if (verseString.indexOf("\\") != -1)
                            {
                                int iTagStart = verseString.indexOf("\\");
                                int iTagEnd = verseString.indexOf(" ", iTagStart);
                                if (iTagEnd == -1)
                                {
                                    iTagEnd = verseString.length();
                                }
                                System.out.println("EXTRA UNPROCESSED TAGS: [" + verseString.substring(iTagStart, iTagEnd)
                                        + "] " + name + " " + String.valueOf(iChapter) + ":" + String.valueOf(j) 
                                        + " in file: " + sFilename);
                            }

                            //add the parsed verses to the chapter object
 							if (verseString.length() == 0 && GoBibleCreator.EmptyVerseString != null)
								verseString = GoBibleCreator.EmptyVerseString;

							chapter.verses.addElement(verseString);
                            chapter.allVerses.append(verseString);		
                        }
						else
						{
							int asdf = 123;
							asdf++;
						}
                    }
                    
                    chapters.addElement(chapter);            
                }
            }
            catch(Exception e)
            {
                System.out.println("Error: " + e.getMessage());
            }
	}

	public boolean hasVerses()
	{
		int verseSize = 0;
		for (int i = 0; i < chapters.size(); i++)
		{
			Chapter c = (Chapter) chapters.elementAt(i);

			if (GoBibleCreator.EmptyVerseString == null)
			{
				verseSize += c.allVerses.length();
			}
			else
			{
				for (Enumeration e = c.verses.elements(); e.hasMoreElements(); )
				{
					String verse = (String) e.nextElement();
					// this isn't just the filler for an empty verse, so count it as data and stop looking
					if (!GoBibleCreator.EmptyVerseString.equals(verse) && verse.length() > 0)
					{
						verseSize += verse.length();
						break;
					}
				}
			}
			if (verseSize > 0)
				break;
		}
		return verseSize > 0;
	}
}

class Chapter
{
    //
	public Vector verses = new Vector();
	public StringBuffer allVerses = new StringBuffer();
	public int fileNumber;

}

