#include "beehave_node.h"
#include "../debug/debugger_messages.h"
#include "../util.hpp"
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/engine_debugger.hpp>
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/script.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

BeehaveNode::BeehaveNode() {
}

BeehaveNode::~BeehaveNode() {
}

void BeehaveNode::_bind_methods() {
    ClassDB::bind_method(D_METHOD("interrupt", "actor", "blackboard"), &BeehaveNode::interrupt);
    ClassDB::bind_method(D_METHOD("before_run", "actor", "blackboard"), &BeehaveNode::before_run);
    ClassDB::bind_method(D_METHOD("after_run", "actor", "blackboard"), &BeehaveNode::after_run);
    ClassDB::bind_method(D_METHOD("get_class_name"), &BeehaveNode::get_class_name);
    ClassDB::bind_method(D_METHOD("can_send_message", "blackboard"), &BeehaveNode::can_send_message);

    GDVIRTUAL_BIND(_tick, "actor", "blackboard");

    BIND_ENUM_CONSTANT(SUCCESS);
    BIND_ENUM_CONSTANT(FAILURE);
    BIND_ENUM_CONSTANT(RUNNING);
}

PackedStringArray BeehaveNode::_get_configuration_warnings() const {
    PackedStringArray warnings;

    TypedArray<Node> children = get_children();
    if (any_of(children, [](Node *node) { 
        return Object::cast_to<BeehaveNode>(node) == nullptr; 
    })) {
        warnings.push_back("All children of this node should inherit from BeehaveNode class.");
    }

    return warnings;
}

int BeehaveNode::tick(Node* actor, Blackboard* blackboard) {
    return SUCCESS;
}

int BeehaveNode::safe_tick(Node* actor, Blackboard* blackboard) {
    int response = tick(actor, blackboard);
    GDVIRTUAL_CALL(_tick, actor, blackboard, response);
    after_tick(actor, blackboard, response);
    return response;
}

void BeehaveNode::after_tick(Node *actor, Blackboard *blackboard, int response) {
}

void BeehaveNode::interrupt(Node* actor, Blackboard* blackboard) {
}

void BeehaveNode::before_run(Node* actor, Blackboard* blackboard) {
    // Default implementation: do nothing
}

void BeehaveNode::after_run(Node* actor, Blackboard* blackboard) {
    // Default implementation: do nothing
}

TypedArray<StringName> BeehaveNode::get_class_name() const {
    return Array::make(StringName("BeehaveNode"));
}

bool BeehaveNode::can_send_message(Blackboard* blackboard) {
    return blackboard->get_can_send_message();
}

