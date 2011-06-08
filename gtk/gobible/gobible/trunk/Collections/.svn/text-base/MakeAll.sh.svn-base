#!
# $1 - any parameters to be passed to GoBibleCreator such as -u

foreach language (*)
	# if file is a directory then it is a language directory that contains translations
	if (-d "$language") then
		cd "$language"
		foreach translation (*)
			# if file is a directory and it contains a Collections.txt
			# file then process it
			if (-d "$translation" && -e "$translation/$translation.txt") then
				echo $translation
				../MakeCollection.sh "$translation" $1
			endif
		end
		cd ..
	endif
end


