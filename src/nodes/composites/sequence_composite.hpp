#ifndef SEQUENCE_COMPOSITE_HPP
#define SEQUENCE_COMPOSITE_HPP

#include "composite.hpp"

namespace godot
{
    /// A Sequence runs its children in order until one fails or is running.
    /// It succeeds only if all children succeed.
    class SequenceComposite : public Composite
    {
        GDCLASS(SequenceComposite, Composite)

    public:
        SequenceComposite();
        ~SequenceComposite();

        virtual void interrupt(Node *actor, Blackboard *blackboard) override;
        virtual TypedArray<StringName> get_class_name() const override;

    protected:
        static void _bind_methods();
        virtual int tick(Node *actor, Blackboard *blackboard) override;

    private:
        void _reset();

        int successful_index;
        int previous_failure_or_running_index;
    };

} // namespace godot

#endif // SEQUENCE_COMPOSITE_HPP
