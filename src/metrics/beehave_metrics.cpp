#include "beehave_metrics.h"
#include "nodes/beehave_tree.h"

#include <godot_cpp/classes/performance.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

BeehaveMetrics *BeehaveMetrics::singleton = nullptr;

BeehaveMetrics *BeehaveMetrics::get_singleton()
{
    return singleton;
}

BeehaveMetrics::BeehaveMetrics()
{
    tree_count = 0;
    active_tree_count = 0;
    singleton = this;
}

BeehaveMetrics::~BeehaveMetrics()
{
    if (singleton == this)
        singleton = nullptr;
}

void BeehaveMetrics::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("register_tree", "tree"), &BeehaveMetrics::register_tree);
    ClassDB::bind_method(D_METHOD("unregister_tree", "tree"), &BeehaveMetrics::unregister_tree);
    ClassDB::bind_method(D_METHOD("get_total_trees"), &BeehaveMetrics::get_total_trees);
    ClassDB::bind_method(D_METHOD("get_total_enabled_trees"), &BeehaveMetrics::get_total_enabled_trees);
    ClassDB::bind_method(D_METHOD("_on_tree_enabled"), &BeehaveMetrics::_on_tree_enabled);
    ClassDB::bind_method(D_METHOD("_on_tree_disabled"), &BeehaveMetrics::_on_tree_disabled);
}

void BeehaveMetrics::_notification(int p_what)
{
    switch (p_what)
    {
    case NOTIFICATION_ENTER_TREE:
        _enter_tree_internal();
        break;
    case NOTIFICATION_EXIT_TREE:
        _exit_tree_internal();
        break;
    }
}

void BeehaveMetrics::_enter_tree_internal()
{
    Performance *performance = Performance::get_singleton();
    if (performance)
    {
        performance->add_custom_monitor("beehave/total_trees", callable_mp(this, &BeehaveMetrics::get_total_trees));
        performance->add_custom_monitor("beehave/total_enabled_trees", callable_mp(this, &BeehaveMetrics::get_total_enabled_trees));
    }
}

void BeehaveMetrics::_exit_tree_internal()
{
    Performance *performance = Performance::get_singleton();
    if (performance)
    {
        performance->remove_custom_monitor("beehave/total_trees");
        performance->remove_custom_monitor("beehave/total_enabled_trees");
    }
}

void BeehaveMetrics::register_tree(BeehaveTree *p_tree)
{
    if (!registered_trees.insert(p_tree))
        return;
    tree_count++;
    if (p_tree->get_enabled())
        active_tree_count++;
    p_tree->connect("tree_enabled", Callable(this, "_on_tree_enabled"));
    p_tree->connect("tree_disabled", Callable(this, "_on_tree_disabled"));
}

void BeehaveMetrics::unregister_tree(BeehaveTree *p_tree)
{
    if (!registered_trees.has(p_tree))
        return;
    registered_trees.erase(p_tree);
    tree_count--;
    if (p_tree->get_enabled())
        active_tree_count--;
    p_tree->disconnect("tree_enabled", Callable(this, "_on_tree_enabled"));
    p_tree->disconnect("tree_disabled", Callable(this, "_on_tree_disabled"));
}

void BeehaveMetrics::_on_tree_enabled()
{
    active_tree_count++;
}

void BeehaveMetrics::_on_tree_disabled()
{
    active_tree_count--;
}
