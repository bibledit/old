#!
# $1 - If -c then create directories for JAD or JAR files and create and copy them in
# otherwise only create the web pages in the numbered directories.

# Version
set version = "2.2.6"

# Create a wap directory if it doesn't already exist
set wapDir = "../wap"
mkdir -p "$wapDir/Collections/$version"

# Number each directory, numbered directories are simpler to type in the phone
# eg. http://wap.jolon.org/1
set directoryNumber = 1

echo "<title>Go Bible $version</title>" > "$wapDir/welcome.html"

foreach language ( * )
	# if file is a directory then it is a language directory that contains translations
	if (-d "$language") then
	
		#echo $language
	
		cd "$language"
		
		foreach translation (*)
			# if file is a directory and it contains a Collections.txt file
			if (-d "$translation" && -e "$translation/$translation.txt") then
				echo $translation

				echo "<a href=$directoryNumber/welcome.html>$translation</a><br/>" >> "../$wapDir/welcome.html"

				cd "$translation"

				# By default we don't need to create special directories
				# for wap jar and jads as the zip files are now unzipped
				# on the server using PHP and the existing relative
				# "MIDlet-Jar-URL: " property works fine.
				# However this option can still be used by passing in -c
				if "$1" == "-c" then
					# Create a directory for this translation's JAD and JAR files
					mkdir -p "../$wapDir/Collections/$version/$translation"
	
					# Copy in JAR and JAD files
					cp *.jar "../../$wapDir/Collections/$version/$translation"
					cp *.jad "../../$wapDir/Collections/$version/$translation"
	
					cp "../../htaccess" "../../$wapDir/Collections/$version/$translation/.htaccess"
				endif

				# Create a numbered directory for this translation to contain
				# the HTML file
				mkdir -p "../../$wapDir/$directoryNumber"
				
				# Create a HTML file for each translation
				# Create links to each jad file

				set htmlFile = "../../$wapDir/$directoryNumber/welcome.html"	
				echo "<META http-equiv='Content-Type' content='text/html; charset=UTF-8'>" > "$htmlFile"
				echo "<title>$translation $version</title>" >> "$htmlFile"
				
				# The wap translation directory can't have spaces so remove them
				set translationDir=""
				
				foreach word ($translation)
					set translationDir="${translationDir}${word}"
				end
				
				foreach jadFile (*.jad)
					#echo $jadFile
					set variant=`basename "$jadFile" ".jad"`
					set collectionName = `awk -F': ' '/MIDlet-Name/ {print $2}' "$jadFile"`
					
					echo -n '<a href="http://wap.jolon.org/Collections/' >> "$htmlFile"
					echo -n "$version/$translationDir/$jadFile" >> "$htmlFile"
					echo -n '">' >> "$htmlFile"
					echo "$collectionName</a><br/>" >> "$htmlFile"

				end
				cd ..
				
				set directoryNumber = `expr $directoryNumber + 1`
			endif
		end
		cd ..
	endif
end


