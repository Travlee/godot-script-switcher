#include <godot_cpp/classes/editor_interface.hpp>
#include <godot_cpp/classes/input_event_key.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/v_box_container.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/classes/style_box.hpp>

#include <godot_cpp/classes/theme.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/packed_scene.hpp>

#include "script_switcher.hpp"

using namespace godot;

ScriptSwitcher::ScriptSwitcher() {}
ScriptSwitcher::~ScriptSwitcher() {}

void ScriptSwitcher::_bind_methods()
{
        UtilityFunctions::print("_bind_methods()");

        ClassDB::bind_method(D_METHOD("_on_script_changed", "script"), &ScriptSwitcher::_on_script_changed);

        ClassDB::bind_method(D_METHOD("_enter_tree"), &ScriptSwitcher::_enter_tree);
        ClassDB::bind_method(D_METHOD("_exit_tree"), &ScriptSwitcher::_exit_tree);

        ClassDB::bind_method(D_METHOD("_update_list_ui"), &ScriptSwitcher::_update_list_ui);
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

        // EditorInterface::get_singleton()->get_base_control()->add_child(popup);
        add_child(popup);

        // Ref<Theme> editor_theme = EditorInterface::get_singleton()->get_base_control()->get_theme();
        // Ref<Theme> editor_theme = EditorInterface::get_singleton()->get_base_control()->get_theme_stylebox("panelcontainer", "EditorStyles");
        // popup->set_theme(editor_theme);
        popup->hide();

        // You can now access children by name if needed
        // Label *lbl = panel_instance->get_node<Label>("MyLabel");



        // ? Archive
        // popup = memnew(PanelContainer);
        // item_list = memnew(ItemList);

        // popup->set_anchors_and_offsets_preset(Control::PRESET_CENTER_TOP);
        // popup->set_z_index(1000);

        // VBoxContainer *vbox = memnew(VBoxContainer);
        // Label *help_label = memnew(Label);
        // help_label->set_text(" Recent Scripts (Hold Ctrl to cycle)");
        // help_label->set_mouse_filter(Control::MOUSE_FILTER_PASS);

        // vbox->add_child(help_label);
        // vbox->add_child(item_list);
        // item_list->set_v_size_flags(Control::SIZE_EXPAND_FILL);
        // popup->add_child(vbox);
        // popup->hide();

        // popup->connect("_input", Callable(this, "_popup_input"));
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
        if (!script_editor->is_visible_in_tree()){
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
                this->_update_list_ui();
                popup->show();

                get_viewport()->set_input_as_handled();

                UtilityFunctions::print("popup->show()");

                return;
        }

        if (popup->is_visible() && !key_event->is_pressed() && key == KEY_CTRL)
        {
                popup->hide();
                get_viewport()->set_input_as_handled();
                UtilityFunctions::print("popup->hide()");
        }

        if (popup->is_visible() && key_event->is_pressed() && key == KEY_TAB)
        {
                get_viewport()->set_input_as_handled();
                UtilityFunctions::print("itemlist->cycle");
        }

        return;

        // if (key_event->is_pressed() && key_event->get_keycode() == KEY_TAB && key_event->is_command_or_control_pressed())
        // {
        //         if (!popup->is_visible())
        //         {
        //                 _update_list_ui();
        //                 popup->popup();

        //                 // Select second item (the "previous" script)
        //                 if (item_list->get_item_count() > 1)
        //                 {
        //                         item_list->select(1);
        //                 }
        //         }
        //         else
        //         {
        //                 // Cycle selection
        //                 int next = (item_list->get_selected_items()[0] + 1) % item_list->get_item_count();
        //                 item_list->select(next);
        //         }

        //         get_viewport()->set_input_as_handled();
        // }

        // // Logic: If Ctrl is released, switch to selected script
        // if (!key_event->is_pressed() && !key_event->is_command_or_control_pressed() && popup->is_visible())
        // {
        //         int selected = item_list->get_selected_items()[0];
        //         String path = history[selected];

        //         Ref<Resource> res = ResourceLoader::get_singleton()->load(path);
        //         EditorInterface::get_singleton()->edit_resource(res);

        //         popup->hide();
        //         get_viewport()->set_input_as_handled();
        // }
}

void ScriptSwitcher::_update_list_ui()
{
        item_list->clear();
        for (const String &path : history)
        {
                item_list->add_item(path.get_file());
                item_list->set_item_tooltip(item_list->get_item_count() - 1, path);
        }
}
