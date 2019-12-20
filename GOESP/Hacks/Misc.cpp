#include "Misc.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

#include "../Config.h"
#include "../Helpers.h"
#include "../Interfaces.h"
#include "../Memory.h"
#include "../SDK/ConVar.h"
#include "../SDK/Cvar.h"
#include "../SDK/Engine.h"
#include "../SDK/Entity.h"
#include "../SDK/EntityList.h"
#include "../SDK/GlobalVars.h"

void Misc::drawReloadProgress(ImDrawList* drawList) noexcept
{
    if (config.reloadProgress.enabled && interfaces->engine->isInGame()) {
        const auto localPlayer = interfaces->entityList->getEntity(interfaces->engine->getLocalPlayer());

        if (!localPlayer || !localPlayer->isAlive())
            return;

        static float reloadLength = 0.0f;

        if (const auto activeWeapon = localPlayer->getActiveWeapon(); activeWeapon && activeWeapon->isInReload()) {
            if (!reloadLength)
                reloadLength = activeWeapon->nextPrimaryAttack() - memory->globalVars->currenttime;

            const auto [width, height] = interfaces->engine->getScreenSize();
            constexpr int segments = 20;
            drawList->PathArcTo({ width / 2.0f, height / 2.0f }, 20.0f, -IM_PI / 2, std::clamp(IM_PI * 2 * (0.75f - (activeWeapon->nextPrimaryAttack() - memory->globalVars->currenttime) / reloadLength), -IM_PI / 2, -IM_PI / 2 + IM_PI * 2), segments);
            const ImU32 color = Helpers::calculateColor(config.reloadProgress.color, config.reloadProgress.rainbow, config.reloadProgress.rainbowSpeed, memory->globalVars->realtime);
            drawList->PathStroke(color, false, config.reloadProgress.thickness);
        } else {
            reloadLength = 0.0f;
        }
    }
}

void Misc::drawRecoilCrosshair(ImDrawList* drawList) noexcept
{
    if (config.recoilCrosshair.enabled && interfaces->engine->isInGame()) {
        const auto localPlayer = interfaces->entityList->getEntity(interfaces->engine->getLocalPlayer());

        if (!localPlayer || !localPlayer->isAlive())
            return;

        static auto weaponRecoilScale = interfaces->cvar->findVar("weapon_recoil_scale");

        const auto punchAngle = interfaces->entityList->getEntity(interfaces->engine->getLocalPlayer())->aimPunchAngle() * weaponRecoilScale->getFloat();
        const auto [width, height] = interfaces->engine->getScreenSize();

        const float x = width * (0.5f - punchAngle.y / 180.0f);
        const float y = height * (0.5f + punchAngle.x / 180.0f);
        const auto color = Helpers::calculateColor(config.recoilCrosshair.color, config.recoilCrosshair.rainbow, config.recoilCrosshair.rainbowSpeed, memory->globalVars->realtime);

        drawList->AddLine({ x, y - 10 }, { x, y + 10 }, color, config.recoilCrosshair.thickness);
        drawList->AddLine({ x - 10, y }, { x + 10, y }, color, config.recoilCrosshair.thickness);
    }
}
