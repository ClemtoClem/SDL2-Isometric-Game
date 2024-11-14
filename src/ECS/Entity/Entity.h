#ifndef __ENTITY_H
#define __ENTITY_H

#include <SDL2/SDL.h>
#include "../Components/Component.h"

typedef struct Entity {
    Uint32 id;                  // unique ID for the entity

    // Each Uint32 can hold 32 components (there are 32 bits in a 32bit integer)
    Uint32 componentSet1;       // First set of 32 components
    //Uint32 componentSet2;     // Second set of 32 components
    //Uint32 componentSet3;     // Add this for a 3rd set of components
                                // Add even more sets here if needed
} Entity;

#endif // __ENTITY_H

