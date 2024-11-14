#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ComponentAnimation.h"
#include "../Scene/Scene.h"
#include "../../logger.h"

ComponentAnimation *ComponentAnimation_New() {
    int i = 0;    //allocate memory for the animation component
    ComponentAnimation *newAnimComponent = malloc(sizeof(struct ComponentAnimation)*NUM_INITIAL_ENTITIES);

    //if the memory allocation failed
    if (newAnimComponent == NULL) {
        //Log it as an error
        WriteError("Could not allocate memory for animation component!");
        return NULL;
    }
    //initialize the animations
    for (i = 0; i < NUM_INITIAL_ENTITIES; ++i) {
        newAnimComponent[i].animations = NULL;
        newAnimComponent[i].numAnimations = 0;
        newAnimComponent[i].maxAnimations = 0;
        newAnimComponent[i].animationState = ANIMATION_STATE_NONE;
        newAnimComponent[i].direction = ENTITY_WORLD_DIRECTION_DOWN;
    }
    //return the new component
    return newAnimComponent;
}

void ComponentAnimation_AllocateMoreMemory(Scene *scene,int componentIndex) {
    int j = 0;
    ComponentAnimation *newComponentAnimation = realloc((ComponentAnimation*)scene->components[componentIndex].data,sizeof(struct ComponentAnimation)*scene->maxEntities);
    if (newComponentAnimation == NULL) {
        //write the error to the logfile
        WriteError("Could not allocate more memory for animation components! ");
        //mark that memory allocation failed
        scene->memallocFailed = 1;
        return;
    }
    //initialize the component for the new entities
    for (j=scene->numEntities;j < scene->maxEntities; ++j) {
        newComponentAnimation[j].animations = NULL;
        newComponentAnimation[j].numAnimations = 0;
        newComponentAnimation[j].maxAnimations = 0;
        newComponentAnimation[j].animationState = ANIMATION_STATE_NONE;
    }
    //point the data pointer to the new data
    scene->components[componentIndex].data = newComponentAnimation;
}

int ComponentAnimation_CreateNewAnimation(ComponentAnimation *componentAnimation,int entity,Texture *texture,char *animationName) {
    int i = 0;
    Animation *newAnimations = NULL;
    Animation *animations;

    if (componentAnimation == NULL) {
        //Log it as an error
        WriteError("Parameter: 'ComponentAnimation *componentAnimation' is NULL! Animation:%s was not created!",animationName);
        //exit out of the function
        return -1;
    }

    //if the texture is NULL
    if (texture == NULL) {
        //Log it as an error
        WriteError("Parameter: 'Texture *texture' is NULL! Animation:%s was not created!",animationName);
        //exit out of the function
        return -1;
    }
    //if the name of the animation is NULL
    if (animationName == NULL) {
        //Log it as an error
        WriteError("Parameter: 'char *animationName' is NULL! Animation:%s was not created!",animationName);
        //exit out of the function
        return-1;
    }

    //if the animations are not allocated
    if (componentAnimation[entity].animations == NULL) {
        //allocate memory for them
        componentAnimation[entity].animations = malloc(sizeof (struct Animation)*COMPONENT_ANIMATION_NUM_INITIAL_ANIMATIONS);
        //if memory allocation failed
        if (componentAnimation[entity].animations == NULL) {
            //Log it as an error
            WriteError("Could not allocate memory for animations for animation:%s",animationName);
            //exit out of the function
            return -1;
        }
        //initialize the animation component
        componentAnimation[entity].maxAnimations = COMPONENT_ANIMATION_NUM_INITIAL_ANIMATIONS;
        componentAnimation[entity].numAnimations = 0;

        //initialize the animations
        for (i = 0; i < COMPONENT_ANIMATION_NUM_INITIAL_ANIMATIONS; ++i) {
            componentAnimation[entity].animations[i].currentFrame = 0;
            componentAnimation[entity].animations[i].frames = NULL;
            componentAnimation[entity].animations[i].name = NULL;
            componentAnimation[entity].animations[i].numFrames = 0;
            componentAnimation[entity].animations[i].texture = NULL;
        }
    }

    //allocate more memory for animations if needed
    if (componentAnimation[entity].numAnimations >= componentAnimation[entity].maxAnimations) {
        //Try to allocate memory for another 5 animations
        componentAnimation[entity].maxAnimations += 5;
        newAnimations = realloc(componentAnimation[entity].animations,sizeof(struct Animation)*componentAnimation[entity].maxAnimations);

        //if memory allocation failed
        if (newAnimations == NULL) {
            //Log it as an error
            WriteError("Could not add more animations. Re-allocation of memory for animations for animation:%s failed!",animationName);

            //roll back number of max animations
            componentAnimation[entity].maxAnimations -= 5;

            //exit out of the function
            return -1;
        }
                //point to the new memory location
        componentAnimation[entity].animations = newAnimations;

        //initialize the animations
        for (i=componentAnimation[entity].numAnimations;i < componentAnimation[entity].maxAnimations; ++i) {
            componentAnimation[entity].animations[i].currentFrame = 0;
            componentAnimation[entity].animations[i].frames = NULL;
            componentAnimation[entity].animations[i].name = NULL;
            componentAnimation[entity].animations[i].numFrames = 0;
            componentAnimation[entity].animations[i].texture = NULL;
        }
    }

    //for less typing, point the animation pointer to the animation;
    animations = componentAnimation[entity].animations;

    //allocate memory for the animation name
    animations[componentAnimation[entity].numAnimations].name = malloc(sizeof(char)*strlen(animationName)+1);
    //if memory allocation for the name failed
    if (animations[componentAnimation[entity].numAnimations].name == NULL) {
        //Log it as an error
        WriteError("Could not allocate memory for animation name!");
        //exit out of the function
        return -1;
    }

    //set the name
    sprintf(animations[componentAnimation[entity].numAnimations].name,"%s",animationName);

    //set the texture pointer
    animations[componentAnimation[entity].numAnimations].texture = texture;

    //initialize the frame timer to 100ms
    Timer_Init(&animations[componentAnimation[entity].numAnimations].frameTime,100);

    //increase number of animations
    componentAnimation[entity].numAnimations++;

    //return the index of the last created animation
    return componentAnimation[entity].numAnimations-1;
}

int ComponentAnimation_AddAnimationFrames(ComponentAnimation *componentAnimation,int entity,int animationIndex,
                                         int width,int height,int numFrames,int startFrameIndex, int frameTimeMilliSeconds) {
    int x=0,y=0,i = 0, j = 0;
    int w=0,h=0;
    int textureSizeCheckX=0;
    int textureSizeCheckY=0;
    int maxFramesInImage=0;

    AnimationFrame *frames = NULL;

    if (componentAnimation == NULL) {
        WriteError("Parameter ComponentAnimation *componentAnimation is NULL!");
        return -1;
    }

    if (componentAnimation[entity].animations == NULL) {
        //Log it as an error
        WriteError("componentAnimation[%d].animations pointer is NULL.",entity);
        //exit out of the function
        return -1;
    }

    if (animationIndex > componentAnimation[entity].numAnimations) {
        //Log it as an error
        WriteError("Parameter: 'int animationIndex' is larger than number of animations");
        //exit out of the function
        return -1;
    }

    //If number of frames are invalid
    if (numFrames <= 0) {
        //Log it as an error
        WriteError("Parameter: 'int numFrames' is <= 0. Animation frames were not added.");
        //exit out of the function
        return -1;
    }

    //If parameter numFrames is more than 200 frames
    if (numFrames > 200) {
        //Log it as a warning
        WriteWarning("Parameter int numFrames is > 200, are you sure this is correct?");
    }

    //if frame time is less than 10 milliseconds
    if (frameTimeMilliSeconds<10) {
        //set it to 10 milliseconds
        frameTimeMilliSeconds = 10;
    }

    //if the texture is NULL
    if (componentAnimation[entity].animations[animationIndex].texture == NULL) {
        //Log it as an error
        WriteError("Cannot add animation frames! The animation:%s texture is NULL.",componentAnimation[entity].animations[animationIndex].name);
        //exit out of the function
        return -1;
    }

    //get width and height of the texture
    w = componentAnimation[entity].animations[animationIndex].texture->width;
    h = componentAnimation[entity].animations[animationIndex].texture->height;

    if (w < width) {
        WriteError("Texture width is smaller (%d) than the animation width (%d)! Aborting!",width,w);
        return -1;
    }
    if (h<height) {
        WriteError("Texture height is smaller (%d) than the animation height(%d)! Aborting!",height,h);
        return -1;
    }

    //calculate number of frames that can fit within the animation texture
    textureSizeCheckX = w/width;
    textureSizeCheckY = h/height;

    maxFramesInImage = textureSizeCheckX * textureSizeCheckY;
    //if the texture cannot contain all the frames
    if ( maxFramesInImage < numFrames) {
        //write a friendly warning about it in the log file
        WriteWarning("Animation:%s texture size can only hold:%d frames. Parameter: int numFrames is:%d",componentAnimation[entity].animations[animationIndex].name,(textureSizeCheckX * textureSizeCheckY),numFrames);
        return -1;
    }

    //allocate memory for the new frames
    componentAnimation[entity].animations[animationIndex].frames = malloc(sizeof(struct AnimationFrame)*numFrames);
    if (componentAnimation[entity].animations[animationIndex].frames == NULL) {
        //Log it as an error
        WriteError("Could not allocate memory for animation frames for animation:%s!",componentAnimation[entity].animations[animationIndex].name);
        //exit out of the function
        return -1;
    }
    //for less typing, point the frames pointer to the frames;
    frames = componentAnimation[entity].animations[animationIndex].frames;

    //loop through all the animation frames
    while (i <= maxFramesInImage) {
        //if we have found a frame within the animation
        if (i >= startFrameIndex) {
            //set the size of the clip rectangle
            SetupRect(&frames[j].clipRect,x,y,width,height);
            //set frame time
            frames[j].frameTimeMilliSeconds = frameTimeMilliSeconds;
            //go to the next frame in the animation
            j++;

            //if all frames are accounted for
            if (j >= numFrames) {
                //break out of the while loop
                break;
            }
        }

        //go to next frame in the texture
        x+=width;
        //if x has passed the width of the image
        if (x>=w) {
            //reset x to 0
            x=0;
            //go to the next row in the image
            y+=height;
        }
        //go to the next frame in the image
        ++i;
    }

    //set number of frames in the animation
    componentAnimation[entity].animations[animationIndex].numFrames = numFrames;
    return 1;
}

int ComponentAnimation_GetAnimationIndexByName(ComponentAnimation *componentAnimation,int entity,char *animationName) {
    int i = 0;
    //if the component animation is invalid
    if (componentAnimation == NULL) {
        //Log it as an error
        WriteError("Parameter: 'ComponentAnimation *componentAnimation' is NULL!");
        //exit out of the function
        return -1;
    }
    //if the animation name is invalid
    if (animationName == NULL) {
        //Log it as an error
        WriteError("Parameter: 'char *animationName' is NULL!");
        //exit out of the function
        return -1;
    }
    //if entity has a negative value
    if (entity < 0) {
        //Log it as an error
        WriteError("Parameter: 'int entity' has a negative value!");
        //exit out of the function
        return -1;
    }

    //loop through all animations
    for (i = 0; i < componentAnimation[entity].numAnimations; ++i) {
        //if the component is found
        if (strcmp(componentAnimation[entity].animations[i].name,animationName)==0) {
            //return the index
            return i;
        }
    }
    //if the animation was not found, return -1
    return -1;
}

void ComponentAnimation_SetAnimationState(ComponentAnimation *componentAnimation,int entity,char *animationName) {
    int i = 0;
    //if the component animation is invalid
    if (componentAnimation == NULL) {
        //Log it as an error
        WriteError("Parameter: 'ComponentAnimation *componentAnimation' is NULL!");
        //exit out of the function
        return;
    }
    //if the animation name is invalid
    if (animationName == NULL) {
        //Log it as an error
        WriteError("Parameter: 'char *animationName' is NULL!");
        //exit out of the function
        return;
    }
    //if entity has a negative value
    if (entity<0) {
        //Log it as an error
        WriteError("Parameter: 'int entity' has a negative value!");
        //exit out of the function
        return;
    }
    //loop through all animations
    for (i = 0; i < componentAnimation[entity].numAnimations; ++i) {
        //if the component is found
        if (strcmp(componentAnimation[entity].animations[i].name,animationName)==0) {
            //set the animation state to the animation
            componentAnimation[entity].animationState = i;
            //return out of the function
            return;
        }
    }
}
//this function sets frame time for individual frames in an animation
void ComponentAnimation_SetAnimationFrameTime(ComponentAnimation *componentAnimation,int entity,int animationIndex,int frame,int frameTimeMilliseconds) {

    //if the component animation is not allocated
    if (componentAnimation == NULL) {
        //Log it as an error
        WriteError("Parameter: 'ComponentAnimation *componentAnimation' is NULL!");
        //exit out of the function
        return;
    }
    //if the animation index is negative
    if (animationIndex <= 0) {
        //Log it as an error
        WriteError("Parameter: 'int animationIndex' is less than 0");
        //exit out of the function
        return;
    }
    //if the index is out of bounds
    if (animationIndex > componentAnimation[entity].numAnimations) {
        //Log it as an error
        WriteError("Parameter: 'int animationIndex' is larger than the number of animations in the animation");
        //exit out of the function
        return;
    }

    //if the animation does not have any frames allocated
    if (componentAnimation[entity].animations[animationIndex].frames == NULL) {
        //Log it as an error
        WriteError("Animation:%s 'frames' pointer is NULL!",componentAnimation[entity].animations[animationIndex].name);
        //exit out of the function
        return;
    }

    //if the frame is larger than the number of frames in the animation
    if (frame > componentAnimation[entity].animations->numFrames) {//Log it as an error
        
        WriteError("Parameter: 'int frame' is larger than the number of frames in the animation:%s!",componentAnimation[entity].animations[animationIndex].name);
        //exit out of the function
        return;
    }

    //set the frame time for the animation frame.
    componentAnimation[entity].animations->frames[frame].frameTimeMilliSeconds = frameTimeMilliseconds;
}

void ComponentAnimation_Free(ComponentAnimation *componentAnimation,int maxEntities) {
    int i = 0, j = 0;
    if (componentAnimation!=NULL) {
        //loop through all entities in the component
        for (i = 0; i < maxEntities; ++i) {
            //if the animation is not NULL
            if (componentAnimation[i].animations!=NULL) {
                //loop through all the animations
                for (j = 0; j < componentAnimation[i].numAnimations; ++j) {
                    //If the animation frames are not NULL
                    if (componentAnimation[i].animations[j].frames != NULL) {
                        //Free them
                        free(componentAnimation[i].animations[j].frames);
                    }
                    //if the animation name is not NULL
                    if (componentAnimation[i].animations[j].name != NULL) {
                        //free it
                        free(componentAnimation[i].animations[j].name);
                    }

                    //textures are not stored in the components themselves, therefore we will
                    //not free the memory the texture pointer is pointing to.
                }
                //free the animations
                free(componentAnimation[i].animations);
            }
        }
        //free the animation component
        free(componentAnimation);
    }
}
