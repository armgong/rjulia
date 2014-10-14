library(rjulia)
# julia_home disable_gc =false parallel=T
# this init make julia can start multi-process for distribute computing
julia_init("d:/codes/julia32/usr/bin",T,T)
julia_eval("addprocs(3)")
for (i in 1:4)
{
 julia_void_eval(paste("r=remotecall(",i,", rand, 2, 2)",sep=""))
 y<-j2r(" fetch(r)")
 cat("\n")
 cat(paste("process ",i," get value:\n",sep=""))

 print(y)
}

