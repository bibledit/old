#!/bin/bash
pushd ~/dev/dependencies/npm > /dev/null
: <<'COMMENT'
npm install prosemirror-state
npm install prosemirror-view
npm install prosemirror-menu
npm install prosemirror-schema-basic
npm install prosemirror-schema-list
npm install prosemirror-history
npm install prosemirror-keymap
npm install prosemirror-model
npm install prosemirror-example-setup
npm install browserify
COMMENT
cp ~/dev/bibledit/lib/editpm/main.js .
./node_modules/.bin/browserify main.js -o prosemirror.js
cp ./node_modules/prosemirror-view/style/prosemirror.css .
cp ./node_modules/prosemirror-menu/style/menu.css .
rm main.js
mv *.js ~/dev/bibledit/lib/editpm/
mv *.css ~/dev/bibledit/lib/editpm/
popd > /dev/null
