library(rjulia)
#init embedding julia,paraments are julia_home and disable_gc
if(.Platform$OS.type == "unix") {
 julia_init("/usr/bin",F,T)  
}
else
{  
 if (.Platform$r_arch=="x64")   
  julia_init("c:/julia/bin",F,T)
 else
   julia_init("c:/julia32/bin",F,T)  
}

f=function(n){
 for (i in 1:n)
 {   
  #pass R double vector to Julia
  x<-matrix(1.01:6.01,3,2)
  r2j(x,"tt")
  #get passed vector from Julia
  y<-j2r("tt")
  cat("Matrix:","\n")
  print(y)
  cat("run time is:",i,"\n")
 }
 #create 2d array in julia,get from R
 julia_void_eval("x = rand(2,2)")
 yy<-j2r("x")
 cat("Matrix:","\n")
 print(yy)
}
f(1)
f(10)
xdd<-f(10000)
