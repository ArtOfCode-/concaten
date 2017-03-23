Concaten v0.5.1
===============

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

**A note about versions**: My format is `major.milestone`, where each is
  the number for the most recently finished. For example, version `1.1` just
  completed the first task of milestone 2 of verison 1. Note that, despite the
  superficial similarity, versions are not numbers: `1.10` comes after `1.9`.

Major version `0` is pre-completion; some of the parts may be
  done, but not all of it. Major version `1` is the first completed version of
  the language; from there, I'll iterate based on feedback and input from the
  users, and implement niceties.

---

###Current milestone

#### Intermediary refactor - 0.5

* [ ] Refactor to interface consistent across all bits and pieces - 0.5  
  The code is getting a little out of hand. I have some things I want to do to
    clean it up.
  * [x] Change tests to success counters - 0.5.0  
    Count total tests, count number of successes (or, equivalently, fails)
      and report that.
    * [x] Change to count successes + only display totals
    * [x] Collect successes/fails in main test method; if there are failures,
      HCF and exit with error code representing failed module.
  * [x] Consistent error handling across all functions - 0.5.1  
    For each of the files listed below:
    1. Make methods return an `unsigned long` error code. If they can't
      fail, then they'll always return success.  
      If you need to return data (e.g. ctors, getters) use an out parameter.
    2. Convert `bool` error indicators to error codes  
      Don't store error in an object, return it. (*cough* Tokenizer *cough*)
    3. Collect all error codes in `errors.h`, ensure no dupes/overlap  
      Also, a `const char *to_str(error_code)` to provide error messages
    * [x] `code_block.h`
    * [x] `data_stack.h`
    * [x] `method_map.h`
    * [x] `object.h`
    * [x] `prop_map.h`
    * [x] `stringbuilder.h`
    * [x] `token_stack.h`
    * [x] `tokenizer.h`
  * [x] TLC for `tknr_next` - 0.5.2  
    It's currently a jumbled mess. Some careful thought will be good for it,
      to simplify it as much as possible. FSA may be useful, as well as taking
      a peek at FORTH and Factor's tokenizers.
    * [x] We don't need to take `char *next_char`. That's a holdover.
    * ~~[ ] Use `unsigned char` instead of `char`.~~
    * [x] Use `skip_char` instead of `read_char`  
    It's pretty much unsalvageable :( Oh well.
  * [ ] Write a code style guide 0.5.3  
    Start by getting down the ideas, see what patterns emerge, categorize
      based on that.
    * Topics to cover:
      * `foo = malloc(sizeof(*foo))` vs. `sizeof(FooType)`
      * save old, build in `foo->bar` or build in variable, copy to `foo->bar`
      * continuation line indent for function calls
      * check for `NULL` in out params & ignore, or sometimes error?
      * `free`s return error/success? (`free()` doesn't)
      * `free`s check for null, skip if it is?
      * null-check on "this"-equivalent?
      * Order of parameters (esp. out params, and in ctors)
      * exceptions to the "all returns must be through out params and all
        functions return `ERROR`s" rule? (e.g. `_claim`)
    * [ ] Go through the code to make sure everything abides by it.
  * [ ] Ensure everything is unit tested - 0.5.4
    * [x] `StringBuilder`
    * [ ] `ctno_copy`
    * [ ] `tst_peek`?
    * Look for unused functions; those indicate obvious non-tests
  * [ ] Everything in the `TODO` comments - 0.5.5  
    ...as long as it's possible to do now; i.e. doesn't require work that's
      slated for completion in a later milestone.
  * [ ] Misc. cleanup tasks
    * [ ] Give every type its own `.h` and `.c`
    * [ ] Make sure `Object`s aren't accidentally getting passed around by
      value to avoid nasty GC-related bugs later
    * [ ] Ditto for `Tokenizer`s and file handle cleanup
    * [ ] Implement that one Token -> Object method
    * [ ] Condense the error codes and make sure they're named consistently.
    * [ ] Nested error types? (i.e. SOME_ERROR thrown b/c SOME_OTHER_ERROR)
    * [ ] Make sure everything checks return values (where there are any)

###Upcoming milestones

* [ ] `runnable.h` - 0.6  
  A combination object so I can either define things in Concaten, through
    code blocks, or in C, through functions with a certain signature, and
    call them without worrying about which is which. This layer of abstraction
    will make it much easier to implement user-defined words.
* [ ] `scope_stack.h` - 0.7  
  Contains the list of words. This is a stack so we can sensibly implement
    things like local variables. Shouldn't be too much trouble; it'll mostly
    be combining `PropMap` and `DataStack`.
* [ ] Main method - 0.8
* [ ] Minimal standard library - 0.9  
  Words like `if`, `{`, and `puts` so we can play with the language at all.
  * When designing/adding arrays, add the same methods to `CodeBlock`
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
* [ ] Command-line options - 0.10  
  Definitely at least `-e`; take inspiration from Ruby and Python.  
  These should mostly be minor variations on the existing main method.
* [ ] Documentation - 1.0  
  Including demo code, a detailed up-to-date list of every default global word
    (like `if`) and what they do, as well as a brief overview of how Concaten
    works, how to install it, and what the command-line arguments are.  
  In theory, mostly done (because of the spec), but in practice, it probably
    should be rewritten.
* [ ] Debug mode - 1.1  
  Allows breakpoints (w/ `breakpoint`?) and stops execution when exception is
    thrown outside of a `try`.
* [ ] User-created libraries - 1.2  
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

###Previous milestones

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
