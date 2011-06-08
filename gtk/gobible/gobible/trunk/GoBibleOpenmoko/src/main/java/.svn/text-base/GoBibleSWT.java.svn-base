

import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.net.URL;
import java.net.URLClassLoader;
import java.util.Arrays;
import java.util.Properties;

import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CTabFolder;
import org.eclipse.swt.custom.CTabItem;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.swt.events.MouseListener;
import org.eclipse.swt.events.MouseMoveListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.TextLayout;
import org.eclipse.swt.graphics.TextStyle;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.layout.RowLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.List;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;


public class GoBibleSWT {
	private static final String PREFERENCES_FILE_NAME = System.getProperty("user.home")+"/.gobible.config.xml";
	public static final String BIBLE_FILE_LOCATION = "bibleFileLocation";
	public static final String FONT_SIZE = "fontSize";
	
	static String longString = "Loading";

	public int currentBookIndex = 0;
	public int currentChapterIndex = 0;
	public int currentVerseIndex = 0;

	private int lineHeight = 0;
	
	// Current chapter contents
	public static int[] verseIndex;
	public static int[] verseIndexScreen;
	public static char[] verseData;
	private CTabItem textTab;
	private static final Properties preferences = new Properties();
	
	
	private BibleSource bibleSource;
	
	private TextStyle styleBold ;
	
	private TextLayout layout ;
	private final List bookSelectorBooknames;
	private Font font1;
	private Font fontBold;
	private Display display;

	public static void main(String[] args) throws Exception {
		new GoBibleSWT();
	}
	
	private String[] createStringArray(int numberOfChapters) {
		String[] a = new String[numberOfChapters];
		for (int i = 0; i < a.length; i++) {
			a[i] = ""+(i+1);
		}
		return a;
	}
	
	public GoBibleSWT() throws IOException {
		display = new Display();
		final Shell shell = new Shell(display, SWT.SHELL_TRIM | SWT.DOUBLE_BUFFERED);
		shell.setText("GoBibleSWT");
		shell.setLayout(new GridLayout());
		
		final CTabFolder tabFolder = new CTabFolder (shell, SWT.BORDER | SWT.BOTTOM | SWT.MULTI);
		tabFolder.setTabHeight(50);
//		tabFolder.setMinimumCharacters(20);
//		GridData layoutData = new GridData();
//		layoutData.verticalSpan = 10;
//		tabFolder.setLayoutData(layoutData);
		textTab = new CTabItem (tabFolder, SWT.NONE);
		textTab.setText ("  Text  ");
		final Canvas textCanvas = new Canvas(tabFolder, SWT.NONE); 
		textTab.setControl(textCanvas);
		
		readPreferences();
	
		final List chapter;
		final List verse;
		Button button;
		CTabItem tab2 = new CTabItem (tabFolder, SWT.NONE);
		tab2.setText ("  Goto  ");
		Composite gotoWindow = new Composite(tabFolder, SWT.BORDER);
		gotoWindow.setLayout (new GridLayout ());
		
		Composite bookSelector = new Composite(gotoWindow, SWT.BORDER);
		bookSelectorBooknames = new List (bookSelector, SWT.READ_ONLY | SWT.V_SCROLL);
		bookSelector.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		FillLayout fillLayout2 = new FillLayout ();
		fillLayout2.type = SWT.HORIZONTAL;
		bookSelector.setLayout (fillLayout2);

		
		chapter = new List(bookSelector, SWT.READ_ONLY | SWT.V_SCROLL);
		chapter.setItems(new String[] {""});
		
		
		verse = new List(bookSelector, SWT.READ_ONLY | SWT.V_SCROLL);
		verse.setItems(new String[] {""});
		
		bookSelectorBooknames.addListener(SWT.Selection, new Listener() {
			public void handleEvent(Event arg0) {
				if(bookSelectorBooknames.getSelectionIndex() != -1) {
					int numberOfChapters = bibleSource.getNumberOfChapters(bookSelectorBooknames.getSelectionIndex());
					chapter.setItems(createStringArray(numberOfChapters));
				}
			}
		});

		chapter.addListener(SWT.Selection, new Listener() {
			public void handleEvent(Event event) {
				if(bookSelectorBooknames.getSelectionIndex() != -1 && chapter.getSelectionIndex() != -1) {
					verse.setItems(createStringArray(
							bibleSource.getNumberOfVerses(bookSelectorBooknames.getSelectionIndex(), chapter.getSelectionIndex())));
				}
			}
		});

		
		button = new Button (gotoWindow, SWT.PUSH);
		button.setText ("  Goto  ");
		GridData buttonLayoutData = new GridData(SWT.CENTER, SWT.CENTER, false, false);
		buttonLayoutData.verticalSpan = 12;
		button.setLayoutData(buttonLayoutData);
		
		tab2.setControl(gotoWindow);
		
		CTabItem tab3 = new CTabItem(tabFolder, SWT.NONE);
		addPreferencesTab(tabFolder, tab3);
		
		
		tabFolder.setLayoutData(new GridData(GridData.HORIZONTAL_ALIGN_FILL | GridData.GRAB_HORIZONTAL | GridData.VERTICAL_ALIGN_FILL | GridData.GRAB_VERTICAL));
		
		layout = new TextLayout(display);

		setupFonts();

		layout.setText(longString);

		class MouseMoves implements MouseMoveListener, MouseListener {
			int lastY = -1;
			int lineOffsets[];
			int curOffset = 0;
			boolean mouseDown = false;
			long lastTextMoveTS = 0;

			public void mouseUp(MouseEvent event) {
				processMove(textCanvas, event);
				lastY = -1;
				mouseDown = false;
			}

			public void mouseDown(MouseEvent event) {
				lastTextMoveTS = System.currentTimeMillis();
				mouseDown = true;
			}

			public void mouseDoubleClick(MouseEvent event) {
			}

			public void mouseMove(MouseEvent event) {
				if (!mouseDown) {
					// lastY = event.y;
					return;
				}
				if (lastY == -1) {
					lastY = event.y;
					return;
				}
				long now = System.currentTimeMillis();
//				if((lastTextMoveTS+250)<now) {
//					processMove(textCanvas, event);
//					lastY = event.y;
//					lastTextMoveTS = now;
//				}
			}

			private void processMove(final Canvas textCanvas, MouseEvent event) {
				if(Math.abs(lastY-event.y) < lineHeight) {
					System.out.println("No finger scroll x:"+event.x+"  y:"+event.y+"  x>"+(shell.getSize().y *2/3)+ "  y>"+(shell.getSize().y *3/4));
					if(event.y > (shell.getSize().y *2/3) && event.x > (shell.getSize().x *2/3)) {
						System.out.println("Do page scroll down");
						int i = Arrays.binarySearch(lineOffsets, curOffset);
						scroll(textCanvas, i, (textCanvas.getSize().y/lineHeight)-1);
					} else if(event.y < (shell.getSize().y /3) && event.x > (shell.getSize().x *2/3)) {
						System.out.println("Do page scroll up");
						int i = Arrays.binarySearch(lineOffsets, curOffset);
						scroll(textCanvas, i, (-textCanvas.getSize().y/lineHeight)+1);
					} else if(event.y > (shell.getSize().y *2/3) && event.x < (shell.getSize().x *1/3)) {
						long start = System.currentTimeMillis();
						System.out.println("Next chapter "+currentBookIndex+"  "+currentChapterIndex);
						nextChapter();
						loadCurrentChapter();
						System.out.println("after loadCurrentChapter = "+(System.currentTimeMillis()-start));
						longString = createVerseString();
						layout.setText(longString);
						lineOffsets = layout.getLineOffsets();
						curOffset = 0;
						//						layout.setText(longString.substring(curOffset));
						setTextStyle(0);
						System.out.println("after getLineOffsets = "+(System.currentTimeMillis()-start));
						updatePosition();			
						textCanvas.redraw();
						System.out.println("end = "+(System.currentTimeMillis()-start));
					} else if(event.y < (shell.getSize().y /3) && event.x < (shell.getSize().x *1/3)) {
						System.out.println("Prev chapter"+currentBookIndex+"  "+currentChapterIndex);
						prevChapter();
						loadCurrentChapter();
						longString = createVerseString();
						layout.setText(longString);
						lineOffsets = layout.getLineOffsets();
						curOffset = 0;
						//						layout.setText(longString.substring(curOffset));
						setTextStyle(0);
						updatePosition();
						textCanvas.redraw();
					}
					
				} else if (lastY < event.y) {
					if (curOffset >= 0) {
						int i = Arrays.binarySearch(lineOffsets, curOffset);
						int up = (lastY-event.y)/lineHeight;

						System.out.println("up " + i+"  "+up+ "  pix:"+(lastY-event.y));
//						if(i+up < 0) {
//							up = -i;
//						}
						scroll(textCanvas, i, up);
					}

				} else if (lastY > event.y) {
//					long start = System.currentTimeMillis();
					int i = Arrays.binarySearch(lineOffsets, curOffset);
					int down = (lastY-event.y)/lineHeight;
					if(down > 0) {
						System.out.println("down " + i+"  "+down+"  pix:"+(lastY-event.y));
//						if (!((i+down) < lineOffsets.length)) {
//							//						System.out.println("ny down: "+(i - lineOffsets.length -1)+" gl down:"+down);
//							down = i - lineOffsets.length -1;
//						} 
//						if((i+down)<0) {
//							return;
//						}
						//					System.out.println("down " + down+"  "+(lastY-event.y));
						//						System.out.println("height = "+layout.getLineMetrics(0).getHeight());
						scroll(textCanvas, i, down);
					}
//					System.out.println(System.currentTimeMillis()-start);
				}
				long start = System.currentTimeMillis();
//				textCanvas.update();
				System.out.println("update = "+(System.currentTimeMillis()-start));
			}

			private void nextChapter() {
				if(currentBookIndex==bibleSource.getNumberOfBooks()-1 && currentChapterIndex==bibleSource.getNumberOfChapters(bibleSource.getNumberOfBooks()-1)-1) {
					return;
				}
				System.out.println("number of chapters: "+bibleSource.getNumberOfChapters(currentBookIndex));
				if(currentChapterIndex==bibleSource.getNumberOfChapters(currentBookIndex)-1) {
					currentBookIndex++;
					currentChapterIndex = 0;
				} else {
					currentChapterIndex++;
				}
				System.out.println("cci="+currentChapterIndex+"  cbi="+currentBookIndex);
			}

			private void prevChapter() {
				if(currentBookIndex==0 && currentChapterIndex==0) {
					return;
				}
				if(currentChapterIndex==0) {
					currentBookIndex--;
					currentChapterIndex = bibleSource.getNumberOfChapters(currentBookIndex)-1;
				} else {
					currentChapterIndex--;
				}
				System.out.println("cci="+currentChapterIndex+"  cbi="+currentBookIndex);
			}

			private void scroll(final Canvas textCanvas, int i, int down) {
				if(i+down < 0) {
					down = -i;
				}
				if (!((i+down) < lineOffsets.length)) {
					//						System.out.println("ny down: "+(i - lineOffsets.length -1)+" gl down:"+down);
					down = i - lineOffsets.length -1;
				} 
				if((i+down)<0) {
					return;
				}
				layout.setText(longString.substring(lineOffsets[i+down]));
				curOffset = lineOffsets[i + down];
				setTextStyle(curOffset);
				textCanvas.redraw();
			}
		}
		;

		final MouseMoves dtListener = new MouseMoves();
		
		button.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				currentBookIndex = bookSelectorBooknames.getSelectionIndex();
				currentChapterIndex = chapter.getSelectionIndex();
				if(currentChapterIndex == -1) {
					currentChapterIndex = 0;
				}
				currentVerseIndex = verse.getSelectionIndex();
				loadCurrentChapter();
				longString = createVerseString();
				layout.setText(longString);
				dtListener.lineOffsets = layout.getLineOffsets();
				dtListener.curOffset = 0;
//				layout.setText(longString.substring(curOffset));
				setTextStyle(0);
				updatePosition();
				textCanvas.redraw();
				tabFolder.setSelection(0);
			}
		});

		
		Listener listener = new Listener() {
			public void handleEvent(Event event) {
				switch (event.type) {
				case SWT.Paint:
					layout.setWidth(shell.getClientArea().width - 40);
					layout.draw(event.gc, 10, 10);
					break;
				case SWT.Resize:
					layout.setWidth(textCanvas.getSize().x - 20);
//					System.out.println("resize called "+textCanvas.getSize().x);
					dtListener.lineOffsets = layout.getLineOffsets();
//					System.out.println("lineOffsets = "+Arrays.toString(dtListener.lineOffsets));
					break;
				}
			}
		};

		
		textCanvas.pack();
		tabFolder.pack ();
		shell.pack ();		

		textCanvas.addListener(SWT.Paint, listener);
		//shell.addListener(SWT.Resize, listener);
		tabFolder.addListener(SWT.Resize, listener);
		textCanvas.addMouseMoveListener(dtListener);
		textCanvas.addMouseListener(dtListener);
		
		shell.setSize(480, 500);
		lineHeight = layout.getLineMetrics(0).getHeight();
		
		if(preferences.getProperty(BIBLE_FILE_LOCATION) != null) {
			try {
				readBible();
			} catch (Throwable e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
		}
		
//		dtListener.lineOffsets = layout.getLineOffsets();
//		bibleSource = new CombinedChapterBibleSource(this);
//		loadCurrentChapter();
		
		longString = createVerseString();
		layout.setText(longString);
		updatePosition();
		
		setTextStyle(0);
		tabFolder.setSelection(1);
		boolean focus = bookSelector.setFocus();
//		System.out.println("fokus = "+focus);
//		textCanvas.setFocus();
//		System.out.println("verseIndex       = "+Arrays.toString(verseIndex));
//		System.out.println("verseIndexScreen = "+Arrays.toString(verseIndexScreen));
//		shell.forceActive();
//		shell.forceFocus();

		shell.open();
		tabFolder.setSelection(0);
//		dtListener.lineOffsets = layout.getLineOffsets();
//		System.out.println("lineOffsets = "+Arrays.toString(dtListener.lineOffsets));
		while (!shell.isDisposed()) {
			if (!display.readAndDispatch())
				display.sleep();
		}
		display.dispose();
	}

	private void setupFonts() {
		Color black = display.getSystemColor(SWT.COLOR_BLACK);
		Color gray = display.getSystemColor(SWT.COLOR_GRAY);

		font1 = new Font(display, "Liberation Sans", Integer.parseInt(preferences.getProperty(FONT_SIZE, "7")), SWT.NORMAL);
		fontBold = new Font(display, "Liberation Sans", Integer.parseInt(preferences.getProperty(FONT_SIZE, "7")), SWT.BOLD);
		layout.setFont(font1);
		styleBold = new TextStyle(fontBold, black, gray);
	}

	private void addPreferencesTab(final CTabFolder tabFolder, CTabItem tab3) {
		tab3.setText ("Preferences");
		Composite preferencesTab = new Composite(tabFolder, SWT.BORDER);
		tab3.setControl(preferencesTab);
		RowLayout rowLayout = new RowLayout();
 		rowLayout.wrap = false;
 		rowLayout.pack = false;
 		rowLayout.justify = false;
 		rowLayout.type = SWT.VERTICAL;
 		rowLayout.marginLeft = 2;
 		rowLayout.marginTop = 2;
 		rowLayout.marginRight = 2;
 		rowLayout.marginBottom = 6;
 		rowLayout.spacing = 0;
		preferencesTab.setLayout (rowLayout);
		Label bibleFileLocationLabel = new Label(preferencesTab, SWT.WRAP);
		bibleFileLocationLabel.setText("Bible jar file location:");
		final Text bibleFileLocation = new Text(preferencesTab, SWT.SINGLE);
		bibleFileLocation.setText(preferences.getProperty(BIBLE_FILE_LOCATION, "/tmp"));
		
		Label fontSizeLabel = new Label(preferencesTab, SWT.WRAP);
		fontSizeLabel.setText("Font size:");
		final Text fontSize = new Text(preferencesTab, SWT.SINGLE);
		fontSize.setText(preferences.getProperty(FONT_SIZE, "7"));
		
		Button savePreferences = new Button(preferencesTab, SWT.PUSH);
		savePreferences.setText("Save preferences");
		savePreferences.addListener(SWT.Selection, new Listener() {

			public void handleEvent(Event arg0) {
				preferences.put(BIBLE_FILE_LOCATION, bibleFileLocation.getText());
				preferences.put(FONT_SIZE, fontSize.getText());
				savePreferences();
				try {
					readBible();
					longString = createVerseString();
					layout.setText(longString);
					updatePosition();
				} catch (Throwable e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				setupFonts();
			}


			
		});
	}

	private final void readBible() throws Throwable {
		String path = "jar:file:"+preferences.getProperty(BIBLE_FILE_LOCATION, "/tmp")+"!/";
		URL url = new URL(path);
		URLClassLoader cl = new URLClassLoader(new URL[] { url });

		boolean loaded = false;
		Throwable t = null;
		for(char c = 'a'; c<'m'; c++) {
			try {
				System.out.println("Trying to load "+c);
				Class<?> goBible = cl.loadClass(""+c);

				bibleSource = new CombinedChapterBibleSource(goBible);
				loadCurrentChapter();
				loaded = true;
				break;
			} catch (NullPointerException npe) {
				npe.printStackTrace();
			} catch (Throwable e) {
				System.err.println("Error: "+e.toString());
				t = e;
 			}
		}
		if(!loaded) {
			throw t;
		}
		bookSelectorBooknames.setItems (bibleSource.getBookNames());
	}
	
	private void readPreferences() {
		FileInputStream fis;
		try {
			fis = new FileInputStream(PREFERENCES_FILE_NAME);
			preferences.loadFromXML(fis);
			fis.close();
		} catch (IOException e) {
			preferences.clear();
		}
	}

	private void savePreferences() {
		FileOutputStream fos;
		try {
			fos = new FileOutputStream(PREFERENCES_FILE_NAME);
			preferences.storeToXML(fos, "Saved from GoBibleSWT");
			fos.flush();
			fos.close();
		} catch (IOException e) {
			e.printStackTrace();
			// TODO: handle exception
		}
	}

	
	protected void updatePosition() {
		if(bibleSource != null) {
			textTab.setText("  "+bibleSource.getBookName(currentBookIndex)+" "+(currentChapterIndex+1)+"  ");
		}
	}

	public void loadCurrentChapter() {
		try {
			verseData = bibleSource.getChapter(currentBookIndex,
					currentChapterIndex);
			verseIndex = bibleSource.getChapterIndex(currentBookIndex,
					currentChapterIndex);
		} catch (IOException e) {
			System.out.println("IOException in loadCurrentChapter()");
		}
	}
	
	private void setTextStyle(int offset) {
		if(verseIndexScreen == null) {
			return;
		}
		int start = 0;
		if(offset>0) {
			for(int i=0; i<verseIndexScreen.length; i=i+2) {
				if(verseIndexScreen[i]>=offset) {
					start=i;
					break;
				}
			}
		}
		for(int i=start; i<(verseIndexScreen.length); i=i+2) {
			layout.setStyle(styleBold, verseIndexScreen[i]+(i==0 ? 0 : 1)-offset, verseIndexScreen[i+1]-2-offset);
		}
	}
	
	private String createVerseString() {
		int indexCounter = 0;
		if(verseIndex == null) {
			return "";
		}
		verseIndexScreen = new int[verseIndex.length];
//		System.out.println(Arrays.toString(verseIndex));
		int verseCounter = 1;
		StringBuffer result = new StringBuffer();
		for(int i=0; i<verseData.length; i++) {
			if(indexCounter < verseIndex.length && verseIndex[indexCounter]==i) {
				if(verseCounter != 1) {
					verseIndexScreen[indexCounter] = result.length();
					result.append(' ');
				}
				result.append(verseCounter++);
				result.append(' ');
				verseIndexScreen[indexCounter+1] = result.length();
				indexCounter = indexCounter + 2;
			}
			if(indexCounter < verseIndex.length && verseIndex[indexCounter]==i) {
				i--;
			} else {
				result.append(verseData[i]);
			}
		}
		return result.toString();
	}
}
