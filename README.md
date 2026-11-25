# DeclLang

| <img src="https://raw.githubusercontent.com/BeAllAround/DeclLang/refs/heads/main/Logo/Logo-black-icon.png" alt="Decl Logo" width="50%" /> | <p width="50%"> In active (heavy) development </p> |
| ------------------------------------------------------------ | -------------------------------------------------- |

Decl is a dynamically-typed programming language that ***transpiles*** into a C++ API, implemented from scratch.

The primary idea is source-to-source (ahead of time) compilation. At the same time, there is also a subset of the language that runs at runtime - as an interpreter. This enforces Decl to be a language that can be run both via a VM at runtime and natively at compile time. The compiler frontend and the language backend themselves have no (external) dependencies.



The runtime version of the language allows the developer to rapidly run and test their programs without heavy optimizations, which enables rapid development with the concept being **_"Only build your code for production when you need to"_**.





## Compiler Frontend and Optimizations

The language is built based on the idea that it is not just another compiler or lexer - it is implemented from scratch, independently with the focus on high performance and portability. This way, by fully grasping the implementation of the language itself, this allows full control over the language where every single (frontend and backend) component demands attention to detail and can be optimized to its fullest potential.

The parsing implements the [recursive descent](https://en.wikipedia.org/wiki/Recursive_descent_parser) approach/algorithm.



## REPL - Interactive Shell

Decl supports the interactive shell that allows developers to run and test any Decl code on the fly.



## C++ Motivation

The motivation for most (if not all) of my research content is inspired by Decl as I have made most of these performance improvements during the development, designing, testing, and debugging of Decl. It has been a 2+ year progress as once you came up with a design, you are pretty much stuck with it forever, especially with the main focus on high performance.

To check out my YouTube channel, please visit: https://www.youtube.com/@AlexMilenkovic-ks7xd

The source code from the videos is published at https://github.com/BeAllAround/S2S-Compilers-Blog-Series-Public; related write-ups on https://dev.to/alex01.



## Why C++11 Standard for Decl?

While C++20 introduces powerful features such as `constinit` and `consteval`, which could improve the quality of the generated code, Decl deliberately avoids depending on the latest language standards.

There are three primary reasons for this decision:

- First, heavier use of new C++ features tends to increase backend compilation/build times, as modern compilers must perform more complex analysis and template instantiation.

- Second, supporting a wide range of C++ standards (C++11 and later) improves compatibility and portability for users. The Decl compiler can generate code compatible with older standards while still allowing users who desire newer features to benefit from them through their chosen C++ compiler if they take the liberty of modifying the destination source.

- Third, the Decl frontend compiler must generate clear, high-level errors itself rather than relying on the backend C++ compiler to generate those errors. Every time we leverage a particular C++ feature, the Decl compiler has to fully enforce all of its rules and constraints. In practice, this significantly increases maintenance burden and dramatically raises the overall complexity of the Decl compiler.

The focus of future work remains on strengthening the Decl compiler itself. The goal is that **"Any optimization that _can be_ performed reliably at Decl compile time should be done there"**. If an effect can be achieved using C++11, there is little reason and motivation to rely on newer, more complex language features that would burden the backend compiler and, ultimately, reduce portability.

Ultimately, whenever we optimize C or C++ code, we must examine the generated assembly. No matter how high-level the language appears, it is the low-level machine code that ultimately determines performance. This principle is shared by every programming language: the final arbiter is the machine itself. *In programming, the machine is the judge — humans are only the jury*.



## Usage Reference

It is important to note that the semi-colon is optional, and is used by developers to mark the end of an expression for code readability. This gives the developer a flexible choice - use a semi-colon if desirable, or prefer the new line `\n` as the end of a definition or expression.



### Main Program

All Decl code can run at the top level so there is no such thing as `main` function or similar. If you are really used to that kind of "main program" entry point, consider the following alternative:

```javascript
func main() {
    decl args = sys.args();
    
    sys.println(args);
    
    return 0; // or sys.exit(0);
}

main();
```



### Define a variable

```javascript
decl a = 1;
decl str = "MyDeclString";
decl arr = [1, 2, 3]; // Array Definition
decl hash_table = {'a': 1, 'b': 2 }; // Hash Table (Dictionary) Definition
```

Types like Array and HashTable are dynamic types and can also be used as expressions.



### While Loop

```javascript
decl i = 0;
while(i < 10) {
    sys.println(i);
    i++;
}
```



### If Statement

```javascript
decl i = 1;

if(i >= 2) {
    sys.println()
}
```





### Define a function

```javascript
func f1(a, b) {
    return a + b;
}
```



### Built-In Types and Optimizations

All of the built-in types are highly optimized to deliver the maximum runtime performance with various optimizations also performed at compile-time when possible.

One of these optimizations is the **ahead-of-time** optimization where the Decl frontend compiler eliminates the switch case dispatch, significally increasing the performance runtime overhead for hot and sensitive core operations such as `lessThan`, `postfixIncrement`, etc. Such optimization is applicable in the case of a literal value and if a variable is determined as **_"of one type"_** during the compile-time static analysis.

```javascript
decl a = 1; // Number: Either Float, Double, or Integer
decl str = "MyDeclString"; // String
decl boolean = true; // Boolean
decl n = nill; // Null equiavalnt in other languages. This literal represents the intentional absence of any object value
decl arr = [1, 2, 3]; // Array Definition
decl hash_table = {'a': 1, 'b': 2 }; // Hash Table (Dictionary) Definition
```







### Short-circuit Evaluation

When it comes to logical operators, Decl implements the short-circuit evaluation for the logical `AND(&&)` and `OR(||)` operators.

> For more information, refer to [Short-circuit evaluation](https://en.wikipedia.org/wiki/Short-circuit_evaluation).

```javascript
func func1() {
    sys.println("func1() called!");
    return 0;
}

func func2() {
    sys.println("func2() called!");
    return 0;
}

decl a = 0 || 2; // Holds 2

// Note: "func1() called" printed. In which case, the func2() call is never evaluated
decl output = func1() && func2();
```





### Built-In Functions and Namespaces

Decl comes with the ready-to-use bundle of standard libraries used for general purpose programming.

#### The `sys` namespace

| Function Usage                   | Description                                                  |
| -------------------------------- | ------------------------------------------------------------ |
| `sys.print(Value...)`            | Print values to stdout                                       |
| `sys.println(Value...)`          | Print values followed by a newline                           |
| `sys.eprint(Value...)`           | Print to stderr                                              |
| `sys.read_line() -> String`      | Read a full line from stdin                                  |
| `sys.args() -> Array<String> `   | Get command-line arguments at runtime                        |
| `sys.env(key: String) -> String` | Get the environment variable                                 |
| `sys.sleep(ms: Number)`          | Sleep for the timeout specified (in milliseconds)            |
| `sys.platform() -> String`       | `"linux"`, `"macos"`, `"windows"`, etc.                      |
| `sys.exit(code: Number = 0)`     | Terminates the entire program from any point in the code and returns a status code to the operating system. |



#### Filesystem (`fs` namespace)

| Function                                       | Description                                                  |
| ---------------------------------------------- | ------------------------------------------------------------ |
| `fs.read(path: String) -> String`              | Read the entire contents of a file as a string.              |
| `fs.read_bytes(path: String) -> Array`         | Read the entire contents of a file as raw bytes.             |
| `fs.write(path: String, content: String)`      | Write a string to a file, overwriting it if it already exists. |
| `fs.write_bytes(path: String, content: Array)` | Write raw bytes to a file, overwriting it if it already exists. |
| `fs.append(path: String, content: String)`     | Append a string to the end of a file (creates the file if it does not exist). |
| `fs.exists(path: String) -> Boolean`           | Return whether a file or directory exists at the given path. |
| `fs.is_file(path: String) -> Boolean`          | Return whether the path exists and is a regular file.        |
| `fs.is_dir(path: String) -> Boolean`           | Return whether the path exists and is a directory.           |
| `fs.list(path: String) -> Array<String>`       | List the names of entries in a directory.                    |
| `fs.mkdir(path: String)`                       | Create a directory at the given path.                        |
| `fs.remove(path: String)`                      | Delete a file or empty directory.                            |
| `fs.rename(from: String, to: String)`          | Rename or move a file or directory.                          |



#### Time (`time` namespace)

| Function                                         | Description                                                  |
| ------------------------------------------------ | ------------------------------------------------------------ |
| `time.now() -> Number`                           | Return the current Unix timestamp in milliseconds.           |
| `time.now_iso() -> String`                       | Return the current time as an ISO 8601 formatted string.     |
| `time.sleep(ms: Number)`                         | Pause execution for the given number of milliseconds (or keep this only in sys). |
| `time.format(ts: Number, fmt: String) -> String` | Format a Unix timestamp according to the given format string. |
| `time.parse(s: String, fmt: String) -> Number`   | Parse a formatted time string into a Unix timestamp (milliseconds). Returns None on failure. |
