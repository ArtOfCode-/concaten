# Concaten v0.7.1

Concaten is a concatenative, stack-based, strongly but optionally strictly
typed, hyperdynamic, garbage-collected, interpreted programming language. In
order, that means that:

1. **concatenative** - Each "word" (Concaten's version of a function) is
  applied to the results of the one that appears before it in the source code.
  There are a few exceptions, though; notably, `{` and `->`. The full spec
  explains how those words work; it'll be available as part of v1.0.
2. **stack-based** - Results, rather than being passed directly to methods or
  stored in variables, go onto an intermediary data stack, on which they can
  stay indefinitely. Note that this may occur in other languages like C, as
  part of the implementation of passing parameters to methods; however,
  Concaten's is persistent -- C's, if not empty after all the parameters have
  been passed to the method, is messing something up.
3. **strongly but optionally strictly typed** - Most methods related to
  variable assignment take a type as an optional parameter, and check that the
  datum to be stored is of that type. They are **optional**, however, and duck
  typing works fine in Concaten. At the same time, objects of one type are
  never implicitly converted to another type by the language -- though, of
  course, words, including those in the standard library, are free to. (c.f.
  all standard library words beginning `*`)
4. **hyperdynamic** - Every keyword is an overrideable (though *not*
  replaceable) word, and everything is an object that can have methods
  monkey-patched in or out. Every function's code can be read, analyzed, and
  changed at runtime. The one exception to this is an object's properties --
  to allow fundamental objects and methods to be implemented in the
  interpreter's language, either for speed boosts or simplicity, properties
  are neither visible nor editable outside of that object's methods.
5. **garbage-collected** - You don't need to worry about memory; Concaten will
  handle it for you with a reference-counting garbage collector. Because of
  its nature, you can also usually rely on the GC deleting and destructing an
  object when you expect it to -- in comparison to tracing GCs, which run
  effectively whenever they want to, a refcounter imposes a constant, slight
  overhead in exchange for deleting an object as soon as it's no longer used,
  which means that destructors have meaning. As long as cyclical references
  are avoided, at least.
6. **interpreted** - Rather than being converted to machine language or
  assembly, then executed directly by the CPU or OS, Concaten code is executed
  as-is by an intermediary program. Depending on the OS and specific
  circumstances, this might be abstracted away such that the user doesn't see
  it happening directly (e.g. through Windows' file extension assocations);
  under the hood, it always is.
0. **programming langauge** - Hopefully, if you're reading this,
  self-explanatory.

For an example of how the language will probably look, see `test.ctn`.
  However, keep in mind that this is subject to change; though the language is
  specced out, actual usage might reveal things that I didn't consider, and
  therefore need to add in, remove, or change.

---

If you want to contribute code, awesome! Be sure to check out `CODE_STYLE.md`.

If you don't feel like contributing code, but want to help, run this command,
which should work on any shell with `git`, `cmake`, and `make` installed:

    git clone https://github.com/nic-hartley/concaten.git -b dev && \
    cd concaten && cmake . && make && ./tester

Then send me the output (presumably, you can get to me somehow; I doubt you
found this on your own). 

If everything succeeds, the output will look something like this:

    Testing code_block...       42 succeeded in   37 us.
    Testing data_stack...       43 succeeded in   18 us.
    Testing method_map...       46 succeeded in   52 us.
    Testing object...           38 succeeded in   69 us.
    Testing prop_map...         37 succeeded in   39 us.
    Testing stringbuilder...    24 succeeded in   22 us.
    Testing token_stack...     106 succeeded in   12 us.
    Testing tokenizer...        28 succeeded in  426 us.
    Done with all tests

If it failed at any stage, you'll see something like this in the middle:

    /path/to/dir/tests/foo.c:123: Failed something-or-other
    1 failed in 12 us.

And no further tests will be run. If it fails to compile, you'll see GCC
errors; please report those, too.

If you get a segfault, that's especially important. Please try to track it
down; since I don't have unlimited access to your machine, I can't really find
the bug.

---

**A note about versions**: My format is `major.milestone.submilestone`, where
  each is the number for the most recently finished. For example, version
  `1.1.1` just completed the first task of milestone 2 of verison 1. Note that,
  despite the superficial similarity, versions are not numbers: `1.10` comes
  after `1.9`. A "submilestone" is a piece that's big enough to warrant its
  own marker, but not big enough to warrant its own milestone.

Major version `0` is pre-completion; some of the parts may be
  done, but not all of it. Major version `1` is the first completed version of
  the language; from there, I'll iterate based on feedback and input from the
  users, and implement niceties.

Versions only apply to that branch. For example, dev might be at 1.6.3, while
  master is on 0.5.7; that just means that the bit that's on dev is incomplete
  and not ready to be pushed to master yet.

---

### Current milestone

#### `scope_stack.h` - 0.7

* [ ] `scope_stack.h` - 0.7  
  Contains the list of words. This is a stack so we can sensibly implement
    things like local variables. Shouldn't be too much trouble; it'll mostly
    be combining `PropMap` and `DataStack`.
  * [x] Design the interface - 0.7.1
    * Should be analogous to `TokenStack` -- you can push layers, etc. -- but
      with maps, instead of stacks. Layers are basically restore points, but
      stacked up instead of numbered.
    * Needs a `get_candidates` function, which returns a list of every word
      with that name.
  * [x] Implement the functions - 0.7.2
  * [ ] Test them - 0.7.3
    * Make sure they don't leak memory!
  * [ ] Edit `Runnable` to take this as a parameter, too - 0.7.4
    * Nothing outside of Runnable-related stuff should need to be changed,
      because anything that can be run should be run through the `Runnable`
      object. Do a second check, though.
      

### Upcoming milestones
* [ ] Main method - 0.8
* [ ] Minimal standard library - 0.9  
  Words like `if`, `{`, and `puts` so we can play with the language at all.
  * When designing/adding arrays, add the same methods to `CodeBlock`
  * If necessary, create `ctno_mk_` family to automatically create things
  * Some of this might be done during 0.8 to facilitate testing.
* [ ] Misc. required updates as needed
  * [ ] Thorough code review  
    Finding places where the code is needlessly complicated, duplicated, or
      the like, and fixing it.
  * [ ] Profiling/optimizing.  
    Design some code that'll test as many features as possible, as strenuously
      as possible, so that we have a good program to benchmark and profile
      with. I want to optimize the slowest parts.
  * [ ] Benchmarking between Concaten and equivalent-level languages  
    Probably Python or Ruby. I want at most 3x time taken, and for that value
      to be as low as possible.
  * Beta testing?
    Hand out the interpreter as it is to as many people as possible, get them
      to play with the language and try to break it.
* [ ] Documentation - 1.0  
  Including demo code, a detailed up-to-date list of every default global word
    (like `if`) and what they do, every built-in method, and a brief overview
    of how Concaten works, as well as how to install it and what the
    command-line arguments are.  
  In theory, mostly done (because of the spec, and you did this while writing
    the standard library, *right*?), but in practice, it probably should be
    rewritten.
* ***Take a break.*** You just wrote an entire programming language. Give it a
  week or two, play with it a little, but don't start working on the next bits.
  Bugfixes only.
* [ ] Command-line options - 1.1  
  Definitely at least `-e`; take inspiration from Ruby and Python.  
  These should mostly be minor variations on the existing main method.
* [ ] Debug mode - 1.2  
  Allows breakpoints (w/ `breakpoint`?) and stops execution when exception is
    thrown outside of a `try`.
* [ ] User-created libraries - 1.3  
  `namespace` word that uses some Concaten magic to let people define their
    own modules. This is in part to encourage other people to write ~~my code
    for me~~ extensions to Concaten, and in part to set up the framework for
    the next milestone(s).
* [ ] Decide which extension libraries to implement, scatter the rest over 2.0
  and 3.0 as appropriate. These will form the next 1.x updates.
  * Networking
  * File I/O
  * String manipulation
  * Math
  * Regex (that'll be a lot of effort)
    * Optionally, use an external library, integrated with this.
  * GUI (ditto)
* [ ] Multithreading/thread safety - 2.0  
  This is going to require something close to a complete rewrite, to make sure
    everything is as thread-safe as it can get. Note that this is deliberately
    a bullet list, not checkboxes, since it's ideas at this stage.
  * Make everything thread-safe (mutexes on read/write; allow parallel reads
    but not parallel writes)
    * `set`s lock a mutex for the entire time; `get`s lock and immediately
      unlock them? (to sync, make sure that gets don't occur in the middle of
      a set)
      * Maybe disable this until first thread created, then enable it? (for
        performance in single-threaded applications)
    * Each thread gets its own copied, but independent, stacks/scopes; passing
      data has to be done very intentionally. (This also removes the need
      for synchronization on the stacks, which would get expensive)
    * Force sync between reads/writes of console/files; GUI updates?
  * [Task](https://msdn.microsoft.com/en-us/library/dd537609.aspx)s?
  * Asynchronous versions of the Networking and File I/O APIs.
  * Events? (technically possible already, but easier with multithreading)
* [ ] C code linking at interpret-time (like Python) - 3.0  
  That way, people can write Concaten libraries in C, for what'll probably be
    a slight speed boost, but also lower-level access to the OS than Concaten
    allows.
* [ ] Full Unicode support - 4.0  
  Correctly interprets source code with Unicode characters (might already, but
    needs testing); strings go from collections of ASCII bytes to collections
    of Unicode codepoints.  
  Also, support Unicode in string-modification/i18n modules.

### Previous milestones

* [x] `tokenizer.h` - 0.0  
  Converts a stream of characters into a stream of tokens. That way, the
    interpreter doesn't need to even think about things like comments or
    whitespace -- it just takes the tokens and runs with them.  
  Can read from both a string in memory and a file on the disk directly. The
    former will make `eval` easier to code, and the latter makes the
    interpreter a little easier to write.
* [x] `object.h` - 0.1      
  The building block of the language. Even lambdas are manipulatable objects.
    Can store any type of data, because Concaten is dynamically typed, after
    all.
* [x] `data_stack.h` - 0.2  
  The central data stack. Contains intermediary results; words pull their
    arguments from it and push their results to it.
* [x] `token_stack.h` - 0.3  
  Where tokens get pulled from during program execution.
* [x] `code_block.h` - 0.4  
  A simple object which represents Concaten's equivalent of anonymous
    functions. Can be run, in addition to being modified at runtime, as it's
    simply an array of tokens with some extra words (`call`, for example).
* [x] Refactor to interface consistent across all bits and pieces - 0.5  
  The code is getting a little out of hand. I have some things I want to do to
    clean it up. (Fun fact: This bit took more time than any other milestone,
    I think. Just goes to show how important planning in advance is.)
