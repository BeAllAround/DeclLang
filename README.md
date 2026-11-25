# DeclLang

| ![Decl](https://raw.githubusercontent.com/BeAllAround/S2S-Compilers-Blog-Series-Public/refs/heads/main/Logo-DeCL_black-icon01.png) | In active (heavy) development |
| ------------------------------------------------------------ | ----------------------------- |



Decl is a dynamically-typed programming language that ***transpiles*** into a C++ API, implemented from scratch.

The primary idea is source-to-source (ahead of time) compilation. At the same time, there is also a subset of the language that runs at runtime - as an interpreter. This enforces Decl to be a language that can be run both via a VM at runtime and natively at compile time. The compiler frontend and the language backend themselves have no dependencies.



The runtime version of the language allows the developer to rapidly run and test their programs without heavy optimizations, which enables rapid development with the concept of **_"Only build your code for production when you need to"_**.





## Compiler Frontend and Optimizations

The language is built based on the idea that it is not just another compiler or lexer - it is implemented from scratch, independently with the focus on high performance and portability. This way, by fully grasping the implementation of the language itself, this allows full control over the language where every single (frontend and backend) component demands attention to detail and can be optimized to its fullest potential.

The parsing implements the [recursive descent](https://en.wikipedia.org/wiki/Recursive_descent_parser) approach/algorithm.



## REPL - Interactive Shell

Decl supports the interactive shell that allows developers to run and test any Decl code on the fly.



## Usage Reference

It is important to note that the semi-colon is optional, and is used by developers to mark the end of an expression for code readability. This gives the developer a flexible choice - use a semi-colon if desirable, or prefer the new line `\n` as the end of a definition or expression.



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
    sys.print(i);
    i++;
}
```







### Define a function

```javascript
func f1(a, b) {
    return a + b;
}
```



### Built-In Functions and Namespaces

Decl comes with the ready-to-use bundle of standard libraries used for general purpose programming.

These include the following:





