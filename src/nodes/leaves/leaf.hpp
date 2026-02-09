#ifndef LEAF_HPP
#define LEAF_HPP

#include "../beehave_node.h"
#include <godot_cpp/classes/expression.hpp>
#include <godot_cpp/variant/array.hpp>

namespace godot
{
    /// Base class for all leaf nodes of the tree.
    /// Leaf nodes should not have any children.
    class Leaf : public BeehaveNode
    {
        GDCLASS(Leaf, BeehaveNode)

    public:
        inline static constexpr char* EXPRESSION_PLACEHOLDER = "Insert an expression...";

        Leaf();
        ~Leaf();

        virtual PackedStringArray _get_configuration_warnings() const override;
        virtual TypedArray<StringName> get_class_name() const override;

    protected:
        static void _bind_methods();

        Ref<Expression> _parse_expression(const String &source) const;
        virtual TypedArray<String> _get_expression_sources() const;
    };

}

#endif // LEAF_HPP
