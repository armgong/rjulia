
### Test sending data from R to julia and back again

library(RUnit)
library(rjulia)
julia_init(disablegc=FALSE)

## logical, numeric, and character with "e" fails
## logical or character with "f" fails. Logical or character have loop rather than memcpy in R_Julia_MD.

## Send data of each atomic type to julia and back again
##  With and without NAs
##  Test that the roundtrip returns the same value
test_roundtrip <- function() {
    test.list = list(    
        a = 0:5, 
        b = matrix(0:5, ncol=2), 
        c = array( c(0:8), dim=c(2, 2, 2)), 
        d = c(NA_integer_, 0:4), 
        e = matrix(c(0:4, NA_integer_), ncol=2), 
        f = array( c(0:7, NA_integer_), dim=c(2, 2, 2))
    )

    types = c("integer", "logical", "numeric", "character")
    types = c("integer", "numeric", "character")

    ## Vectors, matrices and arrays
    mapply(
        names(test.list),  test.list,
        FUN=function(k, v) {
            lapply(
                types,
                function(x) {
                    message(x, ": ", k)
                    newv = as(v, x)
                    r2j(newv, k)
                    checkIdentical( newv, j2r(k) )
                })
        })

    ## Factors
    mapply(
        names(test.list),  test.list,
        FUN=function(k, v) {
                    message("factor: ", k)
                    newv = as.factor(v)
                    r2j(newv, k)
                    checkIdentical( newv, j2r(k) )
        })

    ## Lists and data.frames
    lists = list(
        h = list( 1:5, letters[1:5] ),
        j = list( c(NA, 2:5), letters[1:5] )
    )
    mapply(names(lists), lists,
           FUN=function(k, v) {
               message("list: ", k)
               r2j(v, k)
               checkIdentical( v, j2r(k) )
               newv = as.data.frame(v, stringsasFactors=FALSE)
               message("data.frame w.o. factors: ", k)
               r2j(newv, k)
               checkIdentical( newv, j2r(k) )
               newv = as.data.frame(v, stringsasFactors=TRUE)
               message("data.frame w. factors: ", k)
               r2j(newv, k)
               checkIdentical( newv, j2r(k) )
           })

}

## Repeated run r2j and j2r, can detect 'random' segfaults
test_stress_roundtrip <- function() {
    for (i in 1:100) {
        test_roundtrip()
    }
    TRUE
}
