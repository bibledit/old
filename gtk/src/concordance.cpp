#include <glibmm/ustring.h>
#include <glibmm/iochannel.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include "directories.h"
#include "settings.h"
#include "books.h"
#include "localizedbooks.h"
#include "versifications.h"
#include "versification.h"
#include "mappings.h"
#include "styles.h"
#include "urltransport.h"
#include "vcs.h"
#include "projectutils.h"
#include "usfmtools.h"
#include "usfm-inline-markers.h"
#include "bookdata.h"

using namespace std;
using namespace Glib;

// Forward declarations
class bible;
class chapter;
class verse;

class book {
  public:
	bible *bbl;  // back pointer to the containing bible
	ustring bookname;
	vector<chapter> chapters;
  public:
	book(bible *_bbl, const ustring &_bknm);
};

book::book(bible *_bbl, const ustring &_bknm)
{
	bbl = _bbl;
	bookname = _bknm;
}

class chapter {
  public:
	book *bk;    // back pointer to the containing book
	int chapnum;     // the chapter number
	vector<verse> verses;
	// project, book, chapter (57 = Philemon, 1 = chapter 1)
  public:
    chapter(book *_bk, int _nm);
	void load(int book, int chapter);
};

chapter::chapter(book *_bk, int _nm)
{
	bk = _bk;
	chapnum = _nm;
}

class verse {
	chapter *ch; // back pointer to the containing chapter
	int vs;
	ustring text;
  public:
	verse(chapter *_ch, int _vs, ustring _txt);
    void print(void);
	void addToWordCount(void);
};

verse::verse(chapter *_ch, int _vs, ustring _txt)
{
	ch = _ch;
	vs = _vs;
	text = _txt;
}

void verse::print(void)
{
  cout << ch->bk->bookname << " " << ch->chapnum << ":" << vs << " " << text << endl;
  return;
}

std::unordered_map<std::string, int, std::hash<std::string>> wordCounts;

void verse::addToWordCount(void)
{
	vector<ustring> words;
	// Split string into its components, deleting USFM codes along the way
	string::size_type s = 0, e = 0; // start and end markers
    ustring delims(" \\,:;!?.\u0022()");
	ustring nonUSFMdelims(" ,:;!?.\u0022()"); // same list as above except without '\'
	// u0022 is unicode double-quote mark
	// TO DO : configuration file that has delimiters in it, and "’s" kinds of things to strip out
	// for the target language.
	for (string::size_type i = 0; i < text.size(); i++) {
		// Walk forward until we run into a character that splits words
		e = i; // move e along to track the iterator i
		bool foundDelim = (delims.find_first_of(text[i]) != ustring::npos);
		// If the last character is not already a delimiter, then we have to treat the next "null" as a delimiter
		if (!foundDelim && (i == text.size()-1)) { foundDelim = true; e = i + 1; } // point e to one past end of string (at null terminator)
		if (foundDelim) { // We found a delimiter
			// For a zero-length word, we bail. e=i can fall behind s if multiple delims in a row
			if (s >= e) { continue; }
			ustring newWord = text.substr(s, e-s); // start pos, length
			//cout << "Found new word at s=" << s << " e=" << e << ":" << newWord << ":" << endl;
			ustring::size_type pos = newWord.find("’s");
			bool found_apostrophe_s = (pos != ustring::npos);
			if (found_apostrophe_s) {
				cout << "Found new word with apostrophe s=" << newWord;
				newWord = newWord.substr(0, pos);
				cout << " replacing with " << newWord << endl;
			}
			if (newWord[0] != '\\') { words.push_back(newWord); } // only add non-usfm words
			// Advance past the splitting character unless it is the usfm delimiter '\'
			if (text[e] != '\\') { e++; }
			// Now if s has landed on a non-USFM delimiter, move fwd one
			while (nonUSFMdelims.find_first_of(text[e]) != ustring::npos) { e++; }
			s=e; // ensure starting point is at last ending point
		}
	}
	
	// Print each word
	for (auto &word : words) {
		//cout << "Word: " << word << ":" << endl;
		// TODO: The problem with lowercase is words like Lord, proper names, etc. need to remain uppercase, sometimes.
		wordCounts[word.lowercase()]++;
	}
}

class bible {
  public:
	ustring projname;
	vector<book> books; 
	bible(ustring _proj);
};

bible::bible(ustring _proj)
{
	projname = _proj;
}

void chapter::load(int book, int chapter)
{
	int vscnt = 0;
	// With these, it will be interesting to leave them at first; then to optimize
	// so that I can see how much faster it will go.
	// TODO. This copies a huge vector. Not efficient. Need to pass references.
	vector<ustring> data = project_retrieve_chapter (bk->bbl->projname, book, chapter);
	cout << "Processing " << bk->bookname << " " << chapnum << endl;
	for (auto it = data.begin(); it != data.end(); ++it) {
		// TODO: probably more inefficiencies here
		ustring marker = usfm_extract_marker(*it);
		if (usfm_is_verse(marker)) {
			// We found a verse, not just a line of usfm codes like \p or \s or whatever
			vscnt++;
			// Remove verse number from usfm text
			ustring &s = *it;
			size_t endposition = s.find_first_of(" ", 1);
			if (endposition != string::npos) {
				ustring versenum = s.substr(0, endposition);
				s.erase(0, endposition+1); // +1 grabs the space after the verse number
			}
			verse newVerse(this, vscnt, s);
			//newVerse.print();
			verses.push_back(newVerse);
			newVerse.addToWordCount();
		}
	}
}

// The set of words that we are NOT interested
// in having concordance data for.
unordered_set<string> excludedWords;

void readExcludedWords(const ustring &filename)
{
	// The file is expected to have unicode-encoded strings, one per line
	ifstream myfile;
	string line;
	myfile.open("strings.txt");
	// eventually change to Glib::IOChannel::read_line for unicode
	while (getline(myfile, line)) {
		//cout << "Found word " << line << endl;
		excludedWords.insert(line);
	}
	myfile.close();
	// Iterate over the set and print each one out, to double-check iter_swap
	cout << "Excluded words are:" << endl;
	for (auto &word : excludedWords) {
		cout << "\t" << word << endl;
	}
}

directories *Directories;
Settings *settings;
BookLocalizations *booklocalizations;
Versifications *versifications;
Mappings *mappings;
Styles *styles;
//GtkAccelGroup *accelerator_group;
URLTransport * urltransport;
VCS *vcs;

extern book_record books_table[];

int main(int argc, char *argv[])
{
	readExcludedWords("strings.txt");
	  // Create a new directories 'factory' and initialize it with argv[0]
	Directories = new directories(argv[0]);

    books_init(); // TEMP - MAP

	Directories->find_utilities();  // Find core utilities like copy, rm, etc. Must do before next.
	//NOT NEEDED Directories->check_structure(); // Check on required directory structure.
	settings = new Settings(true);
	booklocalizations = new BookLocalizations(0);
	versifications = new Versifications(0);
	// Verse mappings object.
	mappings = new Mappings(0);
	// Styles object.
	styles = new Styles(0);
	// Version control object.
	vcs = new VCS(0);
	// URLTransport object.
	urltransport = new URLTransport(0);
	
	// Now do the work of loading the chapters and verses, splitting into words, 
	// counting in our mapping structure, etc.
	bible kjv("KJVBibleFromUSFM");
	for (int b = 1; b <= 66; b++) {
		ustring bookname = books_table[b].name;
		book *newbk = new book(&kjv, bookname);
		kjv.books.push_back(*newbk);
		vector <unsigned int> chapters = versification_get_chapters("English", b);
		for (auto c : chapters) {
			chapter *newchap = new chapter(newbk, c);
			newchap->load(b, c);
		}
	}

	unsigned int totalWords = 0;
	unsigned int uniqueWords = 0;
	for (const auto &pair : wordCounts) {
		cout << pair.first << " " << pair.second << endl;
		uniqueWords++;
		totalWords+=pair.second;
	}
	cout << "UniqueWords " << uniqueWords << endl;
	cout << "TotalWords " << totalWords << endl;
	return 0;
}