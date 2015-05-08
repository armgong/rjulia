library(rjulia)

julia_init()
## warning don't add too much procs in test
## otherwise it will crash on low end machine
julia_eval("addprocs(2)")
for (i in 2:3)
{
 julia_void_eval(paste("r=remotecall(",i,", rand, 2, 2)",sep = ""))
 y <- j2r(" fetch(r)")
 cat("\n")
 cat("process", i, "got value:\n"); print(y)
}
julia_void_eval("rmprocs(workers())")
