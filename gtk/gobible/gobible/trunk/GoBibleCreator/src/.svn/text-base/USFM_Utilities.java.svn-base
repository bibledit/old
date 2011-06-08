/*
 * USFM File format utility functions
 */

/**
 *
 * @author Mission Aviation Fellowship
 *         Learning Technologies Department
 * 
 * Created: September 11, 2008 for build 2.3.0
 * 
 */
//import java.util.regex.*;

public class USFM_Utilities 
{
    /*
     * Check to see if the line starts with certain
     * tags that allow us to delete the entire line
     * like '\cl A'
     * 
     * Return 'true' if we keep
     */
    public static boolean CheckIfWeKeepLine(String sTmp)
    {	
        boolean bKeep = true;
        String sMarker[] = {"\\cl","\\cp","\\cd", "\\qa", "\\sr", "\\ms", "\\mte", "\\mt", "\\mr ", "\\sr ", "\\s", "\\r ", "\\d ", "\\sp"};
        //loop through the markers removing them all from the text
        for (int i = 0; i < sMarker.length; i++)
        {
            if (sTmp.startsWith(sMarker[i]))
            {
                return false;
            }
        }
        return bKeep;
    }    


    /*
     * removes the preceeding chapter header information
     * prior to the start of the first verse
     */
    public static String RemoveChapterHeaders(String sVerse)
    {		
        //find the first verse and remove everything before that
        int iStart = sVerse.indexOf("\\v");
        sVerse = sVerse.substring(iStart);
        return sVerse;
    }    

    /*
     * This function takes a string and removes the USFM markers
     * which are singular (do not have and end tag).  Example
     * of this is the paragraph tag \p which is not needed
     */
    public static String RemoveSingularMarkerTags(String sTmp)
    {		
        //for ones that are a tag and number '\q1'
        String sTagNum[] = {"\\pi", "\\ph"};
        for (int i = 0; i < sTagNum.length; i++)
        {
            //replace tag with trailing number (trailing blank remove)
            //sTmp = sTmp.replaceAll("\\" + sTagNum[i] + "+[0-9] ", "");
            //replace tag with trailing number
            sTmp = sTmp.replaceAll("\\" + sTagNum[i] + "+[0-9]", "");
            //replace tag without trailing number (trailing blank remove)
            sTmp = sTmp.replaceAll("\\" + sTagNum[i] + " ", "");
            //replace tag without trailing number
            sTmp = sTmp.replaceAll("\\" + sTagNum[i], "");
        }
        //the list of markers that this applies to
        //NOTE: you need four '\' to account for the \p string
        //as this is a regex command
        String sMarker[] = {"\\pmo", "\\pm", "\\pmc",
        "\\pmr", "\\mi", "\\nb", "\\cls", "\\pc", "\\pr", 
        "\\qr", "\\qc", "\\pb", "\\b", "\\m", "\\p", "\\z"};
        
        //loop through the markers removing them all from the text
        for (int i = 0; i < sMarker.length; i++)
        {
            //replace the marker tag with an empty string (trailing blank remove)
            //sTmp = sTmp.replaceAll("\\" + sMarker[i] + " ", "");
            //replace the marker tag with an empty string
            sTmp = sTmp.replaceAll("\\" + sMarker[i], "");
        }
        
        //for ones that do not start with the slash
        String sSpecial[] = {"!\\$", "//"};
        for (int i = 0; i < sSpecial.length; i++)
        {
            //replace the marker tag with an empty string (trailing blank remove)
            sTmp = sTmp.replaceAll(sSpecial[i] + " ", "");
            //replace the marker tag with an empty string
            sTmp = sTmp.replaceAll(sSpecial[i], "");
        }
        
        //for ones that are a tag and number '\q1'
        String sTagNum2[] = {"\\li", "\\qm", "\\q"};
        for (int i = 0; i < sTagNum2.length; i++)
        {
            //replace tag with trailing number (trailing blank remove)
            //sTmp = sTmp.replaceAll("\\" + sTagNum2[i] + "+[0-9] ", "");
            //replace tag with trailing number
            sTmp = sTmp.replaceAll("\\" + sTagNum2[i] + "+[0-9]", "");
            //replace tag without trailing number (trailing blank remove)
            //sTmp = sTmp.replaceAll("\\" + sTagNum2[i] + " ", "");
            //replace tag without trailing number
            sTmp = sTmp.replaceAll("\\" + sTagNum2[i], "");
        }
        
        return sTmp;
    }    

    /*
     * This function takes a string and removes the USFM markers
     * which are double (have start and end tags).  Example
     * of this is the bold tag \bd...\bd* in which we want to 
     * remove the tags only but keep the text between
     */
    public static String RemoveDoubleMarkerTags(String sTmp)
    {		
        //the list of markers that this applies to
        String sMarker[] = {"\\qs", "\\qac", "\\add", "\\dc", 
        "\\nd", "\\ord", "\\pn", "\\qt", "\\sig", "\\sls", 
        "\\tl", "\\em", "\\bd", "\\it", "\\bdit", "\\no", "\\sc", "\\k"};
        //loop through the markers removing them all from the text
        for (int i = 0; i < sMarker.length; i++)
        {
            sTmp = sTmp.replaceAll("\\" + sMarker[i] + "\\*", "");
            //remove trailing blank from tag
            sTmp = sTmp.replaceAll("\\" + sMarker[i] + " ", "");                   
            sTmp = sTmp.replaceAll("\\" + sMarker[i], "");                   
        }
        return sTmp;
    }    

    /*
     * This funtion finds the USFM marker for words of Jesus
     * and replaces them with the red lettering tag
     */
    public static String ReplaceWordsOfJesus(String sTmp, char sWJ, boolean useRedLettering)
    {
        //replace the marker tag with the red lettering tag
        //NOTE: you must delete the * ending tag first before the
        //start tag otherwise you end up with only the * left
        if (useRedLettering)
        {
            //insert the red lettering character
            sTmp = sTmp.replaceAll("\\\\wj\\*", Character.toString(sWJ));            
            sTmp = sTmp.replaceAll("\\\\wj", Character.toString(sWJ));            
        }
        else
        {
            //just dump the tags
            sTmp = sTmp.replaceAll("\\\\wj\\*", "");            
            sTmp = sTmp.replaceAll("\\\\wj", "");            
        }
        return sTmp;
    }

    /*
     * This function takes a string and removes the USFM markers
     * which are double (have start and end tags).  Example
     * of this is the footnote tag \f...\f* in which we want to 
     * remove the tags and all the text between
     */
    public static String RemoveDoubleMarkerTagsFull(String sTmp)
    {		
        //the list of markers that this applies to
        //note that order is important here so that \fm is processed
        //before \f
        String sMarker[] = {"\\ca", "\\va", "\\vp", "\\fe", "\\bk", 
        "\\xdc", "\\fdc", "\\fm", "\\fig", "\\ndx", "\\pro", 
        "\\wg", "\\wh", "\\f", "\\w", "\\x", "\\rq", "\\xot", "\\xnt", "\\iqt"};
        //loop through the markers removing them all from the text
        for (int i = 0; i < sMarker.length; i++)
        {
            //find if the start tag is found
            while (sTmp.indexOf(sMarker[i]) != -1) 
            {
                int iStart = sTmp.indexOf(sMarker[i]);
                int iEnd = sTmp.indexOf(sMarker[i] + "*");
                if (iStart != -1 && iEnd != -1)
                {
                    //replace the marker tag with an empty string
                    String sFirstPart = sTmp.substring(0, iStart);
                    String sSecondPart = sTmp.substring(iEnd + sMarker[i].length() + 1);
                    //insert a space if needed here
//                    if(sFirstPart.endsWith(" ") || sSecondPart.startsWith(" "))
//                    {
//                        //blank exists between parts
//                        sTmp = sFirstPart + sSecondPart;
//                    }
//                    else
//                    {
//                        //slap a blank between the parts
//                        sTmp = sFirstPart + " " + sSecondPart;
//                    }
                    sTmp = sFirstPart + sSecondPart;
                }
                else
                {
                    //keep this from going into an endless loop
                    //if the end tag is missing
                    break;
                }
            }
        }

        return sTmp;
    }    

    /*
     * Drop the verse number from the start of the verse string
     */
    public static String RemoveVerseNumbering(String sVerse)
    {		
        //find the first verse and remove everything before that
        int iStart = sVerse.indexOf(" ");
        if (iStart == -1)
        {
            //there is no space after the verse number which happens when
            //there are multiline verses and somebody is putting in blank
            //verse lines
            sVerse = "..";
        }
        else
        {
            sVerse = sVerse.substring(iStart + 1);
        }
        return sVerse;
    }
    
    /*
     * Parse through the book name and try and normalize it to standard characters
     * which won't cause a problem with the hashmap lookup
     */
    public static String CleanBookName(String sTmp)
    {
        /*
        //try to dump string into hashmap and then retrieve what came out
        //anything that has \\ufff we can replace as being a bad character
        java.util.HashMap h = new java.util.HashMap();
        h.put(sTmp.trim(), 0);
        String sHash = h.toString();
        sHash = sHash.substring(1, sHash.length()-3);
        //look for the bad characters and replace them out
        sTmp = "";
        for (int i=0; i<sHash.length(); i++)
        {
            char c = sHash.charAt(i);
            String s = Long.toHexString((long)c).toUpperCase();
            //bad character if it has the FFF
            if (!s.startsWith("FFF")) 
            {
                sTmp += Character.toString(c);
            }
        }
        */
        return sTmp;
    }
    
}
