#ifndef BEEHAVE_METRICS_H
#define BEEHAVE_METRICS_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/templates/hash_set.hpp>

namespace godot
{
    class BeehaveTree;

    class BeehaveMetrics : public Node
    {
        GDCLASS(BeehaveMetrics, Node)

    public:
        static BeehaveMetrics *get_singleton();

        BeehaveMetrics();
        ~BeehaveMetrics();

        void register_tree(BeehaveTree *p_tree);
        void unregister_tree(BeehaveTree *p_tree);

        int get_total_trees() const { return tree_count; }
        int get_total_enabled_trees() const { return active_tree_count; }

        void _notification(int p_what);

    private:
        void _enter_tree_internal();
        void _exit_tree_internal();
        void _on_tree_enabled();
        void _on_tree_disabled();

        int tree_count;
        int active_tree_count;
        HashSet<BeehaveTree *> registered_trees;

        static BeehaveMetrics *singleton;

    protected:
        static void _bind_methods();
    };

} // namespace godot

#endif // BEEHAVE_METRICS_H
