## Types

A1 programming language primarily strives for simplicity and ease of use among less experienced developers or even non developers. Therefore, its syntax is the most similar to Python programming language but with integrated type safetiness and also integrated, though optional, type declarations.

### Basic types

Basic types listed below are perfectly suited for users that do not have much of experience in programming.

| Type | Description |
| - | - |
| num | Represents both integers and decimals |
| bool | Represents either true or false |
| str | Represents text, i.e. any series of characters |

Furthermore, in some cases it is useful to specify integers of a specific size. Therefore, we offer additional integer types:

| Type | Description |
| - | - |
| uint8 / int8 | Represents a number of a size of 1 byte |
| uint16 / int16 | Represents a number of a size of 2 bytes |
| uint32 / int32 | Represents a number of a size of 4 bytes |
| uint64 / int64 | Represents a number of a size of 8 bytes |

### Complex types

Besides few basic types, A1 supports complex types as well. These are useful for developing more complex solutions.

| Type | Description |
| - | - |
| array[\<type>] | Represents a collection of objects of a specified type. E.g. `array[num]` or `array[str]` |
| map[\<type>, \<type>] | Represents a collection of key - value pairs. E.g. `map[num, str]` |