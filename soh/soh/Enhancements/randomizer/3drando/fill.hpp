#pragma once

#include "../randomizerTypes.h"
#include "location_access.hpp"
#include "../entrance.h"

#include <vector>
#include <string>

//RANDOTODO merge into Logic once Logic is a class passed to logic funtions 
struct GetAccessableLocationsStruct {
  std::vector<RandomizerCheck> accessibleLocations;
  std::vector<RandomizerRegion> regionPool;
  //Variables for playthrough
  int gsCount;
  int maxGsCount;
  std::vector<LogicVal> buyIgnores;

  //Variables for search
  std::vector<Rando::ItemLocation*> newItemLocations;
  bool logicUpdated;

  //Variables For Validating Entrences
  bool haveTimeAccess;
  bool foundTempleOfTime;
  bool validatedStartingRegion;
  bool sphereZeroComplete;
  bool timePassChildDay;
  bool timePassChildNight;
  bool timePassAdultDay;
  bool timePassAdultNight;

  std::vector<RandomizerCheck> itemSphere;
  std::list<Rando::Entrance*> entranceSphere;

  GetAccessableLocationsStruct(int _maxGsCount){
    regionPool = {RR_ROOT};
    gsCount = 0;
    maxGsCount = _maxGsCount;
    logicUpdated = false;
    logicUpdated = false;
  }

  void InitLoop(){
    logicUpdated = false;
    for (Rando::ItemLocation* location : newItemLocations) {
      location->ApplyPlacedItemEffect();
    }
    newItemLocations.clear();
    itemSphere.clear();
    entranceSphere.clear();
  }
};

void ClearProgress();
void VanillaFill();
int Fill();

std::vector<RandomizerCheck> GetEmptyLocations(std::vector<RandomizerCheck> allowedLocations);

void ProcessRegion(Region* region, GetAccessableLocationsStruct& gals, RandomizerGet ignore = RG_NONE, 
                   bool stopOnBeatable = false, bool addToPlaythrough = false);

std::vector<RandomizerCheck> ReachabilitySearch(const std::vector<RandomizerCheck>& allowedLocations, RandomizerGet ignore=RG_NONE);

void GeneratePlaythrough();

bool CheckBeatable(RandomizerGet ignore=RG_NONE);

void ValidateEntrances(bool checkPoeCollectorAccess, bool checkOtherEntranceAccess);