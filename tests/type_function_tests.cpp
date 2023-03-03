
#include "shl/type_functions.hpp"

static_assert(is_same(char, char));
static_assert(!is_same(char, wchar_t));

int main(){}
