RJulia
======

RJula provides an interface between R and Julia. It allows a user to run a script in Julia from R, and maps objects between the two languages.

It currently supports use on Linux and Windows from the console,but build on Windows only for advance users.

Install
-------------

1. Install julia v0.3 or the current master branch; v0.2 does not have an embedded API; R version >=3.1.0.
2. Compile Rjulia. This requires you to set the environment variable JULIA\_SRC. A typical
Julia install directory should look like:
  
  juliainstalldir/bin/       (julia execute file)
  juliainstalldir/include    (julia include files)
  juliainstalldir/lib        (libjulia.so )
  juliainstalldir/lib/julia  (julia base package files: sys.so sys0.so etc.)

  if you install Julia into /usr/, change JULIA\_SRC to point to that directory. if you still have problem, please see the Makevar file in your source directoryfor more details.

3. add juliainstalldir/bin into your system PATH variable, and ensure libjulia.so is in ld.so.conf.

4. If you want to be able to type map R or Julia objects that contain NA values or factor or Data Frame, Julia packages DataArrays and DataFrames must be installed.

5. Windows user please download built binary package from https://github.com/armgong/RJulia/releases

 

Demo
-------------

please see .R files in demo dir 

Doc

to be done

**Important Information**
-------------
now develop on two branch,master branch is stable version,nextgen branch is development version.nextgen branch uses pure julia C API to call julia function, don't mix julia script and c code,*so it  fast than master branch and use less memory ,but  less stable*.

if you want use nextgen branch, please patch your DataFrames package, add one function into it:
```julia
# Initialize an empty DataFrame with specific eltypes and names and whether is pooled data array
function DataFrame(column_eltypes::Vector, cnames::Vector,ispda::Vector, nrows::Integer)
    p = length(column_eltypes)
    columns = Array(Any, p)
    for j in 1:p
      if ispda[j]
        columns[j] = PooledDataArray(column_eltypes[j], nrows)
      else
        columns[j] = DataArray(column_eltypes[j], nrows)
      end  
    end
    return DataFrame(columns, Index(cnames))
end



Yu Gong
