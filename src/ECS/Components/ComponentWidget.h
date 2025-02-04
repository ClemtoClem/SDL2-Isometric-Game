#ifndef __COMPONENT_WIDGET_H
#define __COMPONENT_WIDGET_H

#include <SDL2/SDL.h>

//forward declaration of Scene, allows us to use the Scene without causing a cross-referencing header error
typedef struct Scene Scene;


//Enum of widget states
typedef enum WidgetStateType {
    COMPONENT_WIDGET_STATE_IDLE          = 0,       // Default state when no interaction occurs.
    COMPONENT_WIDGET_STATE_DISABLED      = 1u << 0, // Visible but unusable.
    COMPONENT_WIDGET_STATE_HIDDEN        = 1u << 1, // Present in the tree but not displayed.

    COMPONENT_WIDGET_STATE_HOVERABLE     = 1u << 2, 
    COMPONENT_WIDGET_STATE_HOVERED       = 1u << 3, // Mouse or navigation hover.

    COMPONENT_WIDGET_STATE_SELECTABLE    = 1u << 4,
    COMPONENT_WIDGET_STATE_SELECTED      = 1u << 5, // The user validated the item.

    COMPONENT_WIDGET_STATE_FOCUSABLE     = 1u << 6,
    COMPONENT_WIDGET_STATE_FOCUSED       = 1u << 7, // Focused via keyboard/controller.

    COMPONENT_WIDGET_STATE_HIGHLIGHTABLE = 1u << 8,
    COMPONENT_WIDGET_STATE_HIGHLIGHTED   = 1u << 9, // Element is brought forward.

    COMPONENT_WIDGET_STATE_PRESSED       = 1u << 10, // Clicked or button pressed.
    COMPONENT_WIDGET_STATE_HELD          = 1u << 11, // Prolonged press.

    COMPONENT_WIDGET_STATE_DRAGGABLE     = 1u << 12, // Can be moved.
    COMPONENT_WIDGET_STATE_DRAGGED       = 1u << 13, // Currently being moved.
} WidgetStateType;


typedef struct ComponentWidget {
    WidgetStateType state;
    Uint32 parent;
    Uint32 *childs;
    Uint32 nbOfChilds;
} ComponentWidget;

[[nodiscard]] ComponentWidget *ComponentWidget_New();
void ComponentWidget_AllocMoreMemory(Scene *scene, int componentIndex);
void ComponentWidget_Free(ComponentWidget *componentWidget, int numEntities);


#endif // __COMPONENT_WIDGET_H