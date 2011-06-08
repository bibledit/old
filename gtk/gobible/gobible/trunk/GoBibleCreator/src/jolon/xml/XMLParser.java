// Decompiled by DJ v3.10.10.93 Copyright 2007 Atanas Neshkov  Date: 8/28/2008 1:05:26 PM
// Home Page: http://members.fortunecity.com/neshkov/dj.html  http://www.neshkov.com/dj.html - Check often for new version!
// Decompiler options: packimports(3) 
// Source File Name:   XMLParser.java

package jolon.xml;

import java.io.*;
import java.util.Hashtable;
import java.util.Stack;

// Referenced classes of package jolon.xml:
//            XMLObject, XMLCompatible, CDATA, XMLAttribute

public class XMLParser
{

    public static XMLObject parse(String fileName)
        throws IOException
    {
        XMLParser parser = new XMLParser();
        return parser.parse(new File(fileName));
    }

    public static XMLObject parseFile(File file)
        throws IOException
    {
        XMLParser parser = new XMLParser();
        return parser.parse(file);
    }

    public XMLParser(Reader inputStream)
    {
        EOF = false;
        this.inputStream = new LineNumberReader(inputStream);
    }

    public XMLParser()
    {
        EOF = false;
    }

    public int getLineNumber()
    {
        int lineNumber = -1;
        if(inputStream != null)
            lineNumber = inputStream.getLineNumber();
        return lineNumber;
    }

    public XMLObject parse(File file)
        throws IOException
    {
        return parse(file, null);
    }

    public XMLObject parse(File file, Hashtable knownTags)
        throws IOException
    {
        String fileName = file.getName();
        XMLObject xml = null;
        if(fileName.endsWith(".wbxml"))
        {
            InputStream inputStream = new BufferedInputStream(new FileInputStream(file));
            xml = parseWBXML(inputStream);
            inputStream.close();
        } else
        {
            this.inputStream = new LineNumberReader(new InputStreamReader(new FileInputStream(file), "UTF-8"));
            xml = parse(knownTags);
        }
        return xml;
    }

    public XMLObject parse()
    {
        return parse((Hashtable)null);
    }

    public XMLObject parse(Hashtable knownTags)
    {
        tagStack = new Stack();
        XMLCompatible parent = new XMLObject("root");
        currentChar = ' ';
        EOF = false;
        do
        {
            if(EOF)
                break;
            String currentTag = readTag(parent);
            if(currentTag.length() > 0 && !isXMLTag(currentTag))
                if(isEndTag(currentTag))
                {
                    String tagString = parent.getTag();
                    if(currentTag.regionMatches(1, tagString, 0, tagString.length()))
                        parent = (XMLCompatible)tagStack.pop();
                    else
                        System.out.println("Error: Line: " + (inputStream.getLineNumber() + 1) + ": End tag <" + currentTag + "> does not have a matching start tag (found <" + tagString + "> instead), ignoring.");
                } else
                {
                    if(!currentTag.endsWith("/"))
                        tagStack.push(parent);
                    XMLCompatible tempObject = createXMLObject(currentTag);
                    if(knownTags != null && knownTags.containsKey(tempObject.getTag()))
                        try
                        {
                            XMLCompatible realObject = (XMLCompatible)((Class)knownTags.get(tempObject.getTag())).newInstance();
                            realObject.fromXML((XMLObject)tempObject);
                            tempObject = realObject;
                        }
                        catch(IllegalAccessException e)
                        {
                            e.printStackTrace();
                        }
                        catch(InstantiationException e)
                        {
                            e.printStackTrace();
                        }
                    tempObject = parent.addXMLChild(tempObject);
                    if(!currentTag.endsWith("/"))
                        parent = tempObject;
                }
        } while(true);
        return (XMLObject)parent;
    }

    public String readTag(XMLCompatible parent)
    {
        StringBuffer tag = new StringBuffer();
        try
        {
            int current = 0;
            String cdata = "";
            char currentChar;
            do
            {
                current = inputStream.read();
                currentChar = (char)current;
                if(current != -1 && currentChar != '<')
                    cdata = cdata + currentChar;
            } while(current != -1 && currentChar != '<');
            if(!cdata.trim().equals(""))
                parent.addXMLChild(new CDATA(cdata));
            boolean inquote = currentChar == '"';
            do
            {
                if(current == -1 || currentChar == '>' && !inquote)
                    break;
                current = inputStream.read();
                currentChar = (char)current;
                if(currentChar == '"')
                    inquote = !inquote;
                if((currentChar != '>' || inquote) && current != -1)
                    tag.append(currentChar);
            } while(true);
            if(current == -1)
                EOF = true;
        }
        catch(IOException e)
        {
            System.out.println("An IO error has occurred in XMLParser.readTag().");
        }
        return tag.toString();
    }

    protected XMLObject createXMLObject(String tag)
    {
        int index = findEndOfTagName(tag);
        String tagName = tag.substring(0, index);
        XMLObject xmlObject = new XMLObject(tagName);
        getAttributes(xmlObject, tag, index);
        return xmlObject;
    }

    protected int findEndOfTagName(String tag)
    {
        int currentIndex = 0;
        int length = tag.length();
        int index;
        for(index = length; currentIndex < length && index == length; currentIndex++)
        {
            char currentChar = tag.charAt(currentIndex);
            if(isWhiteSpace(currentChar) || currentChar == '>' || currentChar == '/')
                index = currentIndex;
        }

        return index;
    }

    public boolean isWhiteSpace(char inChar)
    {
        return inChar == ' ' || inChar == '\t' || inChar == '\n' || inChar == '\r';
    }

    protected void getAttributes(XMLObject xmlObject, String tag, int index)
    {
        do
        {
            XMLAttribute xmlAttribute = new XMLAttribute();
            index = getAttribute(tag, index + 1, xmlAttribute);
            if(index != -1)
                xmlObject.add(xmlAttribute);
        } while(index != -1);
    }

    protected int getAttribute(String tag, int index, XMLAttribute xmlAttribute)
    {
        int currentIndex = index;
        char currentChar = '/';
        int length = tag.length();
        boolean whitespace = true;
        if(index < length)
            currentChar = tag.charAt(index);
        else
            return -1;
        if(index < length && currentChar != '/' && currentChar != '>')
        {
            int nameIndex = tag.indexOf('=', index);
            if(nameIndex == -1)
                return -1;
            xmlAttribute.setName(tag.substring(index, nameIndex).trim());
            int valueStart = tag.indexOf('"', nameIndex);
            if(valueStart == -1 || valueStart + 1 >= length)
                return -1;
            int valueEnd = tag.indexOf('"', valueStart + 1);
            if(valueEnd == -1)
                return -1;
            xmlAttribute.setData(tag.substring(valueStart + 1, valueEnd));
            index = valueEnd;
        } else
        {
            index = -1;
        }
        return index;
    }

    public boolean isXMLTag(String inTag)
    {
        return inTag.startsWith("?") && inTag.endsWith("?");
    }

    public boolean isEndTag(String tag)
    {
        return tag.charAt(0) == '/';
    }

    public XMLObject parseWBXML(InputStream inputStream)
        throws IOException
    {
        XMLObject xml = new XMLObject("root");
        Hashtable stringTable = readStart(inputStream);
        parseWBXML(xml, inputStream, stringTable);
        return xml;
    }

    public boolean parseWBXML(XMLObject parent, InputStream inputStream, Hashtable stringTable)
        throws IOException
    {
        int tokenCode = inputStream.read();
        if(tokenCode != 1)
        {
            String tag = readIndexedString(inputStream, stringTable);
            XMLObject xml = new XMLObject(tag);
            if((tokenCode & 0x80) != 0)
            {
                int attributeCode = inputStream.read();
                do
                {
                    String attributeName = null;
                    if(attributeCode == 4)
                        attributeName = readIndexedString(inputStream, stringTable);
                    else
                        throw new IOException("attributeName is null");
                    attributeCode = inputStream.read();
                    Object data = null;
                    if(attributeCode == 83)
                    {
                        data = readIndexedString(inputStream, stringTable);
                        xml.addAttribute(attributeName, data);
                    } else
                    if(attributeCode == 81)
                    {
                        int value = readMultibyteInteger(inputStream);
                        xml.addAttribute(attributeName, Float.intBitsToFloat(value));
                    }
                    attributeCode = inputStream.read();
                } while(attributeCode != 1);
            }
            if((tokenCode & 0x40) != 0)
            {
                int i = 0;
                while(parseWBXML(xml, inputStream, stringTable)) ;
            }
            parent.add(xml);
            return true;
        } else
        {
            return false;
        }
    }

    public String readIndexedString(InputStream inputStream, Hashtable stringTable)
        throws IOException
    {
        int index = readMultibyteInteger(inputStream);
        return (String)stringTable.get(new Integer(index));
    }

    public Hashtable readStart(InputStream inputStream)
        throws IOException
    {
        System.out.println("readStart");
        readVersion(inputStream);
        readPublicID(inputStream);
        readCharSet(inputStream);
        System.out.println("readStringTable");
        return readStringTable(inputStream);
    }

    public void readVersion(InputStream inputStream)
        throws IOException
    {
        inputStream.read();
    }

    public void readPublicID(InputStream inputStream)
        throws IOException
    {
        inputStream.read();
    }

    public void readCharSet(InputStream inputStream)
        throws IOException
    {
        inputStream.read();
    }

    public Hashtable readStringTable(InputStream inputStream)
        throws IOException
    {
        Hashtable stringTable = new Hashtable();
        int length = readMultibyteInteger(inputStream);
        System.out.println("length = " + length);
        String string;
        for(int offset = 0; offset < length; offset += string.length() + 1)
        {
            string = readNullTerminatedString(inputStream);
            stringTable.put(new Integer(offset), string);
        }

        return stringTable;
    }

    public int readMultibyteInteger(InputStream inputStream)
        throws IOException
    {
        int currentByte = inputStream.read();
        if(currentByte == -1)
            throw new IOException("End of file");
        int value;
        for(value = currentByte & 0x7f; (currentByte & 0x80) != 0; value |= currentByte & 0x7f)
        {
            currentByte = inputStream.read();
            if(currentByte == -1)
                throw new IOException("End of file");
            value <<= 7;
        }

        return value;
    }

    public String readNullTerminatedString(InputStream inputStream)
        throws IOException
    {
        String result = "";
        for(int currentByte = inputStream.read(); currentByte != 0; currentByte = inputStream.read())
            result = result + (char)currentByte;

        return result;
    }

    LineNumberReader inputStream;
    Stack tagStack;
    char currentChar;
    boolean EOF;
}