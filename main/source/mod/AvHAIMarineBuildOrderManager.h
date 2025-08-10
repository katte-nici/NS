#pragma once

#ifndef AVH_AI_MARINE_BUILD_ORDER_H
#define AVH_AI_MARINE_BUILD_ORDER_H

#include "AvHAIConstants.h"
#include <string>
#include <vector>

extern bool CommanderHasAnnouncedBuildOrder;

typedef enum
{
	BUILD_ORDER_NONE,
	BUILD_ORDER_STRUCTURE,
	BUILD_ORDER_UPGRADE,
} AvHAIBuildOrderType;

typedef enum
{
	CONDITION_NONE,
	STRUCTURE_EXISTS,
	UPGRADE_EXISTS,
	TIME_ELAPSED,
} AvHAIBuildCondition;

typedef enum
{
	OR,
	AND,
} AvHAILogicConnective;

typedef struct _BUILD_ORDER_ENTRY
{
	AvHAIBuildOrderType          BuildOrderType;
	AvHAIDeployableStructureType StructureToBuild;
	AvHTechID                    UpgradeToResearch;
	AvHAIBuildCondition          BuildConditionOne;
	AvHAIDeployableStructureType StructureRequiredOne;
	AvHTechID                    UpgradeRequiredOne;
	AvHAILogicConnective         Connective;
	AvHAIBuildCondition          BuildConditionTwo;
	AvHAIDeployableStructureType StructureRequiredTwo;
	AvHTechID                    UpgradeRequiredTwo;
	int TimeLimitInSecondsOne = 0;
	int TimeLimitInSecondsTwo = 0;
} Build_order_entry;

typedef struct _MARINE_BUILD_ORDER {
	std::vector<Build_order_entry> BuildOrder = {};
    std::string BuildOrderName = "";
	float Weight = 1.0f; // Weight for the build order, used for selection in case of multiple orders
} Marine_build_order;

extern std::vector<Marine_build_order> AIBO_MarineBuildOrders;
extern Marine_build_order* AIBO_CurrentBuildOrder;

static void AIBO_LoadHardCodedMarineBuildOrder();
void AIBO_ParseMarineBuildOrder();
void AIBO_ResetMarineBuildOrder();
void AIBO_SelectBuildOrderRandomly();
int AIBO_IntRandomRange(int MinValue, int MaxValue);
float AIBO_FloatRandomRange(float MinValue, float MaxValue);
AvHAIDeployableStructureType AIBO_MapStringToStructure(const std::string& StructureName);
AvHAIBuildCondition AIBO_MapStringToBuildCondition(const std::string& ConditionName);
AvHAIBuildOrderType AIBO_MapStringToBuildOrderType(const std::string& BuildOrderTypeName);
AvHTechID AIBO_MapStringToTech(const std::string& TechName);

#endif