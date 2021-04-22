#include "../SonicMania.h"

ObjectGHZCutsceneST *GHZCutsceneST;

void GHZCutsceneST_Update(void)
{
    void *states[5] = { GHZCutsceneST_CutsceneState_Unknown1, GHZCutsceneST_CutsceneState_Unknown2, GHZCutsceneST_CutsceneState_Unknown3,
                        GHZCutsceneST_CutsceneState_LoadNextStage, NULL };

    RSDK_THIS(GHZCutsceneST);
    if (!entity->timer) {
        foreach_active(Player, player)
        {
            if (Player_CheckCollisionTouch(player, entity, &entity->hitbox) && !player->sidekick) {
                CutsceneSeq_StartSequence((Entity *)entity, states);
                if (RSDK_GET_ENTITY(SLOT_CUTSCENESEQ, CutsceneSeq)->objectID) {
                    EntityCutsceneSeq *cutsceneSeq = (EntityCutsceneSeq *)RSDK.GetEntityByID(SLOT_CUTSCENESEQ);
                    cutsceneSeq->skipState         = 3;
                    cutsceneSeq->skipCallback      = GHZCutsceneST_SkipCB;
                }
                entity->timer = 1;
            }
        }
    }
}

void GHZCutsceneST_LateUpdate(void) {}

void GHZCutsceneST_StaticUpdate(void) {}

void GHZCutsceneST_Draw(void) {}

void GHZCutsceneST_Create(void *data)
{
    RSDK_THIS(GHZCutsceneST);
    if (!RSDK_sceneInfo->inEditor) {
        entity->active        = ACTIVE_BOUNDS;
        entity->visible       = 0;
        entity->updateRange.x = 0x800000;
        entity->updateRange.y = 0x800000;
        if (!entity->size.x)
            entity->size.x = 0x1A80000;
        if (!entity->size.y)
            entity->size.y = 0xF00000;
        entity->updateRange.x       = 0x800000 + entity->size.x;
        entity->updateRange.y       = 0x800000 + entity->size.y;
        RSDK_sceneInfo->timeEnabled = false;
        entity->hitbox.left         = -entity->size.x >> 17;
        entity->hitbox.right        = entity->size.x >> 17;
        entity->hitbox.top          = -entity->size.y >> 17;
        entity->hitbox.bottom       = entity->size.y >> 17;
        GHZCutsceneST_SetupCutscene();
    }
}

void GHZCutsceneST_StageLoad(void) {}

void GHZCutsceneST_SetupCutscene(void)
{
    foreach_all(Platform, platform)
    {
        if (!platform->frameID) {
            platform->drawOrder     = Zone->drawOrderHigh - 1;
            GHZCutsceneST->platform = (Entity *)platform;
            foreach_break;
        }
    }

    foreach_all(AIZKingClaw, claw)
    {
        GHZCutsceneST->claw = (Entity *)claw;
        foreach_break;
    }

    foreach_all(PhantomRuby, phantomRuby)
    {
        GHZCutsceneST->phantomRuby = (Entity *)phantomRuby;
        foreach_break;
    }

    foreach_all(FXRuby, fxRuby)
    {
        GHZCutsceneST->fxRuby  = (Entity *)fxRuby;
        fxRuby->state          = 0;
        fxRuby->timer          = 512;
        fxRuby->waitForTrigger = 512;
        fxRuby->outerRadius    = RSDK_screens->width;
        fxRuby->fadeWhite      = 64;
        foreach_break;
    }

    foreach_all(CutsceneHBH, cutsceneHBH) { GHZCutsceneST->cutsceneHBH[cutsceneHBH->characterID] = (Entity *)cutsceneHBH; }
}

bool32 GHZCutsceneST_CutsceneState_Unknown1(EntityGHZCutsceneST *host)
{
    EntityPlayer *player1 = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
    EntityPlayer *player2 = RSDK_GET_ENTITY(SLOT_PLAYER2, Player);
    RSDK_GET_ENTITY(SLOT_CAMERA1, Camera);
    EntityFXRuby *fxRuby    = (EntityFXRuby *)GHZCutsceneST->fxRuby;
    EntityPhantomRuby *ruby = (EntityPhantomRuby *)GHZCutsceneST->phantomRuby;
    if (host->timer) {
        if (host->timer >= 60) {
            if (fxRuby->timer <= 0) {
                if (fxRuby->waitForTrigger <= 0) {
                    PhantomRuby_PlaySFX(4);
                    return true;
                }
                fxRuby->waitForTrigger -= 16;
            }
            else {
                fxRuby->timer -= 16;
            }
        }
    }
    else {
        player1->position.y = ruby->position.y;
        player1->state      = Player_State_None;
        player1->inputState = 0;
        CutsceneSeq_LockAllPlayerControl();
        if (player2->objectID == Player->objectID) {
            player2->position.y = ruby->position.y;
            player2->state      = Player_State_None;
            player2->inputState = StateMachine_None;
        }
    }

    int id = 0;
    for (int angle = 0; angle < 0x80; angle += 0x40) {
        EntityPlayer *player = RSDK_GET_ENTITY(id++, Player);
        if (!player || player->objectID == TYPE_BLANK)
            break;
        RSDK.SetSpriteAnimation(player->spriteIndex, ANI_FAN, &player->playerAnimData, 0, 0);
        player->position.x += (player->position.x - player->position.x) >> 3;
        player->position.y += (0xA00 * RSDK.Sin256(2 * (host->timer + angle - host->timer2)) + ruby->position.y - player->position.y) >> 3;
        player->state = Player_State_None;
    }
    return false;
}
bool32 GHZCutsceneST_CutsceneState_Unknown2(EntityGHZCutsceneST *host)
{
    EntityPlayer *player1   = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
    EntityPlayer *player2   = RSDK_GET_ENTITY(SLOT_PLAYER2, Player);
    EntityCamera *camera    = RSDK_GET_ENTITY(SLOT_CAMERA1, Camera);
    EntityPhantomRuby *ruby = (EntityPhantomRuby *)GHZCutsceneST->phantomRuby;
    EntityFXRuby *fxRuby    = (EntityFXRuby *)GHZCutsceneST->fxRuby;
    if (!host->timer)
        fxRuby->state = FXRuby_Unknown4;

    EntityPlayer **curPlayer = &player1;
    if (fxRuby->outerRadius <= 0) {
        int id = 0;
        for (int angle = 0; angle < 0x80; angle += 0x10) {
            EntityPlayer *player = RSDK_GET_ENTITY(id++, Player);
            if (!player || player->objectID == TYPE_BLANK)
                break;
            RSDK.SetSpriteAnimation(player->spriteIndex, ANI_FAN, &player->playerAnimData, false, 0);
            int x              = (player->position.x - player->position.x) >> 3;
            int y              = (0xA00 * RSDK.Sin256(2 * (angle + host->timer - host->timer2)) + ruby->position.y - player->position.y) >> 3;
            player->velocity.y = (y >> 8) * (y >> 8);
            player->velocity.x = (x >> 8) * (x >> 8);
            player->state      = Player_State_Air;
            player->up         = true;
            player->camera     = NULL;
            ++curPlayer;
        }
        host->field_6C = 1;
        Camera_Unknown3(0, 0, camera->position.x, camera->position.y, 0);
        return true;
    }
    else {
        int id = 0;
        for (int angle = 0; angle < 0x80; angle += 0x10) {
            EntityPlayer *player = RSDK_GET_ENTITY(id++, Player);
            if (!player || player->objectID == TYPE_BLANK)
                break;
            RSDK.SetSpriteAnimation(player->spriteIndex, ANI_FAN, &player->playerAnimData, 0, 0);
            player->position.x += (player->position.x - player->position.x) >> 3;
            player->position.y += (0xA00 * RSDK.Sin256(2 * (host->timer + angle - host->timer2)) + ruby->position.y - player->position.y) >> 3;
            player->state = Player_State_None;
            ++curPlayer;
        }
        return false;
    }
    return false;
}
bool32 GHZCutsceneST_CutsceneState_Unknown3(EntityGHZCutsceneST *host)
{
    RSDK_GET_PLAYER(player1, player2, camera);
    unused(player2);
    EntityPhantomRuby *ruby  = (EntityPhantomRuby *)GHZCutsceneST->phantomRuby;
    EntityAIZKingClaw *claw  = (EntityAIZKingClaw *)GHZCutsceneST->claw;
    EntityPlatform *platform = (EntityPlatform *)GHZCutsceneST->platform;
    if (host->timer >= 60) {
        if (host->timer == 60) {
            int id = 0;
            for (int i = 0; i < 2; ++i) {
                EntityPlayer *player = RSDK_GET_ENTITY(id++, Player);
                if (!player || player->objectID == TYPE_BLANK)
                    break;
                player->up = false;
            }
            Music_FadeOut(0.025);
            return false;
        }
        
        if (host->timer == 180) {
            player1->camera = camera;
            Camera_SetTargetEntity(0, player1);
            return true;
        }
        else {
            claw->velocity.y -= 0x1800;
            byte hbhChar = 0;
            for (int hbhChar = 0; hbhChar < 5; ++hbhChar) {
                EntityCutsceneHBH *hbh = (EntityCutsceneHBH *)GHZCutsceneST->cutsceneHBH[hbhChar];
                switch (hbhChar) {
                    case HBH_GUNNER:
                        hbh->velocity.x += 0x800;
                        hbh->velocity.y -= 0x1800;
                        hbh->position.x += hbh->velocity.x;
                        hbh->position.y += hbh->velocity.y;
                        break;
                    case HBH_SHINOBI:
                        if (host->timer == 60) {
                            RSDK.SetSpriteAnimation(hbh->spriteIndex, 3, &hbh->data, true, 0);
                            RSDK.SetSpriteAnimation(0xFFFF, 0, &hbh->altData, true, 0);
                        }
                        hbh->position.x -= 0x4000;
                        hbh->position.y -= 0x40000;
                        break;
                    case HBH_MYSTIC:
                        hbh->velocity.x += 0x2000;
                        hbh->velocity.y -= 0x1000;
                        hbh->position.x += hbh->velocity.x;
                        hbh->position.y += hbh->velocity.y;
                        break;
                    case HBH_RIDER:
                    case HBH_KING:
                        hbh->position.x += claw->velocity.x;
                        hbh->position.y += claw->velocity.y;
                        break;
                    default: break;
                }
            }

            ruby->position.x += claw->velocity.x;
            ruby->position.y += claw->velocity.y;
            platform->position.x += claw->velocity.x;
            platform->position.y += claw->velocity.y;
            platform->drawPos.x   = platform->position.x;
            platform->drawPos.y   = platform->position.y;
            platform->centerPos.x = platform->position.x;
            platform->centerPos.y = platform->position.y;
            claw->position.x += claw->velocity.x;
            claw->position.y += claw->velocity.y;
            return false;
        }
    }
    return false;
}

bool32 GHZCutsceneST_CutsceneState_LoadNextStage(EntityGHZCutsceneST *host)
{
    RSDK_THIS(GHZCutsceneST);
#if RETRO_USE_PLUS
    if (globals->gameMode == MODE_ENCORE)
        RSDK.LoadScene("Encore Mode", "");
    else
#endif
        RSDK.LoadScene("Mania Mode", "");
    globals->parallaxOffset[0] = entity->timer2;
    EntityPlayer *player       = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);
    player->onGround           = true;
    player->state              = Player_State_Ground;
    Zone_StoreEntities((RSDK_screens->position.x + RSDK_screens->centerX) << 16, (RSDK_screens->height + RSDK_screens->position.y) << 16);
    RSDK.InitSceneLoad();
    return true;
}

void GHZCutsceneST_SkipCB(void)
{
#if RETRO_USE_PLUS
    if (globals->gameMode == MODE_ENCORE)
        RSDK.LoadScene("Encore Mode", "Green Hill Zone+ 1");
    else
#endif
        RSDK.LoadScene("Mania Mode", "Green Hill Zone 1");
}

void GHZCutsceneST_EditorDraw(void) {}

void GHZCutsceneST_EditorLoad(void) {}

void GHZCutsceneST_Serialize(void) { RSDK_EDITABLE_VAR(GHZCutsceneST, VAR_VECTOR2, size); }
