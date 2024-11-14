#ifndef __ANIMATION_SYSTEM_H
#define __ANIMATION_SYSTEM_H

int SystemAnimation_Init(void *scene);
void SystemAnimation_Update();
void SystemAnimation_UpdateEntity(Uint32 entity);
void SystemAnimation_Free();

#endif //__ANIMATION_SYSTEM_H_
