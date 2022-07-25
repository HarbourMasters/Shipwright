#include "trial.hpp"

namespace Trial {

TrialInfo::TrialInfo(Text name_)
  : name(std::move(name_)) {}

  TrialInfo::~TrialInfo() = default;

  TrialInfo ForestTrial = TrialInfo(Text{"the Forest Trial", /*french*/"l'épreuve de la forêt",   /*spanish*/"la prueba del bosque"});
  TrialInfo FireTrial   = TrialInfo(Text{"the Fire Trial",   /*french*/"l'épreuve du feu",        /*spanish*/"la prueba del fuego"});
  TrialInfo WaterTrial  = TrialInfo(Text{"the Water Trial",  /*french*/"l'épreuve de l'eau",      /*spanish*/"la prueba del agua"});
  TrialInfo SpiritTrial = TrialInfo(Text{"the Spirit Trial", /*french*/"l'épreuve de l'esprit",   /*spanish*/"la prueba del espíritu"});
  TrialInfo ShadowTrial = TrialInfo(Text{"the Shadow Trial", /*french*/"l'épreuve de l'ombre",    /*spanish*/"la prueba de las sombras"});
  TrialInfo LightTrial  = TrialInfo(Text{"the Light Trial",  /*french*/"l'épreuve de la lumière", /*spanish*/"la prueba de la luz"});

  const TrialArray trialList = {
    &ForestTrial,
    &FireTrial,
    &WaterTrial,
    &SpiritTrial,
    &ShadowTrial,
    &LightTrial,
  };

} //namespace Trial
