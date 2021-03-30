#About

This is team school assignment to showcase possibilities of parallelism using OpenCL.

It is divided into 3 subprojects:
- Matrix multiplication in C using serial algorithm as well as different types of WG/WI for parallel execution
- Edge Detection on unfocused Images using Sobel and Laplacian filters (algorithms)
- Measuring efficiency of password strength by difficulty and final number of attempts for password breaking using brute force

# Prerequisites

###Windows: 
You would need to have [MinGW](http://mingw-w64.org/doku.php/download) or [Cygwin](https://cygwin.com/install.html) installed on your device.

- In your SDE install **gcc** and **cmake**
- To run programs using OpenCL you would need to download native OpenCL implementation for you device
  - Nvidias GPU: https://developer.nvidia.com/opencl
  - Intel CPUs: https://software.intel.com/content/www/us/en/develop/articles/opencl-drivers.html
    
# Running Program 

Once you setup your environment you may run your program.

###Compile

Run cmake against CMakeLists.txt

###Running

Execute built target ***matrix_multiplicator*** (Yes I see that typo :P)

#Contribution

Feel free to contribute after April 2021. ty ❤

