#include "register_types.h"

#include <gdextension_interface.h>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

#include "script_switcher.hpp"

using namespace godot;

void initialize_script_switcher_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_EDITOR) {
		return;
	}

        // TODO add classes here
	GDREGISTER_CLASS(ScriptSwitcher);
}

void uninitialize_script_switcher_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_EDITOR) {
		return;
	}
}

extern "C" {

        GDExtensionBool GDE_EXPORT script_switcher_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
                godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

                init_obj.register_initializer(initialize_script_switcher_module);
                init_obj.register_terminator(uninitialize_script_switcher_module);
                init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_EDITOR);

                return init_obj.init();
        }
}
