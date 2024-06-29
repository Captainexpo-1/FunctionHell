# λ++
## Overview
λ++ is a programming language focused entirely on functions. Variables in λ++ are assigned exclusively to functions, similar to lambda calculus or the lambda/anonymous functions found in other languages.

## Syntax
Function Definition
In λ++, functions are defined and assigned to variables with a specific syntax. Here is the general structure:

```go
var <return_type> <variable_name> = <return_type> <(<parameter_type> <parameter_name>, ...)> {
    <function_body>
}
```
Here is an example demonstrating the assignment of a function that adds two integers:

```go
var int x = int <int a, int b> { 
    ret a + b
}
```

`var <return_type> <variable_name>`: Declares a variable with the specified return type and name.
Example: var int x declares a variable x that returns an integer.

### Function Assignment:
`var int x = int <int a, int b> { ret a + b }` assigns a function that takes two integers a and b, adds them, and returns the result.

Variables can be reassigned to new functions, but the return type must match the original declaration.


### Function Body:

The function body contains the code to be executed when the function is called. It follows the same general structure as found in other programming languages, but with simplified syntax.
Example: `{ ret a + b }` contains the logic to return the sum of a and b.

### DataTypes:

λ++ supports the following data types:

- `int`: Integer data type `42`
- `float`: Floating-point data type `3.14`
- `string`: String data type `"hello, world"`
- `bool`: Boolean data type, `true` or `false`
- `void`: Void data type (no return value)
- `type[]`: List of the specified type `{1, 2, 3}`


### If-Else Statements:

λ++ supports if-else statements for conditional execution of code. Here is the general structure:

```go
if (<condition>) {
    <code_block>
}
else if (<condition>) {
    <code_block>
}
else {
    <code_block>
}
```

### Function Call:

To call a function, use the following syntax:

```go
<function_name> with (<parameter_value>, ...)
```

When a variable is accessed, it's function will be automatically called with no parameters

```go
var int x = int <> { ret 42 }

x // 42
```

### Logging:

To print output to the console, use the `log` function:

```go
log with (string <> { ret "Hello, World!"}) // Outputs: Hello, World!
```

### Comments:

Comments in λ++ are denoted by `//` and are used to provide additional information or context within the code.

```go
// This is a comment
```

## More Examples
Function to Subtract Two Numbers
```go
var int subtract = int <int a, int b> { 
    ret a - b
}
```
Function to Multiply Two Numbers
```go
var int multiply = int <int a, int b> { 
    ret a * b
}
```
Function with No Parameters
```go
var int constant = int <> { 
    ret 42
}
```

The classic "fizzbuzz" program

```go
var void fizzbuzz = void <int n, int j> {
    if (j % 15 == 0) {
        log with (string <> { ret "FizzBuzz" })
    }
    else if (j % 5 == 0){
        log with (string <> { ret "Buzz" })
    }
    else if (j % 3 == 0){
        log with (string <> { ret "Fizz" })
    }
    else {
        log with (j)
    }
    if (j < n) {
        ^fizzbuzz with (int <> { ret n } , int <> { ret j + 1 })
    }
}

fizzbuzz with (int <> { ret 100 }, int <> { ret 0 })
```


## Standard Library

The standard library provides many functions that make life easier for the programmer. The full list includes

- `log`: Print to stdout `log with (x)` equivalent to `print(x())` in python
- `append`: Append to list or string `append with (x, val)` equivalent to `x.append(val)` in python
- `at`: Returns the value at the specified index in a string or list `at with (x, index)` equivalent to `x[index]` in c++, python, etc. 
- `sin, cos, tan`: Returns the sin, cosin, or tangent of some radian value
- `abs`: Returns the absolute value of the provided number
- `parseint`: Converts a string to an integer
- `parsefloat`: Converts a string to a float
- `len`: Returns the length of a string or list
- `str`: Converts a number to a string
- `round`: Rounds a float to the nearest integer
- `floor`: Rounds a float down to the nearest integer
- `ceil`: Rounds a float up to the nearest integer

The standard library also provides a limited selection of constants

- `PI`: equivalent to: `var int PI = int <> { ret 3.14159265 }`