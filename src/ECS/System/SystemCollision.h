#ifndef __COLLISION_SYSTEM_H_
#define __COLLISION_SYSTEM_H_

int SystemCollision_Init(void *scene);
void SystemCollision_Update();
void SystemCollision_UpdateEntity(Uint32 entity);
void SystemCollision_Free();
int SystemCollision_BoundingBoxCollision(SDL_Rect a, SDL_Rect b);

#endif // __COLLISION_SYSTEM_H_
