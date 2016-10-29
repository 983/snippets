#include "vector.hpp"
#include "int.hpp"
#include "util.hpp"

struct DefaultHasher {
    u32 operator () (u32 key) const {
        return key;
    }
};

template <typename KEY, typename VALUE, typename HASHER = DefaultHasher>
struct Map {
    struct KeyValue {
        KEY key;
        VALUE value;

        KeyValue(){}
        KeyValue(KEY key, VALUE value = VALUE()): key(key), value(value){}
    };

    typedef Vector<KeyValue> Bin;

    Vector<Bin> bins;
    u32 n;

    Map(): bins(1){}

    u32 size() const {
        return n;
    }

    VALUE& operator [] (KEY key){
        u32 index = HASHER()(key) % bins.size();
        Bin &bin = bins[index];
        for (KeyValue &kv : bin){
            if (kv.key == key) return kv.value;
        }
        n++;
        bin.push(KeyValue(key));
        return bin.back().value;
    }

    bool has(const KEY &key){
        u32 index = HASHER()(key) % bins.size();
        Bin &bin = bins[index];
        for (KeyValue &kv : bin){
            if (kv.key == key) return true;
        }
        return false;
    }

    bool remove(KEY key){
        u32 index = HASHER()(key) % bins.size();
        Bin &bin = bins[index];
        for (KeyValue &kv : bin){
            if (kv.key == key){
                kv = bin.back();
                bin.pop();
                n--;
                return true;
            }
        }
        return false;
    }
};
