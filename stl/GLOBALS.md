## Global function documentation

### Implemented

* `*puts` - Prints any object, by using that objects `>string` method.  
  **Effect:** `*>`
* `{` - Finds the matching `}`, ignoring all other brace types, adding every
  token until then to a code block that it pushes onto the stack.  
  **Effect:** token: irregular; data: `>[code-block]`

### Not yet implemented / Incomplete

* `[` - Finds the matching `]`, ignoring all other brace types, adding every
  token until then to a code block, which it runs. Anything added to the stack
  is removed from the stack and pushed into a list  
  **Effect:** token: irregular; data: `>[list]`
* `->` - Stores a variable in the lowermost scope. The second on stack must
  match the type on top of the stack.  
  **Effect:** data: `*,[type]>`; token: `[identifier]>`.
* `[integral]` - Pushes the integral type to the stack.  
  **Effect:** `>[type]`
* `[real]` - Pushes the real type to the stack.  
  **Effect:** `>[type]`
* `[string]` - Pushes the `[string]` type to the stack.  
  **Effect:** `>[type]`
* `[type]` - Pushes the `[type]` type to the stack.  
  **Effect:** `>[type]`
