library(rjulia)
julia_init()

f <- function(n) {
    stopifnot(n >= 1)
    for (i in 1:n) {
        ## pass R double vector to Julia
        x <- matrix(1.01:6.01,3,2)
        r2j(x,"tt")
        y <- j2r("tt")
        cat("i=",i,"; Matrix, passed to Julia and back to R:\n", sep="")
        print(y)
        ## cat("run time is:",i,"\n")
        cat(" is equal to original R? ", paste(all.equal(x,y), collapse="\n   "), "\n")
    }

    ## create 2d array in julia,get from R
    julia_void_eval("x = rand(2,2)")
    yy <- j2r("x")
    cat("Matrix from Julia's  rand(2,2):\n")
    print(yy)
}

f(1)
#f(10)
#xdd <- f(10000)
