#include "leaf.hpp"
#include "util.hpp"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

Leaf::Leaf()
{
}

Leaf::~Leaf()
{
}

PackedStringArray Leaf::_get_configuration_warnings() const
{
    PackedStringArray warnings = BeehaveNode::_get_configuration_warnings();
    TypedArray<Node> children = get_children();

    if (any_of(children, [](Node *node)
               { return Object::cast_to<BeehaveNode>(node) != nullptr; }))
        warnings.append("Leaf nodes should not have any child nodes. They won't be ticked.");

    TypedArray<String> sources = _get_expression_sources();
    for (int i = 0; i < sources.size(); i++)
    {
        String source = sources[i];
        Ref<Expression> expr = _parse_expression(source);
        String error_text = expr->get_error_text();
        if (!error_text.is_empty())
        {
            warnings.append(vformat("Expression `%s` is invalid! Error text: `%s`", source, error_text));
        }
    }

    return warnings;
}

Ref<Expression> Leaf::_parse_expression(const String &source) const
{
    Ref<Expression> result;
    result.instantiate();
    Error error = result->parse(source);

    if (!Engine::get_singleton()->is_editor_hint() && error != OK)
    {
        UtilityFunctions::push_error(
            vformat("[Leaf] Couldn't parse expression with source: `%s` Error text: `%s`",
                    source, result->get_error_text()));
    }

    return result;
}

TypedArray<String> Leaf::_get_expression_sources() const
{
    return {};
}

TypedArray<StringName> Leaf::get_class_name() const
{
    TypedArray<StringName> classes = BeehaveNode::get_class_name();
    classes.push_back("Leaf");
    return classes;
}

void Leaf::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("_parse_expression", "source"), &Leaf::_parse_expression);
    ClassDB::bind_method(D_METHOD("_get_expression_sources"), &Leaf::_get_expression_sources);
}
