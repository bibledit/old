#!
# $1 - Path to Collections folder, eg. "KJV 512KB"
# $2 - other parameters eg. -u

set version=2.2.6

if (! -e ../../Release/$version) then
	mkdir -p ../../Release/$version
endif

set file = "$1"

set zipFile = "../../Release/$version/Go Bible $file $version.zip"
echo $zipFile
if (! -e "$zipFile") then
	echo "  Running GoBibleCreator..."
	java -Xmx128m -jar ../../GoBibleCreator/src/GoBibleCreator.jar $2 -d "../../source_text" "$file/$file.txt"
	cd "$file"
	echo "  Zipping..."
	zip "../$zipFile" *.jad *.jar
	cd ..
endif


