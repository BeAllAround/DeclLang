TODO: TRY THIS TO SEE IF THE SIZE OF THE EXECUTABLE INCREASES - it def should but then we don't need all the cpp dlls for that machine?! - THAT IS CORRECT AND THE SIZE OF THE EXECUTABLE INCREASES SINCE ALL THAT IS NEEDED IS PACKED INTO ONE BINARY EXECUTABLE (edited) 

Default behavior: Major compilers typically perform dynamic linking to the standard runtime library by default, though you can force static linking using specific compiler flags (like -static-libstdc++).
