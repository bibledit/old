/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author daniel
 */

public class SearchOptions {
    boolean exhausted = false;
    int fromBook, toBook;
    String searchString;

    int currentBook, currentChapter, currentVerse;

    public SearchOptions(
            int fromBook,
            int toBook,
            String searchString,
            int currentBook,
            int currentChapter,
            int currentVerse) {
        this.fromBook = fromBook;
        this.toBook = toBook;
        this.searchString = searchString;
        this.currentBook = currentBook;
        this.currentChapter = currentChapter;
        this.currentVerse = currentVerse;
    }
    public void dump() {
        System.err.println("Search Snapshot dump: ");
        System.err.println("\tfromBook: " + fromBook);
        System.err.println("\ttoBook: " + toBook);
        System.err.println("\tsearchString: " + searchString);
        System.err.println("\tcurrentBook: " + currentBook);
        System.err.println("\tcurrentChapter: " + currentChapter);
        System.err.println("\tcurrentVerse: " + currentVerse);
        System.err.println();

    }
}