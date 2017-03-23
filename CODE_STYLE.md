###Guiding principles

1. Error tracking must be possible. If an error happens, it has to be reported
   to the caller, so the caller can either report the exception up the chain
   or deal with the errors. For all functions declared in header files:
   * If an error is _impossible_, and the function is checking a boolean
     condition, a `bool` can be returned.
   * If the function is a `claim` function, it can return a pointer to the
     claimed resource, or `NULL` if and only if the maximum refcount has been
     reached.
   * In all other cases, a function returns an `ERROR` and returns other data
     through out params.
2. KISS -- both in terms of code maintenance and cleaning up at runtime, and
   finding potential errors as quickly and painlessly as possible.
    * `FooType *f = malloc(sizeof(*foo))`, rather than
      `FooType *f = malloc(sizeof(FooType))`
    * When replacing something, create the replacement in a variable, *then*
      copy it into its destination, rather than creating it in the destination
      and having to copy the original back if you need to revert.
    * Always perform `NULL` checks on passed-in pointers. If `NULL` shouldn't
      have been passed, return an error.
      * out params, in constructors or elsewhere, should never be `NULL`, so
        return an error immediately if `NULL` is passed in.
    * Use a single error handler if possible; whenever an error occurs, set
      the error to return and `goto error_handler`.
3. Use standard library conventions where possible.
    * `void foo_free(Foo *f)` will just silently return immediately if `NULL`
      is passed in.

###Style
* Indent with spaces, and spaces only.
* When continuing a line:
  * If continuing a parameter list or parenthetical expression, align with the
    innermost opening parenthesis.
  * If continuing an expression that's not wrapped in parentheses, align with
    the first character of the expression.
* When defining a function:
  * If it's a "constructor" (i.e. a function that creates an instance of a
    certain type), the out param for the created object is last.
  * If it's a "method" (i.e. a function that's associated with a certain type)
    then the `this` equivalent is the first argument, and the "real" arguments
    are after that.
  * Out parameters are always last.
* Open braces on the same line as their associated statement/expression/etc.
* Lines are no more than 79 actual characters (80 including LF newline)
* One space around binary operators, none between a unary operator and its
  operand.
* 