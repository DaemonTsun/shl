
#pragma once

#define JOIN(X, Y) X##Y
#define JOIN3(X, Y, Z) X##Y##Z

#define GET_MACRO2(_1, _2, _3, NAME, ...) NAME
#define GET_MACRO3(_1, _2, _3, _4, NAME, ...) NAME
