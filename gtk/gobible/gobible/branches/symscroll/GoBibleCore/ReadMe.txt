GoBibleCore Build Instructions
==============================

Ant is used to build GoBibleCore using the build.xml file. Some additional libraries must first be acquired before GoBibleCore will successfully build. These are:

- Antenna
- WTK
- Microemulator

Antenna is an extension to Ant which provides tasks specific for JavaME application development. It can be downloaded from: http://antenna.sourceforge.net/

Place the antenna JAR file into the lib subdirectory and modify the taskdef tag in the build.xml file to point to the JAR, eg:

<taskdef resource="antenna.properties" classpath="lib/antenna-bin-0.9.13.jar"/>

The next line in the build.xml file sets the home directory of your WTK installation, by default I have set it to the path to MPowerPlayer on Mac OS X, however you may want to set it to the WTK directory. Either way you need either Sun's WTK or MPowerPlayer to build GoBibleCore.

GoBibleCore uses the NokiaUI API and therefore needs the class definitions in the API. I use the Nokia UI and WMA jar files provided by Microemulator. You can change these to something else if you like, either way you will probably need to modify the paths to point to your Microemulator installation directory.

Once these libraries have been set up you can simply build by typing 'ant' at the command line (without the single quotes).

