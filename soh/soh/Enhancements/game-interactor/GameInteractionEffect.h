#pragma once

#ifndef GameInteractionEffect_h
#define GameInteractionEffect_h

#include <stdint.h>

#ifdef __cplusplus
enum GameInteractionEffectQueryResult {
    Possible                = 0x00,
    TemporarilyNotPossible  = 0x01,
    NotPossible             = 0xFF
};

class GameInteractionEffectBase {
public:
    virtual GameInteractionEffectQueryResult CanBeApplied() = 0;
    virtual GameInteractionEffectQueryResult CanBeRemoved();
    GameInteractionEffectQueryResult Apply();
    GameInteractionEffectQueryResult Remove();
    int32_t parameters[3];

  protected:
    virtual void _Apply() = 0;
    virtual void _Remove() {};
};

namespace GameInteractionEffect {
    class SetSceneFlag: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
    };

    class UnsetSceneFlag: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
    };

    class SetFlag: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
    };

    class UnsetFlag: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
    };

    class ModifyHeartContainers: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
    };

    class FillMagic: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
    };

    class EmptyMagic: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
    };

    class ModifyRupees: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
    };

    class NoUI: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
        void _Remove() override;
    };

    class ModifyGravity: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
        void _Remove() override;
    };

    class ModifyHealth: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
    };

    class SetPlayerHealth: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
    };

    class FreezePlayer: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
    };

    class BurnPlayer: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
    };

    class ElectrocutePlayer: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
    };

    class KnockbackPlayer: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
    };

    class ModifyLinkSize: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
        void _Remove() override;
    };

    class InvisibleLink : public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
        void _Remove() override;
    };

    class PacifistMode : public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
        void _Remove() override;
    };

    class DisableZTargeting: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
        void _Remove() override;
    };

    class WeatherRainstorm: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
        void _Remove() override;
    };

    class ReverseControls: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
        void _Remove() override;
    };

    class ForceEquipBoots: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
        void _Remove() override;
    };

    class ModifyRunSpeedModifier: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
        void _Remove() override;
    };

    class OneHitKO : public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
        void _Remove() override;
    };

    class ModifyDefenseModifier: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
        void _Remove() override;
    };

    class GiveOrTakeShield: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
    };

    class TeleportPlayer: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
    };

    class ClearAssignedButtons: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
    };

    class SetTimeOfDay: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
    };

    class SetCollisionViewer: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
        void _Remove() override;
    };

    class SetCosmeticsColor: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
    };

    class RandomizeCosmetics: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
    };

    class PressButton: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
    };

    class PressRandomButton: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
    };

    class AddOrTakeAmmo: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
    };

    class RandomBombFuseTimer: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
        void _Remove() override;
    };

    class DisableLedgeGrabs: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
        void _Remove() override;
    };

    class RandomWind: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
        void _Remove() override;
    };

    class RandomBonks: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
        void _Remove() override;
    };

    class PlayerInvincibility: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
        void _Remove() override;
    };

    class SlipperyFloor: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
        void _Remove() override;
    };
}

#endif /* __cplusplus */
#endif /* GameInteractionEffect_h */
