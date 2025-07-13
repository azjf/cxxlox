#include <unordered_map>

#include "object.h"
#include "value.h"

class Table
{
public:
    bool get(ObjString *key, Value *value);
    bool set(ObjString *key, Value value);
    bool delete_(ObjString *key);
    ObjString *findString(const char *chars, int length, uint32_t hash);
    void addAll(Table *from, Table *to);

private:
    // Value of Obj type will be freed by VM dtor
    std::unordered_map<ObjString *, Value> map;
};
