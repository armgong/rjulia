library(rjulia)
#init embedding julia,paraments are julia_home and disable_gc
julia_init("d:/codes/julia32/usr/bin")
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


