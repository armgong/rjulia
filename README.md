RJulia
======

RJula is a  R to Julia interface. it provides an interface between R and Julia. It allows to create Array, run script  in julia  from R.


currently it can be use on unix and windows console,on windows gui it crashed.

Install

1 need install julia v0.3 or git master branch,becase julia v0.2 no embedding API

2 Rjulia will need Set the environment variables JULIA_SRC, 

  if you install julia into /usr then export JULIA_SRC=/usr should be ok,

  if still have problem,please see the file Makevar in src dir.

3 add juliainstalldir\bin into your system PATH variable

Demo

please see two txt files in demo dir 

Doc

to be done


Yu Gong
