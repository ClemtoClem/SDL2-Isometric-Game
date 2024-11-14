#ifndef __CONTROL_ENTITY_SYSTEM_H_
#define __CONTROL_ENTITY_SYSTEM_H_

int SystemControlEntity_Init(void *scene);
void SystemControlEntity_Compute();
void SystemControlEntity_SetEntityToControlByID(Scene *scene,Uint32 entityID);
void SystemControlEntity_SetEntityToControlByNameTag(Scene *scene,char *nameTag);
Uint32 SystemControlEntity_GetControlledEntity();
void SystemControlEntity_Free();
#endif // __CONTROL_ISOMETRIC_SYSTEM_H_

