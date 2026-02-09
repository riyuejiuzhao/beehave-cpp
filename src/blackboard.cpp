#include "blackboard.h"
#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

Blackboard::Blackboard()
{
    data = Dictionary();
    data[DEFAULT] = Dictionary();
}

Blackboard::~Blackboard()
{
}

TypedArray<String> Blackboard::keys()
{
    TypedArray<String> result;
    result.assign(data.keys());
    return result;
}

void Blackboard::set_blackboard(const Dictionary &p_blackboard)
{
    data[DEFAULT] = p_blackboard.duplicate();
}

Dictionary Blackboard::get_blackboard() const
{
    return data[DEFAULT];
}

void Blackboard::set_value(const String &p_key, const Variant &p_value, const String &p_blackboard_name)
{
    if (!data.has(p_blackboard_name))
        data[p_blackboard_name] = Dictionary();

    Dictionary board = data[p_blackboard_name];
    board[p_key] = p_value;
    data[p_blackboard_name] = board;
}

Variant Blackboard::get_value(const String &p_key, const Variant &p_default_value, const String &p_blackboard_name)
{
    if (has_value(p_key, p_blackboard_name))
    {
        Dictionary board = data[p_blackboard_name];
        return board.get(p_key, p_default_value);
    }
    return p_default_value;
}

bool Blackboard::has_value(const String &p_key, const String &p_blackboard_name)
{
    if (!data.has(p_blackboard_name))
        return false;

    Dictionary board = data[p_blackboard_name];
    if (!board.has(p_key))
        return false;

    Variant value = board[p_key];
    return value.get_type() != Variant::NIL;
}

void Blackboard::erase_value(const String &p_key, const String &p_blackboard_name)
{
    if (!data.has(p_blackboard_name))
        return;
    Dictionary board = data[p_blackboard_name];
    board[p_key] = Variant();
    data[p_blackboard_name] = board;
}

Dictionary Blackboard::get_debug_data() const
{
    return data;
}

void Blackboard::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_blackboard", "blackboard"), &Blackboard::set_blackboard);
    ClassDB::bind_method(D_METHOD("get_blackboard"), &Blackboard::get_blackboard);

    ClassDB::bind_method(D_METHOD("set_value", "key", "value", "blackboard_name"), &Blackboard::set_value, DEFVAL(DEFAULT));
    ClassDB::bind_method(D_METHOD("get_value", "key", "default_value", "blackboard_name"), &Blackboard::get_value, DEFVAL(Variant()), DEFVAL(DEFAULT));
    ClassDB::bind_method(D_METHOD("has_value", "key", "blackboard_name"), &Blackboard::has_value, DEFVAL(DEFAULT));
    ClassDB::bind_method(D_METHOD("erase_value", "key", "blackboard_name"), &Blackboard::erase_value, DEFVAL(DEFAULT));
    ClassDB::bind_method(D_METHOD("keys"), &Blackboard::keys);
    ClassDB::bind_method(D_METHOD("get_debug_data"), &Blackboard::get_debug_data);

    ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "blackboard", PROPERTY_HINT_NONE), "set_blackboard", "get_blackboard");
}
