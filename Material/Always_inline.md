No, GCC's __attribute__((always_inline)) does not provide an absolute guarantee that a function will be inlined, although it is significantly more forceful than the standard inline keyword. 
Stack Overflow
Stack Overflow
 +1
Key Limitations
While the attribute instructs the compiler to ignore most cost-benefit heuristics (such as code size thresholds), inlining will still fail in the following scenarios: 
Recursive Calls: A function cannot be inlined into itself infinitely.
Indirect Calls: If a function is called via a function pointer or is a virtual function, the compiler often cannot determine the target at compile time to perform the inlining.
Missing Definition: The function's definition must be visible to the compiler in the current translation unit. Without Link Time Optimization (LTO), functions defined in other source files cannot be inlined.
Conflicting Attributes: Inlining may be blocked if the caller and callee have incompatible target features (e.g., trying to inline an AVX-512 function into a function not compiled for that architecture).
Variable-length Arrays (VLAs): Functions using VLAs or certain alloca patterns may sometimes be rejected for inlining depending on the GCC version. 
LLVM Discussion Forums
LLVM Discussion Forums
 +7
Behavior and Error Reporting
Mandatory Diagnostic: Unlike the inline hint, if GCC is unable to inline a function marked always_inline, it will typically issue a compile-time error or warning (e.g., -Wattributes) explaining why.
Optimization Levels: While standard inline often requires at least -O1 to take effect, always_inline will attempt inlining even at -O0.
Best Practice: To ensure the highest chance of success, always pair the attribute with the inline and static keywords (e.g., static inline __attribute__((always_inline))) to limit visibility and provide the definition to the compiler. 
Stack Overflow
Stack Overflow
 +2
[RFC] Avoid inlining alwaysinline functions when they cannot ...
Jun 11, 2024 — [RFC] Avoid inlining alwaysinline functions when they cannot be inlined * sdesmalen-arm June 11, 2024, 3:48pm 1. As the name sugge...

LLVM Discussion Forums
GCC still finicky about inlining functions? : r/C_Programming
Nov 25, 2021 — GCC is entirely within its rights not to honour your request (using inline ) to inline a function if it thinks you're being silly.

Reddit
How do I force gcc to inline a function? - Stack Overflow
Dec 5, 2011 — * 44. GCC uses code size as heuristic to determine whether something will be inlined. I have a networking/serialization library wh...

Stack Overflow
Difference between __always_inline and inline - Stack Overflow
Jan 8, 2014 — 1. inline is standard and __always_inline looks like a compiler extension only for certain compilers. Btw inline is a suggestion t...

Stack Overflow
How can I tell gcc not to inline a function? - Stack Overflow
Sep 24, 2009 — (That won't work if you're using -flto to allow cross-file inlining with link-time optimization, which you should for production b...

Stack Overflow
Is the GNU C++ compiler ((GCC) 5.2.0) good at "inlining" by itself?
Aug 30, 2015 — * If you implement a non inline function at a header you'll get a linker error for multiply defined symbol when including it from ...

Stack Overflow
[[always_inline]] attribute - Google Groups
And the GCC documentation explicitly covers this, saying "GCC does not inline any functions when not optimizing unless you specify...

Google Groups
warning: always_inline function might not be inlinable [-Wattributes]
Sep 7, 2015 — * 4 Answers. Sorted by: 62. For functions declared inline(!!!!), this attribute inlines the function independent of any restrictio...

Stack Overflow
Preventing gcc from inlining a function [duplicate] - Stack Overflow
Nov 2, 2011 — Please explain your problem more precisely. Multiple instances of a label? This should never happen. Which version of gcc is givin...

Stack Overflow
GCC and clang have __attribute__((always_inline)). VC++ ...
GCC and clang have __attribute__((always_inline)). VC++ has __forceinline. All o... | Hacker News. ... GCC and clang have __attrib...

Hacker News
Inline Functions (C++) - Microsoft Learn
Feb 6, 2026 — The __forceinline keyword (or [msvc::forceinline] attribute) overrides the cost-benefit analysis and relies on the judgment of the...

Microsoft Learn
Inline Transformation - emmtrix Wiki
Sep 15, 2025 — Compilers often refuse to inline functions that are “too large” because the benefit doesn't justify the cost. In fact, most compil...

emmtrix Technologies

How to use Swift's @inline
Oct 4, 2023 — Warning: The compiler treats @inline(__always) as a strong directive, but it's not absolutely guaranteed. In some complex cases (l...

Reintech.io

Why does the GNU Compiler Collection (GCC) not inline ...
Dec 23, 2022 — As I recall things: In C and C++ the inline keyword does not require the compiler to inline the code, it is a “strong hint and req...

Quora
