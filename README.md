RJulia
======

RJula provides an interface between R and Julia. It allows a user to run a script in Julia from R, and maps objects between the two languages.

It currently supports use on Linux and Windows from the console.

Install
-------------

1. Install julia v0.3 or the current master branch; v0.2 does not have an embedded API.
2. Compile Rjulia. This requires you to set the environment variable JULIA\_SRC. A typical
Julia install directory should look like:
  
    juliainstalldir/bin/       (julia execute file)
    juliainstalldir/include    (julia include files)
    juliainstalldir/lib        (libjulia.so )
    juliainstalldir/lib/julia  (julia base package files: sys.so sys0.so etc.)
    
  if you install Julia into /usr/, change JULIA\_SRC to point to that directory. if you still have problem, please see the Makevar file in your source directoryfor more details.

3. add juliainstalldir/bin into your system PATH variable, and ensure libjulia.so is in ld.so.conf.
4. If you want to be able to type map R or Julia objects that contain NA values, install the Julia packages DataArrays and DataFrames.

Demo
-------------

please see .R files in demo dir 

Doc

to be done


Yu Gong
