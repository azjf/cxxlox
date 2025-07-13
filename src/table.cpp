// clang-format off
#include "object.h"
#include "table.h"
// clang-format on

bool Table::get(ObjString *key, Value *value)
{
    if (map.empty()) { return false; }
    auto entry = map.find(key);
    if (entry == map.cend()) { return false; }
    *value = entry->second;
    return true;
}

bool Table::set(ObjString *key, Value value)
{
    bool isNewKey = map.find(key) == map.cend();
    map[key] = value;
    return isNewKey;
}

bool Table::delete_(ObjString *key)
{
    if (map.empty()) { return false; }
    auto entry = map.find(key);
    if (entry == map.cend()) { return false; }
    map.erase(entry);
    return true;
}


ObjString *Table::findString(const char *chars, int length, uint32_t hash)
{
    if (map.empty()) { return nullptr; }
    for (const auto &p : map) {
        if (p.first->equals(chars, length, hash)) { return p.first; }
    }
    return nullptr;
}

void Table::addAll(Table *from, Table *to) { to->map.insert(from->map.cbegin(), from->map.cend()); }
