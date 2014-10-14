library(rjulia)
#init embedding julia,paraments are julia_home and disable_gc
julia_init("d:/codes/julia32/usr/bin")
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
