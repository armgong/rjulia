RJulia / rjulia
===============

RJulia (`rjulia` is the name of the R package, `RJulia` the github
repository name) provides an interface between R and Julia. It allows a user to run a script in Julia from R, and maps objects between the two languages.

It currently supports use on Linux and Windows from the console, but build on Windows only for advance users.

Install
-------------

1. Install julia v0.3x or julia v0.4 and R version >=3.1.0.
 
2. add <juliainstalldir>/bin to your system PATH variable if needed.

3. Choose rjulia branch and get source code

  **rjulia now have two branchs,user need select which branch to use**
  
    *   master branch support julia v0.3x.
  
    *   0.4 branch support julia dev 0.4.

4. "Compile" RJulia.

5. If you want to be able to use R or Julia objects that contain NA values or factors or data frames, the Julia packages `DataArrays` and `DataFrames` must be installed.

6. Windows user please download built binary package from https://github.com/armgong/rjulia/releases 

Simple Run it
-------------
library(rjulia)

julia_init() #**(will auto find your julia home)**

julia_eval("1+1")

Demo
-------------

please see the `*.R` files in the `demo/` directory, or use
```
	demo(package = "rjulia")
```


Doc
-------------
Help files are now done, mostly with examples.


**Know Problem**
-------------
   **julia api rapidly change between releases, each time you upgrade or downgrade julia, rjulia need to be recompiled and reinstalled.**

   **due to https://github.com/JuliaLang/julia/issues/10085, rjulia  master branch may crash when run certain (testparallel etc.) julia code on julia 0.35 or 0.3 head branch,but most demo run ok.**
