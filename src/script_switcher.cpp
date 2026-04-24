#include <godot_cpp/classes/editor_interface.hpp>
#include <godot_cpp/classes/input_event_key.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <algorithm>

#include "script_switcher.hpp"

using namespace godot;

ScriptSwitcher::ScriptSwitcher() {}
ScriptSwitcher::~ScriptSwitcher() {}

void ScriptSwitcher::_bind_methods()
{
        ClassDB::bind_method(D_METHOD("_on_script_changed", "script"), &ScriptSwitcher::_on_script_changed);
        ClassDB::bind_method(D_METHOD("_on_popup_visibility_changed"), &ScriptSwitcher::_on_popup_visibility_changed);
        ClassDB::bind_method(D_METHOD("_on_script_editor_input", "event"), &ScriptSwitcher::_on_script_editor_input);
        ClassDB::bind_method(D_METHOD("_on_item_list_gui_input", "event"), &ScriptSwitcher::_on_item_list_gui_input);
}

void ScriptSwitcher::_load_popup()
{
        ResourceLoader *loader = ResourceLoader::get_singleton();

        const String scene_file_path = ADDON_DIR "godot_script_switcher_panel.tscn";

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

        popup->set_as_top_level(true);

        if (script_editor) {
                script_editor->add_child(popup);

                if (!script_editor->is_connected("gui_input", Callable(this, "_on_script_editor_input"))) {
                        script_editor->connect("gui_input", Callable(this, "_on_script_editor_input"));
                }
        } else {
                EditorInterface::get_singleton()->get_base_control()->add_child(popup);
        }

        if (!item_list->is_connected("gui_input", Callable(this, "_on_item_list_gui_input"))) {
                item_list->connect("gui_input", Callable(this, "_on_item_list_gui_input"));
        }

        popup->hide();
}

void ScriptSwitcher::_enter_tree()
{
        script_editor = EditorInterface::get_singleton()->get_script_editor();

        this->_load_popup();

        if (script_editor && !script_editor->is_connected("editor_script_changed", Callable(this, "_on_script_changed")))
        {
                script_editor->connect("editor_script_changed", Callable(this, "_on_script_changed"));
        }

        if (popup && !popup->is_connected("visibility_changed", Callable(this, "_on_popup_visibility_changed")))
        {
                popup->connect("visibility_changed", Callable(this, "_on_popup_visibility_changed"));
        }
}

void ScriptSwitcher::_on_popup_visibility_changed()
{
        if (!popup->is_visible())
        {
                return;
        }

        if (history.size() == 0 && script_editor->get_open_scripts().size() > 0)
        {
                this->_fill_history();
        }

        this->_update_list();

        if (item_list->get_item_count() > 0 && item_list->get_selected_items().size() == 0)
        {
                item_list->select(0);
        }

        popup->set_anchors_and_offsets_preset(Control::PRESET_CENTER_TOP, Control::PRESET_MODE_KEEP_SIZE);
}

// ? Lazy solution to fill history after first enable to avoid crash
void ScriptSwitcher::_fill_history()
{
        TypedArray<Script> open_scripts = script_editor->get_open_scripts();

        for (const Ref<Script> &script: open_scripts){
                _on_script_changed(script);
        }

        Ref<Script> current_script = script_editor->get_current_script();
        if (current_script.is_valid()){
                _on_script_changed(current_script);
        }
}

void ScriptSwitcher::_exit_tree()
{
        if (popup)
        {
                popup->queue_free();
        }

        if (script_editor)
        {
                if (script_editor->is_connected("editor_script_changed", Callable(this, "_on_script_changed")))
                {
                        script_editor->disconnect("editor_script_changed", Callable(this, "_on_script_changed"));
                }

                if (script_editor->is_connected("gui_input", Callable(this, "_on_script_editor_input")))
                {
                        script_editor->disconnect("gui_input", Callable(this, "_on_script_editor_input"));
                }
        }
}

void ScriptSwitcher::_on_script_changed(const Ref<Script> &script)
{
        if (script.is_null())
        {
                UtilityFunctions::printerr("Null script!");
                return;
        }

        String path = script->get_path();
        if (path.is_empty())
        {
                UtilityFunctions::printerr("Empty script path!");
                return;
        }

        TypedArray<Script> open_scripts = script_editor->get_open_scripts();

        // ? Remove current script from history -> insert it at the front
        std::erase_if(history, [&](const auto &history_script){
                if (history_script == path){
                        return true;
                }
                return false;
        });

        history.insert(history.begin(), path);

        // ? make sure history isn't stale; remove any not currently open
        std::erase_if(history, [&](const auto &history_script){
                for (const Ref<Script> &open_script : open_scripts) {
                        if (history_script == open_script->get_path()) {
                                return false;
                        }
                }
                return true;
        });
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

void ScriptSwitcher::_input(const Ref<InputEvent> &event)
{
        if (popup->is_visible())
        {
                return;
        }

        if (!script_editor->is_visible_in_tree() || script_editor->get_open_scripts().size() == 0)
        {
                return;
        }

        Ref<InputEventKey> key_event = event;
        if (key_event.is_valid() && key_event->is_pressed() && !key_event->is_echo())
        {
                if (key_event->get_keycode() == KEY_TAB && key_event->is_command_or_control_pressed())
                {
                        popup->show();
                        item_list->grab_focus();
                        get_viewport()->set_input_as_handled();
                }
        }
}

void ScriptSwitcher::_on_script_editor_input(const Ref<InputEvent> &event)
{
        Ref<InputEventKey> key_event = event;
        if (key_event.is_valid() && key_event->is_pressed() && !key_event->is_echo())
        {
                if (key_event->get_keycode() == KEY_TAB && key_event->is_command_or_control_pressed())
                {
                        if (!popup->is_visible())
                        {
                                popup->show();
                                item_list->grab_focus();
                                script_editor->accept_event();
                        }
                }
        }
}

void ScriptSwitcher::_on_item_list_gui_input(const Ref<InputEvent> &event)
{
        Ref<InputEventKey> key_event = event;
        if (!key_event.is_valid())
        {
                return;
        }

        auto key = key_event->get_keycode();
        bool pressed = key_event->is_pressed();

        if (pressed)
        {
                if (key == KEY_TAB || key == KEY_DOWN || key == KEY_UP)
                {
                        if (item_list->get_item_count() > 0)
                        {
                                PackedInt32Array selected = item_list->get_selected_items();
                                int selected_index = selected.size() > 0 ? selected[0] : 0;
                                int next_index = selected_index;

                                if (key == KEY_TAB || key == KEY_DOWN)
                                {
                                        next_index = (selected_index + 1) % item_list->get_item_count();
                                }
                                else if (key == KEY_UP)
                                {
                                        next_index = (selected_index - 1 + item_list->get_item_count()) % item_list->get_item_count();
                                }

                                item_list->select(next_index);
                                item_list->ensure_current_is_visible();
                        }
                        item_list->accept_event();
                }
        }
        else // Released
        {
                if (key == KEY_CTRL)
                {
                        popup->hide();
                        item_list->accept_event();

                        if (item_list->get_item_count() == 0)
                        {
                                return;
                        }

                        PackedInt32Array selected = item_list->get_selected_items();
                        int selected_index = selected.size() > 0 ? selected[0] : 0;

                        if (selected_index < history.size())
                        {
                                String path = history[selected_index];
                                Ref<Resource> script_res = ResourceLoader::get_singleton()->load(path);
                                if (script_res.is_valid())
                                {
                                        EditorInterface::get_singleton()->edit_resource(script_res);
                                }
                        }
                }
        }
}
