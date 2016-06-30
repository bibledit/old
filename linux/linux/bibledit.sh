#!/bin/bash
cd
cd bibledit
./bibledit 2>&1 | grep -v WARNING | tr -d 012
