#include "condition_leaf.hpp"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

ConditionLeaf::ConditionLeaf()
{
}

ConditionLeaf::~ConditionLeaf()
{
}

TypedArray<StringName> ConditionLeaf::get_class_name() const
{
    TypedArray<StringName> classes = Leaf::get_class_name();
    classes.push_back("ConditionLeaf");
    return classes;
}

void ConditionLeaf::_bind_methods()
{
}

void ConditionLeaf::after_tick(Node *actor, Blackboard *blackboard, int response)
{
    blackboard->set_last_condition_result(this, response, String::num_int64(actor->get_instance_id()));
}
