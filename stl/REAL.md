## `integral` method documentation

### Implemented

* `>string` - Converts the real number to its base-10 string representation.  
  **Effect:** `[real]>[string]`  
* `+` - Adds two numbers. Second on stack can be either a `real` or
  `integral`, but always returns a `real`.  
  **Effect:** `[real],[real]>[real]` or `[integral],[real]>[real]`
* `*` - Multiplies two numbers. Second on stack can be either a `real` or
  `integral`, but always returns a `real`.  
  **Effect:** `[real],[real]>[real]` or `[integral],[real]>[real]`
* `-` - Subtracts the top of stack from the second on stack. Second on stack
  can be either a `real` or `integral`, but always returns a `real`.  
  **Effect:** `[real],[real]>[real]` or `[integral],[real]>[real]`
* `/` - Divides the second on stack by the top of stack. Second on stack can
  be either a `real` or `integral`, but always returns a `real`.  
  **Effect:** `[real],[real]>[real]` or `[integral],[real]>[real]`
* `mod` - Takes the second on stack mod the top. Note that this is a
  mathematical modulus, not modulo as done by `%` in C. The second can either
  be an integer  
  **Effect:** `[real],[real]>[real]` or `[integral],[real]>[real]`

### Not yet implemented / Incomplete
