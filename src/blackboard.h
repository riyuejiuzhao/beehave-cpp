#ifndef BLACKBOARD_H
#define BLACKBOARD_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot {
    class ActionLeaf;
    class ConditionLeaf;

    class Blackboard : public Node {
        GDCLASS(Blackboard, Node)

    public:
        inline static constexpr const char* DEFAULT = "default";
        inline static constexpr const char* KEY_CAN_SEND_MESSAGE = "_beehave_can_send_message";
        inline static constexpr const char* KEY_RUNNING_ACTION = "_beehave_running_action";
        inline static constexpr const char* KEY_LAST_CONDITION = "_beehave_last_condition";
        inline static constexpr const char* KEY_LAST_CONDITION_STATUS = "_beehave_last_condition_status";

        Blackboard();
        ~Blackboard();

        TypedArray<String> keys();
        
        void set_blackboard(const Dictionary &p_blackboard);
        Dictionary get_blackboard() const;

        void set_value(const String &p_key, const Variant &p_value, const String &p_blackboard_name = DEFAULT);
        Variant get_value(const String &p_key, const Variant &p_default_value = Variant(), const String &p_blackboard_name = DEFAULT);
        bool has_value(const String &p_key, const String &p_blackboard_name = DEFAULT);
        void erase_value(const String &p_key, const String &p_blackboard_name = DEFAULT);

        void set_can_send_message(bool p_enabled, const String &p_blackboard_name = DEFAULT);
        bool get_can_send_message(const String &p_blackboard_name = DEFAULT);
        void set_running_action(ActionLeaf *p_action, const String &p_blackboard_name = DEFAULT);
        ActionLeaf *get_running_action(const String &p_blackboard_name = DEFAULT);
        void set_last_condition_result(ConditionLeaf *p_condition, int p_status, const String &p_blackboard_name = DEFAULT);
        ConditionLeaf *get_last_condition(const String &p_blackboard_name = DEFAULT);
        int get_last_condition_status(int p_default_status = -1, const String &p_blackboard_name = DEFAULT);

        Dictionary get_debug_data() const;

    private:
        Dictionary data;

        static bool _is_reserved_key(const String &p_key);
        static void _warn_reserved_key_collision(const String &p_key);
        void _set_value_unchecked(const String &p_key, const Variant &p_value, const String &p_blackboard_name = DEFAULT);
        Variant _get_value_unchecked(const String &p_key, const Variant &p_default_value = Variant(), const String &p_blackboard_name = DEFAULT) const;

    protected:
        static void _bind_methods();
    };

}

#endif
