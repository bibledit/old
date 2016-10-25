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


var EditorState = require("prosemirror-state").EditorState
var EditorView = require("prosemirror-view").EditorView
var schema = require("prosemirror-schema-basic").schema
var history = require("prosemirror-history")
var keymap = require("prosemirror-keymap").keymap


var view = new EditorView (document.getElementById ("editor"), {
  state: EditorState.create({
    schema: schema,
    plugins: [history.history(), keymap({
      "Mod-z": history.undo,
      "Mod-Shift-z": history.redo
    })]
  }),
  onAction: function(action) {
    view.updateState (view.state.applyAction (action))
  },
  // This'll get less ugly soon.
  handleKeyDown: function(view, event) {
    if (event.ctrlKey && event.keyCode == 90) {
      return history.undo(view.state, view.props.onAction)
    }
  }
})
