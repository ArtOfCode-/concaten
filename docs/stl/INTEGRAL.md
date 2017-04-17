## integral method documentation

### Implemented

* `>string` - Converts the integer to its base-10 string representation.  
  **Effect:** `[integral]>[string]`  
  **Equivalent:** `>string-d`
* `+` - Adds two numbers. Second on stack can be either a real or integral,
  but always returns a real.  
  **Effect:** `[real],[integral]>[real]` or `[integral],[integral]>[integral]`
* `*` - Multiplies two numbers. Second on stack can be either a real or
  integral, but always returns a real.  
  **Effect:** `[real],[integral]>[real]` or `[integral],[integral]>[integral]`
* `-` - Subtracts the top of stack from the second on stack. Second on stack
  can be either a real or integral, but always returns a real.  
  **Effect:** `[real],[integral]>[real]` or `[integral],[integral]>[integral]`
* `/` - Divides the second on stack by the top of stack. Second on stack can
  be either a real or integral, but always returns a real.  
  **Effect:** `[real],[integral]>[real]` or `[integral],[integral]>[integral]`
* `mod` - Takes the second on stack mod the top. Note that this is a
  mathematical modulus, not modulo as done by `%` in C. The second can either
  be an integral or a real  
  **Effect:** `[real],[integral]>[real]` or `[integral],[integral]>[integral]`

### Not yet implemented / Incomplete

* `>string-b` - Converts the integer to its base-2 string representation.  
  **Effect:** `[integral]>[string]`
* `>string-o` - Converts the integer to its base-8 string representation.  
  **Effect:** `[integral]>[string]`
* `>string-x` - Converts the integer to its base-16 string representation.  
  **Effect:** `[integral]>[string]`

