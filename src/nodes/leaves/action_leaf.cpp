#include "action_leaf.hpp"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

ActionLeaf::ActionLeaf()
{
}

ActionLeaf::~ActionLeaf()
{
}

TypedArray<StringName> ActionLeaf::get_class_name() const
{
    TypedArray<StringName> classes = Leaf::get_class_name();
    classes.push_back("ActionLeaf");
    return classes;
}

void ActionLeaf::_bind_methods()
{
}
