// Decompiled by DJ v3.10.10.93 Copyright 2007 Atanas Neshkov  Date: 8/28/2008 1:04:08 PM
// Home Page: http://members.fortunecity.com/neshkov/dj.html  http://www.neshkov.com/dj.html - Check often for new version!
// Decompiler options: packimports(3) 
// Source File Name:   XMLAttribute.java

package jolon.xml;

import java.io.*;

public class XMLAttribute
{

    public XMLAttribute()
    {
        this(null, null);
    }

    public XMLAttribute(String name, Object data)
    {
        this.name = name;
        this.data = data;
    }

    public void write(Writer writer)
    {
        try
        {
            if(data != null)
                writer.write(name + "=\"" + data + "\"");
        }
        catch(IOException e)
        {
            System.out.println(e);
        }
    }

    public String getName()
    {
        return name;
    }

    public void setName(String name)
    {
        this.name = name;
    }

    public Object getData()
    {
        return data;
    }

    public int getDataAsInt()
    {
        return Integer.parseInt(data.toString());
    }

    public double getDataAsDouble()
    {
        double value = 0.0D;
        String string = data.toString();
        try
        {
            if(string.equals("Infinity"))
                value = (1.0D / 0.0D);
            else
            if(string.equals("NaN"))
                value = 0.0D;
            else
                value = Double.valueOf(string).doubleValue();
        }
        catch(NumberFormatException e)
        {
            System.out.println(string);
            throw e;
        }
        return value;
    }

    public boolean getDataAsBoolean()
    {
        return Boolean.valueOf(data.toString()).booleanValue();
    }

    public String getDataAsString()
    {
        return data.toString();
    }

    public void setData(Object data)
    {
        this.data = data;
    }

    public String toString()
    {
        return name + "=\"" + data + "\"";
    }

    private String name;
    private Object data;
}