#ifndef OBJ_ARMADILOID_H
#define OBJ_ARMADILOID_H

#include "SonicMania.h"

// Object Class
typedef struct {
    RSDK_OBJECT
    uint16 aniFrames;
    uint16 sfxShot;
} ObjectArmadiloid;

// Entity Class
typedef struct {
    RSDK_ENTITY
    StateMachine(state);
    int32 type;
    int32 timer;
    Entity *child;
    Entity *parent;
    Vector2 startPos;
    Hitbox hitbox;
    Animator animator1;
    Animator animator2;
    Animator animator3;
} EntityArmadiloid;

// Object Struct
extern ObjectArmadiloid *Armadiloid;

// Standard Entity Events
void Armadiloid_Update(void);
void Armadiloid_LateUpdate(void);
void Armadiloid_StaticUpdate(void);
void Armadiloid_Draw(void);
void Armadiloid_Create(void* data);
void Armadiloid_StageLoad(void);
void Armadiloid_EditorDraw(void);
void Armadiloid_EditorLoad(void);
void Armadiloid_Serialize(void);

// Extra Entity Functions
void Armadiloid_DebugSpawn(void);
void Armadiloid_DebugDraw(void);

void Armadiloid_State_Setup(void);

void Armadiloid_State_PlatformFlying(void);
void Armadiloid_PlatformShootDelay(void);
void Armadiloid_State_PlatformShoot(void);

void Armadiloid_State_Rider(void);

#endif //!OBJ_ARMADILOID_H
