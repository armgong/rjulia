library(rjulia)

julia_init()

f <- function(n) {
    stopifnot( n >= 1)
    for (i in 1:n) {
	## pass R array to Julia
	x <- array(1.01:18.01, c(3,3,2))

	st <- system.time({
	    ## pass R matrix to Julia
	    r2j(x,"tt")
	    ## and get it passed back from Julia
	    y <- j2r("tt")
	})
	cat("MD array (rank 3) passed to julia and back: ")
	stopifnot(identical(y, x))
	cat(sprintf("[Ok].  Elapsed system.time(): %g\n", st[["elapsed"]]))
    }
}

f(1)
#f(10)
#xdd <- f(10000)


