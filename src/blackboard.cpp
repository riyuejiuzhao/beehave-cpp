#include "blackboard.h"
#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

Blackboard::Blackboard()
{
    data = Dictionary();
}

TypedArray<String> Blackboard::keys()
{
    return data.keys().duplicate();
}

void Blackboard::set_data(const Dictionary& p_data)
{
    data = p_data;
}

void Blackboard::set_value(const String &p_key, const Variant &p_value)
{
    data[p_key] = p_value;
}

Variant Blackboard::get_value(const String &p_key, const Variant &default_value)
{
    return data.get(p_key, default_value);
}

bool Blackboard::has_value(const String &p_key)
{
    return data.has(p_key);
}

void Blackboard::erase_entry(const String &p_key)
{
    if (data.has(p_key))
        data.erase(p_key);
    else
        WARN_PRINT("entry already erased");
}

Blackboard::~Blackboard()
{
}

void Blackboard::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_data", "data"), &Blackboard::set_data);
    ClassDB::bind_method(D_METHOD("get_data"), &Blackboard::get_data);

    ClassDB::bind_method(D_METHOD("set_value", "key", "value"), &Blackboard::set_value);
    ClassDB::bind_method(D_METHOD("get_value", "key", "default_value"), &Blackboard::get_value, Variant());
    ClassDB::bind_method(D_METHOD("has_value", "key"), &Blackboard::has_value);
    ClassDB::bind_method(D_METHOD("erase_entry", "key"), &Blackboard::erase_entry);
    ClassDB::bind_method(D_METHOD("keys"), &Blackboard::keys);

    ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "data", PROPERTY_HINT_NONE), "set_data", "get_data");
}
