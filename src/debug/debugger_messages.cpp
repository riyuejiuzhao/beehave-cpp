// #include "debugger_messages.h"

// #include <godot_cpp/variant/utility_functions.hpp>


// using namespace godot;

// bool BeehaveDebuggerMessages::can_send_message() {
//     Engine* engine = Engine::get_singleton();
//     OS* os = OS::get_singleton();
    
//     return !engine->is_editor_hint() && os->has_feature("editor");
// }

// void BeehaveDebuggerMessages::register_tree(const Dictionary& beehave_tree) {
//     if (can_send_message()) {
//         EngineDebugger* debugger = EngineDebugger::get_singleton();
//         if (debugger) {
//             Array message_data;
//             message_data.push_back(beehave_tree);
//             debugger->send_message("beehave:register_tree", message_data);
//         }
//     }
// }

// void BeehaveDebuggerMessages::unregister_tree(int instance_id) {
//     if (can_send_message()) {
//         EngineDebugger* debugger = EngineDebugger::get_singleton();
//         if (debugger) {
//             Array message_data;
//             message_data.push_back(instance_id);
//             debugger->send_message("beehave:unregister_tree", message_data);
//         }
//     }
// }

// void BeehaveDebuggerMessages::process_tick(int instance_id, int status, const Dictionary& blackboard) {
//     if (can_send_message()) {
//         EngineDebugger* debugger = EngineDebugger::get_singleton();
//         if (debugger) {
//             Array message_data;
//             message_data.push_back(instance_id);
//             message_data.push_back(status);
//             message_data.push_back(blackboard);
//             debugger->send_message("beehave:process_tick", message_data);
//         }
//     }
// }

// void BeehaveDebuggerMessages::process_interrupt(int instance_id, const Dictionary& blackboard) {
//     if (can_send_message()) {
//         EngineDebugger* debugger = EngineDebugger::get_singleton();
//         if (debugger) {
//             Array message_data;
//             message_data.push_back(instance_id);
//             message_data.push_back(blackboard);
//             debugger->send_message("beehave:process_interrupt", message_data);
//         }
//     }
// }

// void BeehaveDebuggerMessages::process_begin(int instance_id, const Dictionary& blackboard) {
//     if (can_send_message()) {
//         EngineDebugger* debugger = EngineDebugger::get_singleton();
//         if (debugger) {
//             Array message_data;
//             message_data.push_back(instance_id);
//             message_data.push_back(blackboard);
//             debugger->send_message("beehave:process_begin", message_data);
//         }
//     }
// }

// void BeehaveDebuggerMessages::process_end(int instance_id, const Dictionary& blackboard) {
//     if (can_send_message()) {
//         EngineDebugger* debugger = EngineDebugger::get_singleton();
//         if (debugger) {
//             Array message_data;
//             message_data.push_back(instance_id);
//             message_data.push_back(blackboard);
//             debugger->send_message("beehave:process_end", message_data);
//         }
//     }
// }
