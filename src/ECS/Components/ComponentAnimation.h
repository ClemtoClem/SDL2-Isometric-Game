#ifndef __COMPONENT_ANIMATION_H
#define __COMPONENT_ANIMATION_H

#include "../../Texture.h"
#include "../../Timer.h"
#define COMPONENT_ANIMATION_NUM_INITIAL_ANIMATIONS  2

#define ANIMATION_STATE_NONE    -1

typedef enum EntityDirectionInWorld {
    ENTITY_WORLD_DIRECTION_UP           = 0,
    ENTITY_WORLD_DIRECTION_DOWN         = 1,
    ENTITY_WORLD_DIRECTION_LEFT         = 2,
    ENTITY_WORLD_DIRECTION_RIGHT        = 3,
    ENTITY_WORLD_DIRECTION_UPLEFT       = 4,
    ENTITY_WORLD_DIRECTION_UPRIGHT      = 5,
    ENTITY_WORLD_DIRECTION_DOWNLEFT     = 6,
    ENTITY_WORLD_DIRECTION_DOWNRIGHT    = 7,
} EntityDirectionInWorld;

//forward declaration of Scene, allows us to use the Scene struct without causing a cross-referencing header error
typedef struct Scene Scene;

typedef struct AnimationFrame {
    SDL_Rect clipRect;          //clip rectangle for the animation frame
    int frameTimeMilliSeconds;  //how long this frame should be displayed
} AnimationFrame;

typedef struct Animation {
    Texture *texture;          //pointer to the texture
    char *name;                 //name of the animation
    AnimationFrame *frames;    //animation frames
    int numFrames;              //number of frames in the animation
    int currentFrame;           //current frame to draw
    Timer frameTime;           //frame timer
} Animation;

typedef struct ComponentAnimation {
    Animation *animations;             //pointer to the animations
    int animationState;                 //which animation to show
    EntityDirectionInWorld direction;  //the direction the entity is facing
    int numAnimations;                  //number of animations
    int maxAnimations;                  //current max number of animations
} ComponentAnimation;

[[nodiscard]] ComponentAnimation *ComponentAnimation_New();
void ComponentAnimation_Free(ComponentAnimation *componentAnimation,int maxEntities);
void ComponentAnimation_AllocateMoreMemory(Scene *scene,int componentIndex);
int ComponentAnimation_CreateNewAnimation(ComponentAnimation *componentAnimation,int entity,Texture *texture,char *animationName);
int ComponentAnimation_AddAnimationFrames(ComponentAnimation *componentAnimation,int entity,int animationIndex,
                                         int width,int height,int numFrames,int startFrameIndex, int frameTimeMilliSeconds);
void ComponentAnimation_SetAnimationFrameTime(ComponentAnimation *componentAnimation,int entity,int animationIndex,int frame,int frameTimeMilliseconds);
int ComponentAnimation_GetAnimationIndexByName(ComponentAnimation *componentAnimation,int entity,char *animationName);
void ComponentAnimation_SetAnimationState(ComponentAnimation *componentAnimation,int entity,char *animationName);

#endif // __COMPONENT_ANIMATION_H

