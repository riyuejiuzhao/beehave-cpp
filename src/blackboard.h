#ifndef BLACKBOARD_H
#define BLACKBOARD_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot {

    class Blackboard : public Node {
        GDCLASS(Blackboard, Node)

    public:
        Blackboard();
        ~Blackboard();

        TypedArray<String> keys();
        
        void set_data(const Dictionary &p_data);
        const Dictionary &get_data() const {return data;}

        void set_value(const String &p_key, const Variant &p_value);
        Variant get_value(const String &p_key, const Variant &default_value = Variant());
        bool has_value(const String &p_key);
        void erase_entry(const String &p_key);
    private:
        Dictionary data;
    protected:
        static void _bind_methods();
    };

}

#endif
