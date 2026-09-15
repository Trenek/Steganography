# How to compile?
`cmake -B build`  
`cmake --build build --target all`

# How to run it?
`./Steganography encode <number of LSB to use> <bite image> <secret image> <result image>`  
`./Steganography decode [-b <number of LSB to use> | -a <algorithm>] <suspect image> <result image>`  

There are only two algorithms:
* `en` - entrophy based
* `sp` - spacial based
