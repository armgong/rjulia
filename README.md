RJulia / rjulia
===============

RJulia (`rjulia` is the name of the R package, `RJulia` the github
repository name) provides an interface between R and Julia. It allows a user to run a script in Julia from R, and maps objects between the two languages.

It currently supports use on Linux and Windows from the console, but build on Windows only for advance users.

Installing
-------------

1. Install julia v0.3x or julia v0.4 and R version >=3.1.0.
 
2. Add `<juliainstalldir>/bin` to your system PATH variable if needed.

3. Install the rjulia package. If you're using the v0.3x branch of julia, use the "master" branch of rjulia.  If you're using the v0.4 development branch of julia, use the "0.4" branch of rjulia.

    You can install rjulia using the devtools package:

    ```r
    install.packages("devtools") #if not already installed
    devtools::install_github("armgong/RJulia", ref="master")# or ref="0.4" if using Julia v0.4
    ```
    Or you can install manually by downloading the source and building the package. Windows users can download built binary packages from https://github.com/armgong/rjulia/releases.

4. If you want to be able to use R or Julia objects that contain NA values or factors or data frames, the Julia packages `DataArrays` and `DataFrames` must be installed.

Simple example 
-------------

```r
library(rjulia)
julia_init() #**(will auto find your julia home)**
julia_eval("1+1")
```

Demo
-------------

Please see the `*.R` files in the `demo/` directory, or use
```r
	demo(package = "rjulia")
```


Docs
-------------
Help files are now done, mostly with examples.


Know Problems
-------------
   * The Julia api rapidly changes between releases. Each time you upgrade or downgrade Julia, rjulia needs to be recompiled and reinstalled, e.g. with `devtools::install_github`. 

   * Due to https://github.com/JuliaLang/julia/issues/10085, the rjulia master branch may crash when running certain (testparallel etc.) Julia code on Julia 0.3.5 or the release-0.3 branch. Most demos run ok.
   
   *Due to RStudio issue (https://github.com/armgong/RJulia/issues/16), when use RStudio and rjulia,there are two choices:

       1 when use julia 0.39 and rjulia, please stay on RStudio-0.98.1103 http://download1.rstudio.org/RStudio-0.98.1103.zip
    
       2 when use RStudio 0.99.442 , need julia 0.4 and rjulia 0.4 branch
