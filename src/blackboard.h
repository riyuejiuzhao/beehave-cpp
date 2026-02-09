#ifndef BLACKBOARD_H
#define BLACKBOARD_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot {

    class Blackboard : public Node {
        GDCLASS(Blackboard, Node)

    public:
        inline static constexpr const char* DEFAULT = "default";

        Blackboard();
        ~Blackboard();

        TypedArray<String> keys();
        
        void set_blackboard(const Dictionary &p_blackboard);
        Dictionary get_blackboard() const;

        void set_value(const String &p_key, const Variant &p_value, const String &p_blackboard_name = DEFAULT);
        Variant get_value(const String &p_key, const Variant &p_default_value = Variant(), const String &p_blackboard_name = DEFAULT);
        bool has_value(const String &p_key, const String &p_blackboard_name = DEFAULT);
        void erase_value(const String &p_key, const String &p_blackboard_name = DEFAULT);
        
        Dictionary get_debug_data() const;

    private:
        Dictionary data;
    
    protected:
        static void _bind_methods();
    };

}

#endif
