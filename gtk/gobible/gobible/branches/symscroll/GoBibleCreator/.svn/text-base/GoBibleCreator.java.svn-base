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
 * Creates Go Bible JAR files from XML Bible texts. Supports OSIS, ThML and
 * XHTML-TE formats. See OsisConverter, ThmlConverter and XhtmlTeConverter
 * classes for format specific information.
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
    public final static String SUB_VERSION = "4.0";	// "3.6";

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

    //the USFM file codepage for the source files
    // introduced in version 2.3
    protected static String fileCodepage = null;
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

    /* attributes deleted: (put them elsewhere)
     * MIDlet-INfo_URL
     * MIDlet-Vendor
     * Info-string
     */

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
    /**
     * This will be prepended to the Source-Text path in the Collections.txt file.
     * Can be set through the -d parameter.
     */
    protected static String baseSourceDirectory = null;

    // the file path (relative to the baseSource directory) to an alternate
    // icon that will display on the phone
    // introduced in version 2.3
    protected static String phoneIconFilepath = null;
    // alternate name for the application that will be displayed in the
    // phone's title
    // introduced in version 2.3
    protected static String applicationName = null;
    // flag to determine if red lettering is used
    // introduced in version 2.3
    protected static boolean useRedLettering = true; /* TODO: duplicate */
    public enum SourceFormatType
    {
        osis,
        thml,
        usfm,
        xhtml_te,
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
     * the second argument is the XML file (OSIS, ThML, or XHTML-TE),
     * and the third argument is the collections file as specified above.
     **/
    public static void main(String args[]) throws IOException
    {
        System.out.println("----------------------------------");
        System.out.println("GoBibleCreator Version: " + versionString);
        System.out.println("----------------------------------");

        initializeOptions(); // since we don't have a constructor...
        String paramOptionMap[][] = {
            { "-b", "GBCoreBinaryPath" },
            { "-m", "ManifestFilePath" },
            { "-p", "UIFilePath" },
            { "-dh", null},
            { "-u", null},
            { "-d", "BaseSourceDirectory" }
        };

        if (args.length < 1)
        {
                System.out.println("Usage: (java -jar GoBibleCreator) [ options ] CollectionsFilePath");
                System.out.println("\tOptions:");
                System.out.println("\t-b (path) - Path to GoBibleCore binaries (.jar)");
                System.out.println("\t-m (path) - Path to Manifest file template");
                System.out.println("\t-p (path) - Path to ui.properties template");
                System.out.println("\t-dh - Show some debug output");
                System.out.println("\t-u - Update only");
                System.out.println("\t-d - Set base source text directory");
        }
        else
        {
            String updateOption = null;
            int i;
            
            for (i=0; i<args.length; i++)
            {
                String arg = args[i];
                if (updateOption != null) {
                    programOptions.put(updateOption, arg);
                    updateOption = null;
                }
                else {
                    boolean matched = false;
                    for (String[] paramOption : paramOptionMap) {
                        if (arg.compareToIgnoreCase(paramOption[0]) == 0)
                        {
                            matched = true;
                            if (paramOption[1] == null) {
                                programOptions.put(paramOption[0], "");
                            }
                            else {
                                updateOption = paramOption[1];
                            }
                            break;
                        }
                    }
                    if (!matched) {
                        break;
                    }
                }
            }
            if (programOptions.containsKey("-dh"))
            {
                // show some internal debug context information
                String properties[] = {
                    "java.version",
                    "java.vendor",
                    "java.class.path",
                    "os.name",
                    "os.arch",
                    "os.version",
                    "user.dir"};
                System.out.println("***************************************************");
                System.out.println("*** Listing some debug type information here:");
                try
                {
                        for (String property: properties)
                        {
                                System.out.println("*** "+property+" ==> " + System.getProperty(property));
                        }
                }
                catch (Exception e) {}
                System.out.println("***************************************************");
                System.out.println();
            }
            if (programOptions.containsKey("-u"))
            {
                // Update-only flag has been specified which means we won't be
                // parsing the source text or updating the text in the JAR file.
                // Instead we will be only modifying the existing JAR/JAD files
                // which should already exist
                updateOnly = true;
                System.out.println(" ** The \"Update-only\"(-u) flag has been found: updating the JAR/JAD files.");
                System.out.println(" ** The original JAR files[s] are renamed with the file extension .TMP");
            }
            if (programOptions.containsKey("BaseSourceDirectory"))
            {
                    // This argument is the base directory for Source-Text, we can
                    // switch the flag off now and considered the other arguments
                    // as normal.
                    baseSourceDirectory = programOptions.get("BaseSourceDirectory");
                    System.out.println(" ** Using <"+baseSourceDirectory+"> as the base Source Directory.");
            }
            System.out.println(" ** Using the GoBibleCore binary:\n\t<" + programOptions.get("GBCoreBinaryPath") + ">");
            System.out.println(" ** Using the Manifest file:\n\t<" + programOptions.get("ManifestFilePath") + ">");
            System.out.println(" ** Using the UI Properties file:\n\t<" + programOptions.get("UIFilePath") + ">");

            
            for (; i<args.length; i++) {
                // If a thml, xml, or xhtml_te file has been specified then generate a
                // collections file from it and do nothing else, otherwise we assume a
                // collections file has been specified and will generate the collections
                // from it.
                String arg=args[i];

                if ((arg.toLowerCase().endsWith(".xml")
                    || arg.toLowerCase().endsWith(".thm")
                    || arg.toLowerCase().endsWith(".thml")
                    || arg.toLowerCase().endsWith(".xhtml")))
                {
                    // No Collections file specified so generate
                    // Collections file from XML
                    File xmlFile = new File(args[0]);
                    HashMap books = XMLConverter.getConverter(xmlFile).parse();

                    if (books != null)
                    {
                            XMLConverter.generateCollectionsFile(new File(xmlFile.getParent(), "Collections.txt"), xmlFile.getName(), books);
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
	 * was added so the UpdateOnly path can also benefit from the relative
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

        private static HashMap<String,String> programOptions = null;
        private static HashMap<String,String> goBibleOptions = new HashMap();
        
        public static void initializeOptions() {
            File jarDirectory = getJarDirectory();
            if (programOptions == null) {
                programOptions = new HashMap();
            }
            
            programOptions.put("ManifestFilePath", new File(jarDirectory, "GoBibleCore/MANIFEST.MF").getAbsolutePath());
            programOptions.put("GBCoreBinaryPath",  new File(jarDirectory, "GoBibleCore/GoBibleCore.jar").getAbsolutePath());
            programOptions.put("UIFilePath", new File(jarDirectory, "GoBibleCore/ui.properties").getAbsolutePath());

            if (goBibleOptions == null) {
                goBibleOptions = new HashMap();
            }
            goBibleOptions.put("Go-Bible-Align", "left");
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
                    || sourceFormatType == SourceFormatType.thml
                    || sourceFormatType == SourceFormatType.xhtml_te)
                {
                    File xmlFile = new File(baseSourceDirectory, sourceTextPath);

                    XMLConverter xc = XMLConverter.getConverter(xmlFile);

                    books = xc.parse();
                }
                else if (sourceFormatType == SourceFormatType.usfm)
                {
                    USFMConverter uc = new USFMConverter
                            (collectionsFile,
                            baseSourceDirectory,
                            sourceTextPath);
                    books = uc.parse();
                    System.err.println("Books size is " + books.size());
                }
                else
                {
                    //dump out message saying that it cannot determine the file format type
                    System.out.println("Error: Could not determine Bible format type, " +
                            sourceFormatType + ".  Please use the 'Source-Format'");
                    System.out.println("property in your collections file.");
                    return;
                }
            }
            else // !updateOnly
            {
                    // Allow the relative path in the collections file to work for updateing
                    // the icon as well.
                    UpdateBaseSourceDirectory(collectionsFile);
            }

            if (books != null || updateOnly)
            {
                parseUiProperties();

                /* TODO: Collections file is parsed AFTER files are interpreted?
                 * HOw does it affect the fileCodePage?
                 */
                // Parse the collections file
                Vector collections = parseCollectionsFile(collectionsFile, books);

                if (customFontString != null)
                {
                        generateCustomFont(customFontString, collectionsFile, books);
                }
                
                // Work out the current directory of the GoBibleCreator.jar
                File jarFile = new File(programOptions.get("GBCoreBinaryPath"));
                File manifestFile = new File(programOptions.get("ManifestFilePath"));

                JarFile goBibleJar = new JarFile(jarFile);

                // Write out the Go Bible data files into the same directory as the collections file
                writeCollections(collectionsFile.getParentFile(), collections, books, goBibleJar, manifestFile);
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
		
		return new File(".");
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
			System.err.println("The source text is either a ThML, OSIS or XHTML-TE XML file.");
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
                else if (sFormat.equals("xhtml_te"))
                    { srcFormatType = SourceFormatType.xhtml_te; }
				else
					{ srcFormatType = SourceFormatType.unknown; }
				break;
           	}
		}
		reader.close();
		return srcFormatType;
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
                    if (lineIn.indexOf(XhtmlTeConverter.XHTML_TAG.toLowerCase()) > 0)
                    {
                        srcFormatType= SourceFormatType.xhtml_te;
                        break;
                    }
                    iRowCount++;
                }
                readerIn.close();
            }
            catch(Exception e)
            {
                System.out.println("Error: " + e.getMessage());
                e.printStackTrace();
            }
            return srcFormatType;
        }

	/**
	 * Parses the English UI properties file. The English definitions
	 * can be overridden in the Collections file.
	 */
	public static void parseUiProperties() throws IOException
	{
                
        File uiFile = new File(getJarDirectory(), programOptions.get("UIFilePath"));
        boolean bRet = uiFile.exists();
        String sFileExists = "False";
        if (bRet)
            sFileExists = "True";
            System.out.println(uiFile.getAbsolutePath() + " exists:" + sFileExists);
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
                    else if(line.startsWith("#") ||line.startsWith("//") || line.startsWith("rem") || line.startsWith("REM"))
                    {
                            //do nothing - ignore line
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
                                    goBibleOptions.put("Go-Bible-Align", alignString);
				}
				else if (alignString.equals("right"))
				{
                                    goBibleOptions.put("Go-Bible-Align", alignString);
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
                                    bookName = USFMConverter.CleanBookName(bookName);
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
	 * @param collections Vector of collections that have been read from the collections file.
	 * @param books HashMap of books that have been read from the XML file.
	 * @param goBibleJar JAR containing the Go Bible classes.
	 **/
	public static void writeCollections(File directory, Vector collections, HashMap books, JarFile goBibleJar, File manifestFile) throws IOException
	{
		/*StringBuffer wapPage = new StringBuffer();
	
		wapPage.append("<html>\n<head>\n<title>Go Bible</title>\n</head>\n<body>\n\n");*/
	
		for (Enumeration e = collections.elements(); e.hasMoreElements(); )
		{
			Collection collection = (Collection) e.nextElement();
			
			System.out.println("Writing Collection " + collection.fileName + ": ");
			
			writeCollection(directory, collection, books, goBibleJar, manifestFile/*, wapPage*/);
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
	public static void writeCollection(File directory, Collection collection, HashMap books, JarFile goBibleJar, File manifestFile/*, StringBuffer wapPage*/) throws IOException
	{
		// Create a manifest file for the new JAR
		Manifest manifest = null;
                try {
                    System.out.println(manifestFile.getCanonicalFile().getAbsolutePath());
                    manifest = new Manifest(new FileInputStream(manifestFile));
                    System.out.println("Manifest data found");
                }
                catch (IOException ioe) {
                    ioe.printStackTrace(System.out);
                    manifest = new Manifest();
                    System.out.println("No manifest data found");
                }
		
		Attributes attributes = manifest.getMainAttributes();
		attributes.putValue("Manifest-Version", "1.0");

                // prepare the name for this application
                String s = NAME_APPENDAGE;
                if (applicationName != null) {
                    s = applicationName;
                }
                goBibleOptions.put("MIDlet-Name", collection.name + s);
                goBibleOptions.put("MIDlet-1", collection.name + s + ", Icon.png, GoBible");
                
                // put in program options, in case they have been amended
                Set<String> optionsSet = goBibleOptions.keySet();
                for (String o : optionsSet) {
                    attributes.putValue(o, goBibleOptions.get(o));
                }

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
		createJadFile(directory, collection, jarFile.length(), collection.fileName + ".jar", attributes);
		
		if (wapSite != null)
		{
			File wapDirectory = new File(directory, "wap");
			
			if (!wapDirectory.exists())
			{
				wapDirectory.mkdir();
			}	

			// Create the JAD file for the WAP site
			createJadFile(
                                wapDirectory,
                                collection,
                                jarFile.length(),
                                wapSite + "/" + directory.getName() + "/" + collection.fileName + ".jar",
                                attributes);
			
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

		System.out.println(collection.books.size() + " book(s) written to " + jarFile);
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
         * @param attr Re-use the attributes found in the Manifest.
	 **/
	public static void createJadFile
                (File directory,
                Collection collection,
                long jarFileLength,
                String url,
                Attributes attr)
                throws IOException
	{
		// Create a JAD file
		
		File jadFile = new File(directory, collection.fileName + ".jad");

		PrintWriter writer = new PrintWriter(new BufferedWriter(new OutputStreamWriter(new FileOutputStream(jadFile), "UTF-8")));
		
                // retrieve and write values
                Set s = attr.keySet();
                for (Object key: s) {
                    writer.print(key.toString());
                    writer.print(": ");
                    writer.print(attr.getValue(key.toString()));
                    writer.println();
                }
		writer.println("MIDlet-Jar-Size: " + jarFileLength);
		writer.println("MIDlet-Jar-URL: " + url);
                
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
}
