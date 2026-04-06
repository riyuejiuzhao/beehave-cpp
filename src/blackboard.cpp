#include "blackboard.h"
#include "debug/debugger_messages.h"
#include "nodes/leaves/action_leaf.hpp"
#include "nodes/leaves/condition_leaf.hpp"
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
    Array keys = p_blackboard.keys();
    for (int i = 0; i < keys.size(); i++)
    {
        Variant key = keys[i];
        if ((key.get_type() == Variant::STRING || key.get_type() == Variant::STRING_NAME) && _is_reserved_key(String(key)))
            _warn_reserved_key_collision(String(key));
    }
    data[DEFAULT] = p_blackboard.duplicate();
}

Dictionary Blackboard::get_blackboard() const
{
    return data[DEFAULT];
}

void Blackboard::set_value(const String &p_key, const Variant &p_value, const String &p_blackboard_name)
{
    if (_is_reserved_key(p_key))
    {
        _warn_reserved_key_collision(p_key);
    }
    _set_value_unchecked(p_key, p_value, p_blackboard_name);
}

Variant Blackboard::get_value(const String &p_key, const Variant &p_default_value, const String &p_blackboard_name)
{
    if (has_value(p_key, p_blackboard_name))
    {
        return _get_value_unchecked(p_key, p_default_value, p_blackboard_name);
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

void Blackboard::set_can_send_message(bool p_enabled, const String &p_blackboard_name)
{
    _set_value_unchecked(KEY_CAN_SEND_MESSAGE, p_enabled, p_blackboard_name);
}

bool Blackboard::get_can_send_message(const String &p_blackboard_name)
{
    return bool(_get_value_unchecked(KEY_CAN_SEND_MESSAGE, false, p_blackboard_name));
}

void Blackboard::set_running_action(ActionLeaf *p_action, const String &p_blackboard_name)
{
    _set_value_unchecked(KEY_RUNNING_ACTION, p_action, p_blackboard_name);
}

ActionLeaf *Blackboard::get_running_action(const String &p_blackboard_name)
{
    Variant value = _get_value_unchecked(KEY_RUNNING_ACTION, Variant(), p_blackboard_name);
    return Object::cast_to<ActionLeaf>(value);
}

void Blackboard::set_last_condition_result(ConditionLeaf *p_condition, int p_status, const String &p_blackboard_name)
{
    _set_value_unchecked(KEY_LAST_CONDITION, p_condition, p_blackboard_name);
    _set_value_unchecked(KEY_LAST_CONDITION_STATUS, p_status, p_blackboard_name);
}

ConditionLeaf *Blackboard::get_last_condition(const String &p_blackboard_name)
{
    Variant value = _get_value_unchecked(KEY_LAST_CONDITION, Variant(), p_blackboard_name);
    return Object::cast_to<ConditionLeaf>(value);
}

int Blackboard::get_last_condition_status(int p_default_status, const String &p_blackboard_name)
{
    return int(_get_value_unchecked(KEY_LAST_CONDITION_STATUS, p_default_status, p_blackboard_name));
}

Dictionary Blackboard::get_debug_data() const
{
    return data;
}

bool Blackboard::_is_reserved_key(const String &p_key)
{
    return p_key == KEY_CAN_SEND_MESSAGE ||
        p_key == KEY_RUNNING_ACTION ||
        p_key == KEY_LAST_CONDITION ||
        p_key == KEY_LAST_CONDITION_STATUS;
}

void Blackboard::_warn_reserved_key_collision(const String &p_key)
{
    GD_LOG_WARNING(
        "Blackboard key `{0}` is reserved for Beehave internal state. Writing to it will still succeed, but the value may be overwritten by the framework.",
        p_key
    );
}

void Blackboard::_set_value_unchecked(const String &p_key, const Variant &p_value, const String &p_blackboard_name)
{
    if (!data.has(p_blackboard_name))
        data[p_blackboard_name] = Dictionary();

    Dictionary board = data[p_blackboard_name];
    board[p_key] = p_value;
    data[p_blackboard_name] = board;
}

Variant Blackboard::_get_value_unchecked(const String &p_key, const Variant &p_default_value, const String &p_blackboard_name) const
{
    if (!data.has(p_blackboard_name))
        return p_default_value;

    Dictionary board = data[p_blackboard_name];
    return board.get(p_key, p_default_value);
}

void Blackboard::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_blackboard", "blackboard"), &Blackboard::set_blackboard);
    ClassDB::bind_method(D_METHOD("get_blackboard"), &Blackboard::get_blackboard);

    ClassDB::bind_method(D_METHOD("set_value", "key", "value", "blackboard_name"), &Blackboard::set_value, DEFVAL(DEFAULT));
    ClassDB::bind_method(D_METHOD("get_value", "key", "default_value", "blackboard_name"), &Blackboard::get_value, DEFVAL(Variant()), DEFVAL(DEFAULT));
    ClassDB::bind_method(D_METHOD("has_value", "key", "blackboard_name"), &Blackboard::has_value, DEFVAL(DEFAULT));
    ClassDB::bind_method(D_METHOD("erase_value", "key", "blackboard_name"), &Blackboard::erase_value, DEFVAL(DEFAULT));
    ClassDB::bind_method(D_METHOD("set_can_send_message", "enabled", "blackboard_name"), &Blackboard::set_can_send_message, DEFVAL(DEFAULT));
    ClassDB::bind_method(D_METHOD("get_can_send_message", "blackboard_name"), &Blackboard::get_can_send_message, DEFVAL(DEFAULT));
    ClassDB::bind_method(D_METHOD("set_running_action", "action", "blackboard_name"), &Blackboard::set_running_action, DEFVAL(DEFAULT));
    ClassDB::bind_method(D_METHOD("get_running_action", "blackboard_name"), &Blackboard::get_running_action, DEFVAL(DEFAULT));
    ClassDB::bind_method(D_METHOD("set_last_condition_result", "condition", "status", "blackboard_name"), &Blackboard::set_last_condition_result, DEFVAL(DEFAULT));
    ClassDB::bind_method(D_METHOD("get_last_condition", "blackboard_name"), &Blackboard::get_last_condition, DEFVAL(DEFAULT));
    ClassDB::bind_method(D_METHOD("get_last_condition_status", "default_status", "blackboard_name"), &Blackboard::get_last_condition_status, DEFVAL(-1), DEFVAL(DEFAULT));
    ClassDB::bind_method(D_METHOD("keys"), &Blackboard::keys);
    ClassDB::bind_method(D_METHOD("get_debug_data"), &Blackboard::get_debug_data);

    ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "blackboard", PROPERTY_HINT_NONE), "set_blackboard", "get_blackboard");
}
