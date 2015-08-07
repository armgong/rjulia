library(rjulia)

julia_init()

x <- 1:3
x1 <- c("hello","world")
y <- matrix(1:12,c(3,4))
z <- list(x,x1,y)

r2j(z,"tupletest")
y <- j2r("tupletest")
y
if(FALSE) ## FAILS, as y has  1d-arrays instead of vectors
stopifnot(identical(y, z))

arr1d2vec <- function(a) {
    if(is.atomic(a)) {
        if(length(dim(a)) == 1)
            dim(a) <- NULL
        a
    } else if(is.list(a)) {
        lapply(a, arr1d2vec)
    } else
        stop("'a' not atomic or list, not yet supported")
}
stopifnot(identical(z, arr1d2vec(y)))

zz <- list(x,x1,z, arr1d2vec(y))
r2j(zz,"tupletst2")
yy <- j2r("tupletst2")
stopifnot(identical(zz, arr1d2vec(yy)))

#Julia Tuple to R list
julia_eval("(3,)")
julia_eval("(3,5)")

### List with *names*
str(nz <- list(x=x, x1=x1, m = matrix(1:12, 3,4), A = array(1:24, dim=2:4)))
r2j(nz, jnm <- "nmd_tupletst")
nz2 <- j2r(jnm)
str(nz2) ## list names are lost -- FIXME
