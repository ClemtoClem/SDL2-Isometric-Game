#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "initclose.h"
#include "renderer.h"
#include "Texture.h"
#include "TexturePool.h"
#include "IsoEngine/isoEngine.h"
#include "ECS/Scene/SceneManager.h"
#include "logger.h"
#include "FontPool.h"

#define MAP_HEIGHT 640
#define MAP_WIDTH 640

typedef struct Game {
    SceneManager *sceneManager;
    TexturePool *texturePool;
    FontPool *fontPool;
} Game;

Game game;

static ComponentInputKeyboard *inputKeyboard = NULL;
static ComponentInputMouse *inputMouse = NULL;
static ComponentNameTag *nameTag = NULL;
static ComponentPosition *position = NULL;
static ComponentRender2D *render = NULL;
static ComponentVelocity *velocity = NULL;
static ComponentCollision *collision = NULL;
static ComponentAnimation *animationComp = NULL;

static void updateComponentPointers(Scene *scene,int firstRun) {
    if (scene->componentPointersReallocated == 1 || firstRun == 1) {
        ///get the pointers to the components
        //get the name tag components pointer from the scene
        nameTag = (ComponentNameTag*) Scene_GetComponent(scene, COMPONENT_SET1_NAMETAG);
        //get the keyboard components pointer from the scene
        inputKeyboard = (ComponentInputKeyboard*) Scene_GetComponent(scene, COMPONENT_SET1_KEYBOARD);
        //get the mouse components pointer from the scene
        inputMouse = (ComponentInputMouse*) Scene_GetComponent(scene,COMPONENT_SET1_MOUSE);
        //get the position components pointer from the scene
        position = (ComponentPosition*) Scene_GetComponent(scene,COMPONENT_SET1_POSITION);
        //get the render2D components pointer from the scene
        render = (ComponentRender2D*) Scene_GetComponent(scene,COMPONENT_SET1_RENDER2D);
        //get the velocity components pointer from the scene
        velocity = (ComponentVelocity*) Scene_GetComponent(scene,COMPONENT_SET1_VELOCITY);
        //get the collision components pointer from the scene
        collision = (ComponentCollision*) Scene_GetComponent(scene,COMPONENT_SET1_COLLISION);
        //get the animation components pointer from the scene
        animationComp = (ComponentAnimation*) Scene_GetComponent(scene,COMPONENT_SET1_ANIMATION);
    }
}

void init() {
    int entity;
    int entityAnimation;
    int i = 0;
    
    SDL_Rect tmpRect;

    // initialize SDL
    LoggerInitialize();

    //Create a new pool to hold all textures
    game.texturePool = TexturePool_New();
    //if memory allocation failed
    if (game.texturePool == NULL) {
        //exit the program
        exit(-1);
    }
    game.fontPool = FontPool_NewFontPool(4);

    //if memory allocation failed
    if (game.fontPool == NULL) {
        TexturePool_Free(game.texturePool);
        closeDownSDL();
        exit(-1);
    }

    ///Load the textures to the texture pool
    TexturePool_AddTexture(game.texturePool, "data/isotiles.png");
    TexturePool_AddTexture(game.texturePool, "data/character.png");
    TexturePool_AddTexture(game.texturePool, "data/isotree.png");

    //set the font pool pointer to the game.fontPool so we can access it from other files
    FontPool_SetPointer(game.fontPool);
    //add the fonts to the font pool
    FontPool_AddFont(game.fontPool,"data/cleanfont.png", "cleanFont",9,9,7);
    FontPool_AddFont(game.fontPool,"data/nuFont_32x32.png", "nuFont",32,32,34);
    FontPool_AddFont(game.fontPool,"data/8-bit_wonder_64x64.png", "8bitWonderFont",64,64,64);
    FontPool_AddFont(game.fontPool,"data/bitmgothic_64x64.png", "gothicFont",64,64,35);

    //it is possible to adjust the space for an individual character in the font as well.
    //Important, use single quotes ' ' and not double quotes " ", since it's a character and not a string.
    //The E character was hard to read at lower zoom levels, so adding extra space to it, made it readable
    FontPool_SetCharacterFontSpace(game.fontPool,"nuFont",'E',38);
    //The L character had too much space behind it
    FontPool_SetCharacterFontSpace(game.fontPool,"nuFont",'L',30);

    game.sceneManager = SceneManager_CreateNewSceneManager();
    if (game.sceneManager == NULL) {
        TexturePool_Free(game.texturePool);
        exit(-1);
    }
    SceneManager_CreateSceneInSceneManager(game.sceneManager,"testScene");
    Scene *testScene = SceneManager_GetPointerToScene(game.sceneManager,"testScene");
    if (testScene == NULL) {
        exit(-1);
    }

///ADD COMPONENTS TO THE SCENE
    Scene_AddComponentToScene(testScene,COMPONENT_SET1_POSITION);
    Scene_AddComponentToScene(testScene,COMPONENT_SET1_VELOCITY);
    Scene_AddComponentToScene(testScene,COMPONENT_SET1_KEYBOARD);
    Scene_AddComponentToScene(testScene,COMPONENT_SET1_MOUSE);
    Scene_AddComponentToScene(testScene,COMPONENT_SET1_RENDER2D);
    Scene_AddComponentToScene(testScene,COMPONENT_SET1_NAMETAG);
    Scene_AddComponentToScene(testScene,COMPONENT_SET1_COLLISION);
    Scene_AddComponentToScene(testScene,COMPONENT_SET1_ANIMATION);

    //if there was a memory allocation problem anywhere in the scene
    if (testScene->memallocFailed == 1) {
        //free the scene and log the error
        WriteError("Memory allocation error(s) while setting up scene:%s, aborting...",testScene->name);
        Scene_FreeScene(testScene);
        exit(-1);
    }
    updateComponentPointers(testScene,1);

///ADD SYSTEMS TO THE SCENE
    Scene_AddSystemToScene(testScene,SYSTEM_INPUT);
    Scene_AddSystemToScene(testScene,SYSTEM_MOVE);
    Scene_AddSystemToScene(testScene,SYSTEM_COLLISION);
    Scene_AddSystemToScene(testScene,SYSTEM_ANIMATION);
    Scene_AddSystemToScene(testScene,SYSTEM_RENDER_ISOMETRIC_WORLD);      //Run render system before isometric control system
    Scene_AddSystemToScene(testScene,SYSTEM_CONTROL_ISOMETRIC_WORLD);     //Otherwise funny artifacts can occur for drawing
    Scene_AddSystemToScene(testScene,SYSTEM_CONTROL_ENTITY);

/// ISOMETRIC CONTROL ENTITY
    //Add isometric control entity to the scene
    entity = Scene_AddEntityToScene(testScene,
             COMPONENT_SET1_KEYBOARD | COMPONENT_SET1_NAMETAG | COMPONENT_SET1_MOUSE);

    //every time we add a new entity to the scene, we have to check if the component pointers should be updated
    updateComponentPointers(testScene,0);

    //set the name of the entity
    componentNameTagSetName(nameTag,entity,"isometricControls");

    //add the arrow direction keys to the isometric controls entity
    componentInputKeyboardAddAction(inputKeyboard,entity,"up",SDL_SCANCODE_UP);
    componentInputKeyboardAddAction(inputKeyboard,entity,"down",SDL_SCANCODE_DOWN);
    componentInputKeyboardAddAction(inputKeyboard,entity,"left",SDL_SCANCODE_LEFT);
    componentInputKeyboardAddAction(inputKeyboard,entity,"right",SDL_SCANCODE_RIGHT);

    //add mouse actions to the isometric control entity
    ComponentInputMouse_AddAction(inputMouse,entity,"mouseWheel",COMPONENT_INPUTMOUSE_ACTION_MOUSEWHEEL);
    ComponentInputMouse_AddAction(inputMouse,entity,"leftButton",COMPONENT_INPUTMOUSE_ACTION_LEFTBUTTON);
    ComponentInputMouse_AddAction(inputMouse,entity,"rightButton",COMPONENT_INPUTMOUSE_ACTION_RIGHTBUTTON);
    ComponentInputMouse_AddAction(inputMouse,entity,"middleButton",COMPONENT_INPUTMOUSE_ACTION_MIDDLEBUTTON);
    componentInputKeyboardAddAction(inputKeyboard,entity,"toggleGameMode",SDL_SCANCODE_SPACE);

    //activate the mouse input
    ComponentInputMouse_SetActiveState(inputMouse,entity,1);
    //Activate keyboard input (otherwise input won't be applied on the component)
    componentInputKeyboardSetActiveState(inputKeyboard,entity,1);
//// END OF ISOMETRIC CONTROL ENTITY

/// PLAYER ENTITY
    //Add the player entity to the scene
    entity = Scene_AddEntityToScene(testScene,
             COMPONENT_SET1_POSITION | COMPONENT_SET1_VELOCITY |
             COMPONENT_SET1_NAMETAG | COMPONENT_SET1_KEYBOARD |
             COMPONENT_SET1_RENDER2D | COMPONENT_SET1_COLLISION |
             COMPONENT_SET1_ANIMATION);

    //every time we add a new entity to the scene, we have to check if the component pointers should be updated
    updateComponentPointers(testScene,0);

    //set the name of the entity
    componentNameTagSetName(nameTag,entity,"player1");
    //set the player start position
    ComponentPosition_SetPosition(position,entity,0,0);
    //move the player y position up a bit
    ComponentPosition_SetOffset(position,entity,0,-38);
    //set friction to 100
    ComponentVelocity_SetFriction(velocity,entity,100);

    //set keys to control the player
    componentInputKeyboardAddAction(inputKeyboard,entity,"up",SDL_SCANCODE_W);
    componentInputKeyboardAddAction(inputKeyboard,entity,"down",SDL_SCANCODE_S);
    componentInputKeyboardAddAction(inputKeyboard,entity,"left",SDL_SCANCODE_A);
    componentInputKeyboardAddAction(inputKeyboard,entity,"right",SDL_SCANCODE_D);

    //enable the keyboard input for the player entity
    componentInputKeyboardSetActiveState(inputKeyboard,entity,1);

    //tell the entity control system to work on the player entity
    SystemControlEntity_SetEntityToControlByID(testScene,entity);

    //set the kind of collisions the entity will handle
    ComponentCollision_SetCollisionType(collision,entity,COLLISIONTYPE_WORLD_AND_ENTITY);
    SetupRect(&tmpRect,0,0,8,8);
    ComponentCollision_SetCollisionRectangle(collision,entity,&tmpRect);

    //set the clip rectangle (forward frame of the character)
    SetupRect(&tmpRect,350,0,70,70);

    //set the texture to render
    ComponentRender2D_SetTextureAndClipRect(render,entity,TexturePool_GetTexture(game.texturePool,"character.png"),&tmpRect);
    ComponentPosition_SetOffset(position,entity,0,-4);
    ComponentRender2D_SetLayer(render,entity,1);

    // IDLE ANIMATIONS - CURRENTLY JUST 1 FRAME

    entityAnimation = ComponentAnimation_CreateNewAnimation(animationComp,entity,TexturePool_GetTexture(game.texturePool,"character.png"),"idleUpLeft");
    ComponentAnimation_AddAnimationFrames(animationComp,entity,entityAnimation,70,70,1,0,100);
    entityAnimation = ComponentAnimation_CreateNewAnimation(animationComp,entity,TexturePool_GetTexture(game.texturePool,"character.png"),"idleUp");
    ComponentAnimation_AddAnimationFrames(animationComp,entity,entityAnimation,70,70,1,1,100);
    entityAnimation = ComponentAnimation_CreateNewAnimation(animationComp,entity,TexturePool_GetTexture(game.texturePool,"character.png"),"idleUpRight");
    ComponentAnimation_AddAnimationFrames(animationComp,entity,entityAnimation,70,70,1,2,100);
    entityAnimation = ComponentAnimation_CreateNewAnimation(animationComp,entity,TexturePool_GetTexture(game.texturePool,"character.png"),"idleRight");
    ComponentAnimation_AddAnimationFrames(animationComp,entity,entityAnimation,70,70,1,3,100);
    entityAnimation = ComponentAnimation_CreateNewAnimation(animationComp,entity,TexturePool_GetTexture(game.texturePool,"character.png"),"idleDownRight");
    ComponentAnimation_AddAnimationFrames(animationComp,entity,entityAnimation,70,70,1,4,100);
    entityAnimation = ComponentAnimation_CreateNewAnimation(animationComp,entity,TexturePool_GetTexture(game.texturePool,"character.png"),"idleDown");
    ComponentAnimation_AddAnimationFrames(animationComp,entity,entityAnimation,70,70,1,5,100);
    entityAnimation = ComponentAnimation_CreateNewAnimation(animationComp,entity,TexturePool_GetTexture(game.texturePool,"character.png"),"idleDownLeft");
    ComponentAnimation_AddAnimationFrames(animationComp,entity,entityAnimation,70,70,1,6,100);
    entityAnimation = ComponentAnimation_CreateNewAnimation(animationComp,entity,TexturePool_GetTexture(game.texturePool,"character.png"),"idleLeft");
    ComponentAnimation_AddAnimationFrames(animationComp,entity,entityAnimation,70,70,1,7,100);

    // WALK ANIMATIONS
    entityAnimation = ComponentAnimation_CreateNewAnimation(animationComp,entity,TexturePool_GetTexture(game.texturePool,"character.png"),"walkDown");
    ComponentAnimation_AddAnimationFrames(animationComp,entity,entityAnimation,70,70,8,8,100);
    entityAnimation = ComponentAnimation_CreateNewAnimation(animationComp,entity,TexturePool_GetTexture(game.texturePool,"character.png"),"walkUp");
    ComponentAnimation_AddAnimationFrames(animationComp,entity,entityAnimation,70,70,8,16,100);
    entityAnimation = ComponentAnimation_CreateNewAnimation(animationComp,entity,TexturePool_GetTexture(game.texturePool,"character.png"),"walkLeft");
    ComponentAnimation_AddAnimationFrames(animationComp,entity,entityAnimation,70,70,8,24,100);
    entityAnimation = ComponentAnimation_CreateNewAnimation(animationComp,entity,TexturePool_GetTexture(game.texturePool,"character.png"),"walkRight");
    ComponentAnimation_AddAnimationFrames(animationComp,entity,entityAnimation,70,70,8,32,100);
    entityAnimation = ComponentAnimation_CreateNewAnimation(animationComp,entity,TexturePool_GetTexture(game.texturePool,"character.png"),"walkDownLeft");
    ComponentAnimation_AddAnimationFrames(animationComp,entity,entityAnimation,70,70,8,40,100);
    entityAnimation = ComponentAnimation_CreateNewAnimation(animationComp,entity,TexturePool_GetTexture(game.texturePool,"character.png"),"walkDownRight");
    ComponentAnimation_AddAnimationFrames(animationComp,entity,entityAnimation,70,70,8,48,100);
    entityAnimation = ComponentAnimation_CreateNewAnimation(animationComp,entity,TexturePool_GetTexture(game.texturePool,"character.png"),"walkUpRight");
    ComponentAnimation_AddAnimationFrames(animationComp,entity,entityAnimation,70,70,8,56,100);
    entityAnimation = ComponentAnimation_CreateNewAnimation(animationComp,entity,TexturePool_GetTexture(game.texturePool,"character.png"),"walkUpLeft");
    ComponentAnimation_AddAnimationFrames(animationComp,entity,entityAnimation,70,70,8,64,100);

    //set the animation state to idle down
    ComponentAnimation_SetAnimationState(animationComp,entity,"idleDown");
    animationComp[entity].direction = ENTITY_WORLD_DIRECTION_DOWN;

//// END OF PLAYER ENTITY

/// TREES ON THE MAP!
    char msg[100];
    WriteDebug("Adding trees to scene...");
    for (i = 0; i < 1000; ++i) {
        //Add a tree entity to the scene
        entity = Scene_AddEntityToScene(testScene,
             COMPONENT_SET1_POSITION | COMPONENT_SET1_NAMETAG | COMPONENT_SET1_RENDER2D | COMPONENT_SET1_VELOCITY | COMPONENT_SET1_COLLISION);

        //every time we add a new entity to the scene, we have to check if the component pointers should be updated
        updateComponentPointers(testScene,0);

        //give the tree a name
        snprintf(msg, sizeof(msg), "tree %d",i);
        componentNameTagSetName(nameTag,entity,msg);

        //set the tree start position
        ComponentPosition_SetPosition(position,entity,rand()%MAP_WIDTH*32,rand()%MAP_HEIGHT*32);

        //ComponentVelocity_SetVelocity(velocity,entity,10+rand()%100,10+rand()%100);
        ComponentVelocity_SetFriction(velocity,entity,0);
        //move the tree offset y position up a bit
        ComponentPosition_SetOffset(position,entity,0,-96);

        //create the collision rectangle for the tree
        SetupRect(&tmpRect,0,0,20,20);
        ComponentCollision_SetCollisionRectangle(collision,entity,&tmpRect);

        //set the texture to render
        ComponentRender2D_SetTextureAndClipRect(render,entity,TexturePool_GetTexture(game.texturePool,"isotree.png"),NULL);
        ComponentRender2D_SetLayer(render,entity,1);
    }

    //Setup the isometric engine
    testScene->isoEngine = IsoEngine_New();
    if (testScene->isoEngine == NULL) {
        SceneManager_FreeSceneManager(game.sceneManager);
        closeDownSDL();
        exit(1);
    }
    //create an empty map
    testScene->isoEngine->isoMap = isoMapCreateNewMap("Testmap",MAP_WIDTH,MAP_HEIGHT,2,64,1232,20);
    if (testScene->isoEngine->isoMap == NULL) {
        SceneManager_FreeSceneManager(game.sceneManager);
        closeDownSDL();
        exit(1);
    }
    //load the isometric tile set from the texture pool
    isoMapLoadTileSet(testScene->isoEngine->isoMap,TexturePool_GetTexture(game.texturePool,"isotiles.png"),64,80);

    //set isometric game mode to focus at the selected entity
    IsoEngine_SetGameMode(testScene->isoEngine,GAME_MODE_OBJECT_FOCUS);
}


int main(int argc, char *argv[]) {
    //get rid of compiler warnings for argc and argv
    argv = argv;
    (void)argc;

    initSDL("Isometric Game Tutorial - Part 7 - By Johan Forsblom");
    init();

    SDL_ShowCursor(0);
    SDL_SetWindowGrab(getWindow(),SDL_TRUE);
    SDL_WarpMouseInWindow(getWindow(),WINDOW_WIDTH/2,WINDOW_HEIGHT/2);

    SceneManager_SetActiveScene(game.sceneManager, "testScene");
    SceneManager_RunActiveScene(game.sceneManager);
    SceneManager_FreeSceneManager(game.sceneManager);
    TexturePool_Free(game.texturePool);
    FontPool_Free(game.fontPool);
    closeDownSDL();
    return 0;
}
