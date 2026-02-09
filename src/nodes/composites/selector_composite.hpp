#ifndef SELECTOR_COMPOSITE_HPP
#define SELECTOR_COMPOSITE_HPP

#include "composite.hpp"

namespace godot
{
    /// A Selector runs its children in order until one succeeds or is running.
    /// On failure, skips already-processed children across ticks.
    class SelectorComposite : public Composite
    {
        GDCLASS(SelectorComposite, Composite)

    public:
        SelectorComposite();
        ~SelectorComposite();

        virtual void after_run(Node *actor, Blackboard *blackboard) override;
        virtual void interrupt(Node *actor, Blackboard *blackboard) override;
        virtual TypedArray<StringName> get_class_name() const override;

    protected:
        static void _bind_methods();
        virtual int tick(Node *actor, Blackboard *blackboard) override;

    private:
        int last_execution_index;
        int previous_success_or_running_index;
        bool ready_to_interrupt_all;
    };

}

#endif // SELECTOR_COMPOSITE_HPP
