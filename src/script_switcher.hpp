#pragma once

#include <godot_cpp/classes/editor_plugin.hpp>
#include <godot_cpp/classes/script_editor.hpp>
#include <godot_cpp/classes/item_list.hpp>
#include <godot_cpp/classes/panel_container.hpp>
#include <godot_cpp/classes/script.hpp>
#include <godot_cpp/classes/input_event.hpp>

#include <vector>

using namespace godot;

class ScriptSwitcher : public EditorPlugin
{
        GDCLASS(ScriptSwitcher, EditorPlugin);

private:
        PanelContainer *popup;
        ScriptEditor *script_editor;
        ItemList *item_list;
        std::vector<String> history;

        void _update_list_ui();
        void _load_popup();

protected:
        static void _bind_methods();

public:
        ScriptSwitcher();
        ~ScriptSwitcher();

        void _on_script_changed(const Ref<Script> &script);

        virtual void _input(const Ref<InputEvent> &event) override;

        void _popup_input(const Ref<InputEvent> &event);

        void _enter_tree() override;

        void _exit_tree() override;
};
