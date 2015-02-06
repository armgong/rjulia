RJulia / rjulia
===============

RJulia (`rjulia` is the name of the R package, `RJulia` the github
repository name) provides an interface between R and Julia. It allows a user to run a script in Julia from R, and maps objects between the two languages.

It currently supports use on Linux and Windows from the console, but build on Windows only for advance users.

Install
-------------

1. Install julia v0.33 or the current master branch; R version >=3.1.0.

  **WARNING: due to julia api rapidly change between releases, each time you upgrade or downgrade julia, rjulia need to be recompiled and reinstalled.**
  
   **WARNING: due to https://github.com/JuliaLang/julia/issues/10085, rjulia not stable on julia 0.34 0.35 or 0.3 head branch, please install 0.33 or 0.4-dev.**

2. add <juliainstalldir>/bin to your system PATH variable if needed.

3. "Compile" RJulia.

4. If you want to be able to use R or Julia objects that contain NA values
   or factors or data frames, the Julia packages `DataArrays` and `DataFrames` must be installed.

5. Windows user please download built binary package from https://github.com/armgong/rjulia/releases  **(Caution: use master branch build first, nextgen branch build not stable)**

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


**Important Information**
-------------
We now develop on two branches. The `master` branch is the stable version,
the `nextgen` branch is the development version. The `nextgen` branch uses
the pure julia C API to call a julia function, don't mix julia script and c code,
**so it is faster than the master branch and uses less memory, but it is less stable**.

If you want to use the `nextgen` branch, please ensure `DataFrames` (julia) package
version >= 0.60.
