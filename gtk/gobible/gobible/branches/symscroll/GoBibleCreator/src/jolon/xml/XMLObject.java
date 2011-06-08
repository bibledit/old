// Decompiled by DJ v3.10.10.93 Copyright 2007 Atanas Neshkov  Date: 8/28/2008 12:59:06 PM
// Home Page: http://members.fortunecity.com/neshkov/dj.html  http://www.neshkov.com/dj.html - Check often for new version!
// Decompiler options: packimports(3) 
// Source File Name:   XMLObject.java

package jolon.xml;

import java.io.*;
import java.util.*;

// Referenced classes of package jolon.xml:
//            XMLAttribute, XMLCompatible

public class XMLObject
    implements XMLCompatible
{

    public XMLObject(String tag)
    {
        this.tag = tag;
        data = data;
        children = new Vector();
        attributes = new Vector();
    }

    public void add(XMLObject inObject)
    {
        children.addElement(inObject);
    }

    public void fromXML(XMLObject xmlobject)
    {
    }

    public XMLCompatible addXMLChild(XMLCompatible inObject)
    {
        children.addElement(inObject);
        return inObject;
    }

    public XMLObject getAt(int index)
    {
        return (XMLObject)children.elementAt(index);
    }

    public void add(XMLAttribute attribute)
    {
        attributes.addElement(attribute);
    }

    public void addAttribute(String name, Object data)
    {
        add(new XMLAttribute(name, data));
    }

    public void addAttribute(String name, int data)
    {
        add(new XMLAttribute(name, new Integer(data)));
    }

    public void addAttribute(String name, double data)
    {
        add(new XMLAttribute(name, new Double(data)));
    }

    public void addAttribute(String name, boolean data)
    {
        add(new XMLAttribute(name, new Boolean(data)));
    }

    public void writeXML(File file)
    {
        try
        {
            FileWriter writer = new FileWriter(file);
            writeXML(((Writer) (writer)));
            writer.close();
        }
        catch(IOException e)
        {
            e.printStackTrace();
        }
    }

    public void writeXML(Writer writer)
    {
        try
        {
            writer.write("<?xml version=\"1.0\"?>\n");
            write(writer, 0);
        }
        catch(IOException e)
        {
            System.out.println(e);
        }
    }

    protected void write(Writer writer, int level)
        throws IOException
    {
        writeTabs(writer, level);
        writer.write("<" + tag);
        writeAttributes(writer);
        if(children.size() > 0 || data != null)
        {
            writer.write(">\n");
            for(Enumeration e = children.elements(); e.hasMoreElements(); ((XMLObject)e.nextElement()).write(writer, level + 1));
            if(data != null)
            {
                writeTabs(writer, level + 1);
                writer.write(data.toString());
            }
            writeTabs(writer, level);
            writer.write("</" + tag + ">" + "\n");
        } else
        {
            writer.write("/>\n");
        }
    }

    protected void writeTabs(Writer writer, int level)
        throws IOException
    {
        for(int i = 0; i < level; i++)
            writer.write("\t");

    }

    protected void writeAttributes(Writer writer)
        throws IOException
    {
        for(Enumeration e = attributes.elements(); e.hasMoreElements(); ((XMLAttribute)e.nextElement()).write(writer))
            writer.write(" ");

    }

    public void writeWBXML(File file)
    {
        try
        {
            OutputStream outputStream = new BufferedOutputStream(new FileOutputStream(file));
            writeWBXML(outputStream);
            outputStream.close();
        }
        catch(IOException e)
        {
            e.printStackTrace();
        }
    }

    public void writeWBXML(OutputStream outputStream)
    {
        try
        {
            writeWBXMLversion(outputStream);
            writeWBXMLpublicID(outputStream);
            writeWBXMLcharSet(outputStream);
            Hashtable stringTable = writeWBXMLstringTable(outputStream);
            writeWBXML(outputStream, stringTable);
        }
        catch(IOException e)
        {
            System.out.println(e);
        }
    }

    public void writeWBXMLversion(OutputStream outputStream)
        throws IOException
    {
        outputStream.write(1);
    }

    public void writeWBXMLpublicID(OutputStream outputStream)
        throws IOException
    {
        outputStream.write(0);
    }

    public void writeWBXMLcharSet(OutputStream outputStream)
        throws IOException
    {
        outputStream.write(0);
    }

    public Hashtable writeWBXMLstringTable(OutputStream outputStream)
        throws IOException
    {
        Hashtable stringTable = new Hashtable();
        stringTable.put("Jolon's public id", new Integer(0));
        generateStringTable(stringTable);
        int totalLength = 0;
        for(Enumeration e = stringTable.keys(); e.hasMoreElements();)
        {
            String key = (String)e.nextElement();
            stringTable.put(key, new Integer(totalLength));
            totalLength += key.length() + 1;
        }

        writeWBXMLmultibyteInteger(outputStream, totalLength);
        System.out.println("String table length = " + totalLength);
        String key;
        for(Enumeration e = stringTable.keys(); e.hasMoreElements(); writeWBXMLnullTerminatedString(outputStream, key))
            key = (String)e.nextElement();

        return stringTable;
    }

    public void generateStringTable(Hashtable stringTable)
    {
        stringTable.put(tag, new Integer(0));
        if(!attributes.isEmpty())
        {
            Enumeration e = attributes.elements();
            do
            {
                if(!e.hasMoreElements())
                    break;
                XMLAttribute attribute = (XMLAttribute)e.nextElement();
                stringTable.put(attribute.getName(), new Integer(0));
                if(!(attribute.getData() instanceof Double))
                    stringTable.put(attribute.getDataAsString(), new Integer(0));
            } while(true);
        }
        if(!children.isEmpty())
        {
            XMLObject xml;
            for(Enumeration e = children.elements(); e.hasMoreElements(); xml.generateStringTable(stringTable))
                xml = (XMLObject)e.nextElement();

        }
    }

    public void writeWBXMLmultibyteInteger(OutputStream outputStream, int value)
        throws IOException
    {
        if(value == 0)
        {
            outputStream.write(0);
        } else
        {
            int bitsLeft = 28;
            boolean start = false;
            do
            {
                int bits = value >> bitsLeft & 0x7f;
                if(!start && bits > 0)
                    start = true;
                if(start)
                {
                    if(bitsLeft > 0)
                        bits |= 0x80;
                    outputStream.write(bits);
                }
            } while((bitsLeft -= 7) >= 0);
        }
    }

    public void writeWBXMLnullTerminatedString(OutputStream outputStream, String string)
        throws IOException
    {
        outputStream.write(string.getBytes(), 0, string.length());
        outputStream.write(0);
    }

    public void writeWBXML(OutputStream outputStream, Hashtable stringTable)
        throws IOException
    {
        int tokenCode = 4;
        if(!attributes.isEmpty())
            tokenCode |= 0x80;
        if(!children.isEmpty())
            tokenCode |= 0x40;
        outputStream.write(tokenCode);
        writeWBXMLstringIndex(outputStream, stringTable, tag);
        if(!attributes.isEmpty())
        {
            for(Enumeration e = attributes.elements(); e.hasMoreElements();)
            {
                XMLAttribute attribute = (XMLAttribute)e.nextElement();
                outputStream.write(4);
                writeWBXMLstringIndex(outputStream, stringTable, attribute.getName());
                Object data = attribute.getData();
                if(data instanceof Double)
                {
                    Double doubleObject = (Double)data;
                    int bits = Float.floatToIntBits(doubleObject.floatValue());
                    outputStream.write(81);
                    writeWBXMLmultibyteInteger(outputStream, bits);
                } else
                {
                    outputStream.write(83);
                    writeWBXMLstringIndex(outputStream, stringTable, attribute.getDataAsString());
                }
            }

            outputStream.write(1);
        }
        if(!children.isEmpty())
        {
            XMLObject xml;
            for(Enumeration e = children.elements(); e.hasMoreElements(); xml.writeWBXML(outputStream, stringTable))
                xml = (XMLObject)e.nextElement();

            outputStream.write(1);
        }
    }

    public void writeWBXMLstringIndex(OutputStream outputStream, Hashtable stringTable, String string)
        throws IOException
    {
        int index = ((Integer)stringTable.get(string)).intValue();
        writeWBXMLmultibyteInteger(outputStream, index);
    }

    public String getTag()
    {
        return tag;
    }

    public void setTag(String inTag)
    {
        tag = inTag;
    }

    public Object getData()
    {
        return data;
    }

    public void setData(Object inData)
    {
        data = inData;
    }

    public Enumeration getChildren()
    {
        return children.elements();
    }

    public Enumeration getAttributes()
    {
        return attributes.elements();
    }

    public XMLObject getChild(String inTag)
    {
        for(Enumeration e = children.elements(); e.hasMoreElements();)
        {
            XMLObject currentChild = (XMLObject)e.nextElement();
            if(currentChild.getTag().equalsIgnoreCase(inTag))
                return currentChild;
        }

        return null;
    }

    public boolean hasChild(String inTag)
    {
        return getChild(inTag) != null;
    }

    public XMLCompatible getXMLChild(String inTag)
    {
        for(Enumeration e = children.elements(); e.hasMoreElements();)
        {
            XMLCompatible currentChild = (XMLCompatible)e.nextElement();
            if(currentChild.getTag().equalsIgnoreCase(inTag))
                return currentChild;
        }

        return null;
    }

    public XMLAttribute getAttribute(String name)
    {
label0:
        {
            if(name == null)
                break label0;
            Enumeration e = attributes.elements();
            XMLAttribute currentAttribute;
            String attributeName;
            do
            {
                do
                {
                    if(!e.hasMoreElements())
                        break label0;
                    currentAttribute = (XMLAttribute)e.nextElement();
                } while(currentAttribute == null);
                attributeName = currentAttribute.getName();
            } while(attributeName == null || !attributeName.equalsIgnoreCase(name));
            return currentAttribute;
        }
        return null;
    }

    public XMLObject getChildAt(int index)
    {
        return (XMLObject)children.elementAt(0);
    }

    public int getNumberOfChildren()
    {
        return children.size();
    }

    public boolean hasChildren()
    {
        return children.size() > 0;
    }

    public String toString()
    {
        StringBuffer string = new StringBuffer();
        string.append(tag + " : ");
        if(data != null)
            string.append(data.toString().trim() + " ");
        Enumeration e = children.elements();
        if(e.hasMoreElements())
        {
            string.append("{");
            for(; e.hasMoreElements(); string.append(" ]"))
            {
                string.append(" [ ");
                string.append(((XMLObject)e.nextElement()).toString());
            }

            string.append("}");
        }
        e = attributes.elements();
        if(e.hasMoreElements())
        {
            string.append("{");
            for(; e.hasMoreElements(); string.append(" ]"))
            {
                string.append(" [ ");
                string.append(e.nextElement().toString());
            }

            string.append("}");
        }
        return string.toString();
    }

    public String getAttribute(String attributeName, String defaultValue)
    {
        XMLAttribute attribute = getAttribute(attributeName);
        String value = defaultValue;
        if(attribute != null)
            value = attribute.getDataAsString();
        return value;
    }

    public int getAttribute(String attributeName, int defaultValue)
    {
        XMLAttribute attribute = getAttribute(attributeName);
        int value = defaultValue;
        if(attribute != null)
            value = attribute.getDataAsInt();
        return value;
    }

    public double getAttribute(String attributeName, double defaultValue)
    {
        XMLAttribute attribute = getAttribute(attributeName);
        double value = defaultValue;
        if(attribute != null)
            value = attribute.getDataAsDouble();
        return value;
    }

    public boolean getAttribute(String attributeName, boolean defaultValue)
    {
        XMLAttribute attribute = getAttribute(attributeName);
        boolean value = defaultValue;
        if(attribute != null)
            value = attribute.getDataAsBoolean();
        return value;
    }

    public String getAttributeAsString(String attributeName, String defaultValue)
    {
        XMLAttribute attribute = getAttribute(attributeName);
        String value = defaultValue;
        if(attribute != null)
            value = attribute.getDataAsString();
        return value;
    }

    public int getAttributeAsInt(String attributeName, int defaultValue)
    {
        XMLAttribute attribute = getAttribute(attributeName);
        int value = defaultValue;
        if(attribute != null)
            value = attribute.getDataAsInt();
        return value;
    }

    public double getAttributeAsDouble(String attributeName, double defaultValue)
    {
        XMLAttribute attribute = getAttribute(attributeName);
        double value = defaultValue;
        if(attribute != null)
            value = attribute.getDataAsDouble();
        return value;
    }

    public boolean getAttributeAsBoolean(String attributeName, boolean defaultValue)
    {
        XMLAttribute attribute = getAttribute(attributeName);
        boolean value = defaultValue;
        if(attribute != null)
            value = attribute.getDataAsBoolean();
        return value;
    }

    String tag;
    Object data;
    Vector children;
    Vector attributes;
    public static final int END = 1;
    public static final int LITERAL = 4;
    public static final int EXT_T_1 = 81;
    public static final int STR_T = 83;
}