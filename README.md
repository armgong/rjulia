RJulia
======

RJula provides an interface between R and Julia. It allows to run script  in julia  from R,and object mapping between R and Julia.


currently it can be use on unix and windows console,on windows gui it crashed.

Install

1 need install julia v0.3 or git master branch,because v0.2 no embedding API.

2 compile Rjulia will need Set the environment variables JULIA_SRC, 
  
  typical julia install dir should like this 
  
    juliainstalldir/bin/       (julia execute file)
    juliainstalldir/include    (julia include files)
    juliainstalldir/lib        (libjulia.so )
    juliainstalldir/lib/julia  (julia base package files: sys.so sys0.so etc.)
    
  if you install julia into /usr then export JULIA_SRC=/usr should be ok,

  if still have problem,please see the file Makevar in src dir for more details.

3 add juliainstalldir/bin into your system PATH variable, and ensure libjulia.so in your ld.so.conf.

4 if you want typemapping R and julia object that contain NA value, install julia package DataArrays and DataFrames

Demo

please see .R files in demo dir 

Doc

to be done


Yu Gong
