## Global function documentation

### Implemented

* `*puts` - Prints any object, by using that objects `>string` method.  
  **Effect:** `*>`

### Not yet implemented / Incomplete

* `{` - Finds the matching `}`, ignoring all other brace types, adding every
  token until then to a code block that it pushes onto the stack.
  **Effect:** Irregular.
* `[` - Finds the matching `]`, ignoring all other brace types, adding every
  token until then to a code block, which it runs. Anything added to the stack
  is removed from the stack and pushed into a list
* `->` - Stores a variable in the lowermost scope. The second must match the
  type provided as the first.  
  **Effect:** `*,[type]>`
* `[int]` - Pushes the `[int]` type to the stack.  
  **Effect:** `>[type]`
* `[string]` - Pushes the `[string]` type to the stack.  
  **Effect:** `>[type]`
* `[type]` - Pushes the `[type]` type to the stack.  
  **Effect:** `>[type]`
* `[real]` - Pushes the `[real]` type to the stack.  
  **Effect:** `>[type]`
