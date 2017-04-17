# Concaten Documentation

Concaten's documentation ~~is~~ will be broken into three parts:

1. Tutorials and language guide
2. Word reference
3. Glossary

The first will introduce the reader to the essential concepts of Concaten --
things like the three stacks, how words work, and the type system. The second
will provide a detailed explanation of every word in the standard library. The
third will be a reference for terms that someone reading the tutorials or guide
might not be familiar with -- first/second/etc. on stack, pop/peek/push, and so
on. 

There will also be example code for common demonstration tasks, like Hello,
World! and calculating Fibonacci numbers.

## Layout

All of the documentation is under `docs/`; the word reference is under
`docs/words/`, the tutorials are under `docs/tutorials`, and the glossary is
available at `docs/GLOSSARY.md`.

### Word reference

The word reference is broken down by where the words have effect; specifically,
all the methods of each type are grouped with all the other words for that
type, and the types for each namespace are grouped together. The global words
are grouped with each other in the standard library namespace.

|   Namespace   |     Folder    |
| :-----------: | :-----------: |
|  None/global  |  `docs/stl/`  |

### Tutorials

The tutorials cover a wide range of subjects. See the README in that folder
for a list of the tutorials available, and the filenames.

### Glossary

A list of words that might or might not be known by someone reading through
the documentation for the first time. See the file for more information.
