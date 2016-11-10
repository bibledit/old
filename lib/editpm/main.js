/*
 Copyright (©) 2003-2016 Teus Benschop.
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


const {EditorState} = require("prosemirror-state")
const {MenuBarEditorView} = require("prosemirror-menu")
const {DOMParser, Schema} = require("prosemirror-model")
const {schema: baseSchema} = require("prosemirror-schema-basic")
const {addListNodes} = require("prosemirror-schema-list")
const {exampleSetup} = require("prosemirror-example-setup")


schema = new Schema ({
  nodes: addListNodes(baseSchema.nodeSpec, "paragraph block*", "block"),
  marks: baseSchema.markSpec
})


editorPmView = new MenuBarEditorView (document.querySelector ("#editor"), {
  state: EditorState.create ({
    doc: DOMParser.fromSchema (schema).parse (content),
    plugins: exampleSetup ({schema})
  }),
  onAction (action) { view.updateState (view.editor.state.applyAction (action)) },
  onFocus () {
  }
})


/*
const {EditorState} = require("prosemirror-state")
const {EditorView} = require("prosemirror-view")
const {MenuBarEditorView} = require("prosemirror-menu")
const {DOMParser, Schema} = require("prosemirror-model")
const {schema: baseSchema} = require("prosemirror-schema-basic")
const history = require("prosemirror-history")
const {keymap} = require("prosemirror-keymap")
const {addListNodes} = require("prosemirror-schema-list")
const {exampleSetup} = require("prosemirror-example-setup")
const schema = new Schema({
  nodes: addListNodes(baseSchema.nodeSpec, "paragraph block*", "block"),
  marks: baseSchema.markSpec
})
let view = new MenuBarEditorView (
  document.getElementById ("editor"),
  {
    state: EditorState.create({
      doc: DOMParser.fromSchema (schema).parse (content),
      //schema: schema,
      plugins: [
        //history.history (),
        //keymap ({
          //"Mod-z": history.undo,
          //"Mod-Shift-z": history.redo
        //}),
        exampleSetup ({schema})
      ]
    }),
    onAction: function(action) {
      view.updateState (view.state.applyAction (action))
    }
  }
)
// Focus the editor (but for some reason it does not work).
view.focus ();
document.getElementById ("editor").focus ();
*/
