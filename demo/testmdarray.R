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
  x<-array(1.01:18.01,c(3,3,2))
  r2j(x,"tt")
  y<-j2r("tt")
  cat("MD array:","\n")
  print(y)
  cat("run times is:",i,"\n")
 }
}
f(1)
f(10)
xdd<-f(10000)


