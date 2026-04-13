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

        void _update_list();
        void _load_popup();
        void _fill_history();

protected:
        static void _bind_methods();

public:
        ScriptSwitcher();
        ~ScriptSwitcher();

        void _on_popup_visibility_changed();

        void _on_script_changed(const Ref<Script> &script);

        virtual void _input(const Ref<InputEvent> &event) override;

        virtual void _enter_tree() override;

        virtual void _exit_tree() override;
};
