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
    GameInteractionEffectQueryResult Apply();
protected:
    virtual void _Apply() = 0;
};

class RemovableGameInteractionEffect: public GameInteractionEffectBase {
public:
    virtual GameInteractionEffectQueryResult CanBeRemoved();
    GameInteractionEffectQueryResult Remove();
protected:
    virtual void _Remove() {};
};

class ParameterizedGameInteractionEffect {
public:
    int32_t parameters[3];
};

namespace GameInteractionEffect {
    class SetSceneFlag: public GameInteractionEffectBase, public ParameterizedGameInteractionEffect {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
    };

    class UnsetSceneFlag: public GameInteractionEffectBase, public ParameterizedGameInteractionEffect {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
    };

    class SetFlag: public GameInteractionEffectBase, public ParameterizedGameInteractionEffect {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
    };

    class UnsetFlag: public GameInteractionEffectBase, public ParameterizedGameInteractionEffect {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
    };

    class ModifyHeartContainers: public GameInteractionEffectBase, public ParameterizedGameInteractionEffect {
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

    class ModifyRupees: public GameInteractionEffectBase, public ParameterizedGameInteractionEffect {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
    };

    class NoUI: public RemovableGameInteractionEffect {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
        void _Remove() override;
    };

    class ModifyGravity: public RemovableGameInteractionEffect, public ParameterizedGameInteractionEffect {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
        void _Remove() override;
    };

    class ModifyHealth: public GameInteractionEffectBase, public ParameterizedGameInteractionEffect {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
    };

    class SetPlayerHealth: public GameInteractionEffectBase, public ParameterizedGameInteractionEffect {
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

    class KnockbackPlayer: public GameInteractionEffectBase, public ParameterizedGameInteractionEffect {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
    };

    class ModifyLinkSize: public RemovableGameInteractionEffect, public ParameterizedGameInteractionEffect {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
        void _Remove() override;
    };

    class InvisibleLink : public RemovableGameInteractionEffect, public ParameterizedGameInteractionEffect {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
        void _Remove() override;
    };

    class PacifistMode : public RemovableGameInteractionEffect {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
        void _Remove() override;
    };

    class DisableZTargeting: public RemovableGameInteractionEffect, public ParameterizedGameInteractionEffect {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
        void _Remove() override;
    };

    class WeatherRainstorm: public RemovableGameInteractionEffect {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
        void _Remove() override;
    };

    class ReverseControls: public RemovableGameInteractionEffect {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
        void _Remove() override;
    };

    class ForceEquipBoots: public RemovableGameInteractionEffect, public ParameterizedGameInteractionEffect {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
        void _Remove() override;
    };

    class ModifyRunSpeedModifier: public RemovableGameInteractionEffect, public ParameterizedGameInteractionEffect {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
        void _Remove() override;
    };

    class OneHitKO : public RemovableGameInteractionEffect {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
        void _Remove() override;
    };

    class ModifyDefenseModifier: public RemovableGameInteractionEffect, public ParameterizedGameInteractionEffect {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
        void _Remove() override;
    };

    class GiveOrTakeShield: public GameInteractionEffectBase, public ParameterizedGameInteractionEffect {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
    };

    class TeleportPlayer: public GameInteractionEffectBase, public ParameterizedGameInteractionEffect {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
    };

    class ClearAssignedButtons: public GameInteractionEffectBase, public ParameterizedGameInteractionEffect {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
    };

    class SetTimeOfDay: public GameInteractionEffectBase, public ParameterizedGameInteractionEffect {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
    };

    class SetCollisionViewer: public RemovableGameInteractionEffect {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
        void _Remove() override;
    };

    class SetCosmeticsColor: public GameInteractionEffectBase, public ParameterizedGameInteractionEffect {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
    };

    class RandomizeCosmetics: public GameInteractionEffectBase {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
    };

    class PressButton: public GameInteractionEffectBase, public ParameterizedGameInteractionEffect {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
    };

    class PressRandomButton: public GameInteractionEffectBase, public ParameterizedGameInteractionEffect {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
    };

    class AddOrTakeAmmo: public GameInteractionEffectBase, public ParameterizedGameInteractionEffect {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
    };

    class RandomBombFuseTimer: public RemovableGameInteractionEffect {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
        void _Remove() override;
    };

    class DisableLedgeGrabs: public RemovableGameInteractionEffect {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
        void _Remove() override;
    };

    class RandomWind: public RemovableGameInteractionEffect {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
        void _Remove() override;
    };

    class RandomBonks: public RemovableGameInteractionEffect {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
        void _Remove() override;
    };

    class PlayerInvincibility: public RemovableGameInteractionEffect {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
        void _Remove() override;
    };

    class SlipperyFloor: public RemovableGameInteractionEffect {
        GameInteractionEffectQueryResult CanBeApplied() override;
        void _Apply() override;
        void _Remove() override;
    };
}

#endif /* __cplusplus */
#endif /* GameInteractionEffect_h */
