#!/bin/bash
cd ~/dev/dependencies/npm
cp ~/dev/bibledit/lib/editpm/main.js .
./node_modules/.bin/browserify main.js -o prosemirror.js
cp ./node_modules/prosemirror-view/style/prosemirror.css .
rm main.js
mv *.js ~/dev/bibledit/lib/editpm/
mv *.css ~/dev/bibledit/lib/editpm/
