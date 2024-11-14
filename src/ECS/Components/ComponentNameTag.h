#ifndef __COMPONENT_NAMETAG_H
#define __COMPONENT_NAMETAG_H

typedef struct ComponentNameTag {
    char *name;
} ComponentNameTag;

ComponentNameTag *ComponentNameTag_New();
void ComponentNameTag_AllocateMoreMemory(Scene *scene,int componentIndex);
void componentNameTagSetName(ComponentNameTag *nameTagComponent,Uint32 entity,char *name);
void ComponentNameTag_Free(ComponentNameTag *nameTagComponent,Uint32 numEntities);
int  ComponentNameTag_GetEntityIDFromEntityByName(ComponentNameTag *nameTagComponents,char *entityName,Uint32 numEntities);

#endif //__COMPONENT_NAMETAG_H