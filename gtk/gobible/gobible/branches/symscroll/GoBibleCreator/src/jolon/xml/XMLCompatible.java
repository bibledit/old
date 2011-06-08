// Decompiled by DJ v3.10.10.93 Copyright 2007 Atanas Neshkov  Date: 8/28/2008 1:04:50 PM
// Home Page: http://members.fortunecity.com/neshkov/dj.html  http://www.neshkov.com/dj.html - Check often for new version!
// Decompiler options: packimports(3) 
// Source File Name:   XMLCompatible.java

package jolon.xml;


// Referenced classes of package jolon.xml:
//            XMLObject

public interface XMLCompatible
{

    public abstract String getTag();

    public abstract void fromXML(XMLObject xmlobject);

    public abstract XMLCompatible addXMLChild(XMLCompatible xmlcompatible);
}