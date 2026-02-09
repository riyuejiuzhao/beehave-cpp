#ifndef BEEHAVE_DEBUGGER_MESSAGES_H
#define BEEHAVE_DEBUGGER_MESSAGES_H

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/engine_debugger.hpp>
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/variant/dictionary.hpp>

// namespace godot {

//     class BeehaveDebuggerMessages {
//     public:
//         static bool can_send_message();
//         static void register_tree(const Dictionary& beehave_tree);
//         static void unregister_tree(int instance_id);
//         static void process_tick(int instance_id, int status, const Dictionary& blackboard = Dictionary());
//         static void process_interrupt(int instance_id, const Dictionary& blackboard = Dictionary());
//         static void process_begin(int instance_id, const Dictionary& blackboard = Dictionary());
//         static void process_end(int instance_id, const Dictionary& blackboard = Dictionary());
//     };

// }


#define GD_LOG_INFO(fmt, ...) \
    do { \
        godot::UtilityFunctions::print( \
            godot::String("[INFO] {0}:{1} ").format(godot::Array::make(__FILE__, __LINE__)) + \
                godot::String(fmt).format(godot::Array::make(__VA_ARGS__)) \
        ); \
    } while (0)

#define GD_LOG_WARNING(fmt, ...) \
    do { \
        godot::UtilityFunctions::push_warning( \
            godot::String("[WARNING] {0}:{1} ").format(godot::Array::make(__FILE__, __LINE__)) + \
                godot::String(fmt).format(godot::Array::make(__VA_ARGS__)) \
        ); \
    } while (0)

#define GD_LOG_ERROR(fmt, ...) \
    do { \
        godot::UtilityFunctions::push_error( \
            godot::String("[ERROR] {0}:{1} ").format(godot::Array::make(__FILE__, __LINE__)) + \
                godot::String(fmt).format(godot::Array::make(__VA_ARGS__)) \
        ); \
    } while (0)

#endif // BEEHAVE_DEBUGGER_MESSAGES_H
