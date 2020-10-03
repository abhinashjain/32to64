Simple technique to statically detect wrong assignment of data types.

Helpful in finding the line of code that can lead to integer overflow when code written for 32-bit ported to 64-bit architecture.

For instance, change in the size of "long" data type from 4 Byte to 8 Byte in 32 bit and 64 bit architecture respectively.
This change can potentially be vulnearable if the old code (written for 32 bit) is not modified or reviewed properly before compiling it for 64 bit architecture. 

