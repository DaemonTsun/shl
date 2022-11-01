
#include <string_view>

#include "shl/hash.hpp"

hash_t hash_data(const char *data, u64 size)
{
    return std::hash<std::string_view>()(std::string_view(data, size));
}
