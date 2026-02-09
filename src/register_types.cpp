#include "register_types.h"
#include "nodes/beehave_node.h"
#include "nodes/composites/composite.hpp"
#include "nodes/composites/sequence_composite.hpp"
#include "nodes/composites/selector_composite.hpp"
#include "nodes/leaves/leaf.hpp"
#include "nodes/leaves/action_leaf.hpp"
#include "nodes/leaves/condition_leaf.hpp"
#include "nodes/beehave_tree.h"
#include "blackboard.h"
#include "metrics/beehave_metrics.h"
#include <gdextension_interface.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_beehave_module(ModuleInitializationLevel p_level)
{
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	ClassDB::register_class<BeehaveTree>();
	ClassDB::register_class<Blackboard>();
	ClassDB::register_class<BeehaveMetrics>();
	ClassDB::register_class<BeehaveNode>();
	ClassDB::register_class<Composite>();
	ClassDB::register_class<SequenceComposite>();
	ClassDB::register_class<SelectorComposite>();
	ClassDB::register_class<Leaf>();
	ClassDB::register_class<ActionLeaf>();
	ClassDB::register_class<ConditionLeaf>();
}

void uninitialize_beehave_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
}

extern "C"
{
	// Initialization
	GDExtensionBool GDE_EXPORT beehave_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization)
	{
		GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);
		init_obj.register_initializer(initialize_beehave_module);
		init_obj.register_terminator(uninitialize_beehave_module);
		init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

		return init_obj.init();
	}
}