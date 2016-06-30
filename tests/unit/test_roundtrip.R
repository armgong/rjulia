
### Test sending data from R to julia and back again

library(RUnit)
library(rjulia)
rjulia:::.julia_init_if_necessary()

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

    mapply(
        names(test.list),  test.list,
        FUN=function(k, v) {
            lapply(
                types,
                function(x) {
                    message(x)
                    v = as(v, x)
                    r2j(v, k)
                    checkIdentical( v, j2r(k) )
                })
        })

    lists = list(
        h = list( 1:5, letters ),
        j = list( c(NA, 2:5), letters )
    )
    mapply(names(lists), lists,
           FUN=function(k, v) {
               r2j(v, k)
               checkIdentical( v, j2r(k) )
           })
}

## Repeated run r2j and j2r, can detect 'random' segfaults
test_stress_roundtrip <- function() {
    for (i in 1:100) {
        test_roundtrip()
    }
    TRUE
}
