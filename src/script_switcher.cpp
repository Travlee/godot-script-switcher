#include <godot_cpp/classes/editor_interface.hpp>
#include <godot_cpp/classes/input_event_key.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/classes/packed_scene.hpp>

#include "script_switcher.hpp"

using namespace godot;

ScriptSwitcher::ScriptSwitcher() {}
ScriptSwitcher::~ScriptSwitcher() {}

void ScriptSwitcher::_bind_methods()
{
        ClassDB::bind_method(D_METHOD("_enter_tree"), &ScriptSwitcher::_enter_tree);
        ClassDB::bind_method(D_METHOD("_exit_tree"), &ScriptSwitcher::_exit_tree);
        ClassDB::bind_method(D_METHOD("_on_script_changed", "script"), &ScriptSwitcher::_on_script_changed);
}

void ScriptSwitcher::_load_popup()
{
        ResourceLoader *loader = ResourceLoader::get_singleton();

        const String scene_file_path = ADDON_DIR "script_switcher_panel.tscn";

        Ref<PackedScene> scene_resource = loader->load(scene_file_path);

        if (!scene_resource.is_valid())
        {
                UtilityFunctions::printerr("Failed to load popup scene file!");
                return;
        }

        popup = cast_to<PanelContainer>(scene_resource->instantiate());
        if (!popup)
        {
                UtilityFunctions::printerr("Failed to instantiate popup!");
                return;
        }

        item_list = popup->get_node<ItemList>("%ItemList");
        if (!item_list)
        {
                UtilityFunctions::printerr("Failed to load ItemList!");
                return;
        }

        // ? Not sure which parent is best ...
        // EditorInterface::get_singleton()->get_base_control()->add_child(popup);
        add_child(popup);

        popup->hide();
}

void ScriptSwitcher::_enter_tree()
{
        // set_process_mode(PROCESS_MODE_ALWAYS);

        this->_load_popup();

        script_editor = EditorInterface::get_singleton()->get_script_editor();
        if (script_editor && !script_editor->is_connected("editor_script_changed", Callable(this, "_on_script_changed")))
        {
                script_editor->connect("editor_script_changed", Callable(this, "_on_script_changed"));
        }
}

void ScriptSwitcher::_exit_tree()
{
        if (popup)
        {
                popup->queue_free();
        }

        if (item_list)
        {
                item_list->queue_free();
        }

        if (script_editor && script_editor->is_connected("editor_script_changed", Callable(this, "_on_script_changed")))
        {
                script_editor->disconnect("editor_script_changed", Callable(this, "_on_script_changed"));
        }

        if (script_editor)
        {
                script_editor->queue_free();
        }
}

void ScriptSwitcher::_on_script_changed(const Ref<Script> &script)
{
        if (script.is_null())
        {
                UtilityFunctions::print("Null script!");
                return;
        }

        String path = script->get_path();
        if (path.is_empty())
        {
                UtilityFunctions::print("Empty script path!");
                return;
        }

        // ! debugging
        // UtilityFunctions::print("script changed: " + path);

        // Move to front of MRU history
        for (int i = 0; i < history.size(); ++i)
        {
                if (history[i] == path)
                {
                        // history.remove_at(i);
                        history.erase(history.begin() + i);
                        break;
                }
        }
        // history.begin(path, 0);
        history.insert(history.begin(), path);
}

void ScriptSwitcher::_popup_input(const Ref<InputEvent> &event)
{
        UtilityFunctions::print("popup event!");
}

void ScriptSwitcher::_input(const Ref<InputEvent> &event)
{
        // TODO rework this to a configurable keybind for the plugin

        // ? Best method I could find to limit functionalty to when script editor is active
        if (!script_editor->is_visible_in_tree())
        {
                return;
        }

        Ref<InputEventKey> key_event = event;

        if (!key_event.is_valid() || key_event->is_echo())
        {
                return;
        }

        auto key = key_event->get_keycode();

        if (!popup->is_visible() && key_event->is_pressed() && key == KEY_TAB && key_event->is_command_or_control_pressed())
        {
                this->_update_list();
                popup->show();

                get_viewport()->set_input_as_handled();

                return;
        }

        if (popup->is_visible() && !key_event->is_pressed() && key == KEY_CTRL)
        {
                popup->hide();
                get_viewport()->set_input_as_handled();
                UtilityFunctions::print("popup->hide()");

                int selected_index = item_list->get_selected_items()[0];
                if (selected_index >= history.size())
                {
                        UtilityFunctions::printerr("Invalid history item!");
                        return;
                }
                String path = history[selected_index];

                Ref<Resource> script_res = ResourceLoader::get_singleton()->load(path);
                EditorInterface::get_singleton()->edit_resource(script_res);
        }

        if (popup->is_visible() && key_event->is_pressed() && key == KEY_TAB)
        {
                int selected_index = item_list->get_selected_items()[0];
                int next_index = (selected_index + 1) % item_list->get_item_count();
                item_list->select(next_index);
                get_viewport()->set_input_as_handled();
        }
}

void ScriptSwitcher::_update_list()
{
        item_list->clear();
        for (const String &path : history)
        {
                item_list->add_item(path.get_file());
                item_list->set_item_tooltip(item_list->get_item_count() - 1, path);
        }

        // ? Selects previous script for faster switching
        if (item_list->get_item_count() > 1)
        {
                item_list->select(1);
        }
}
