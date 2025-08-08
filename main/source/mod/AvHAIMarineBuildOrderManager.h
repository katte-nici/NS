#pragma once

#ifndef AVH_AI_MARINE_BUILD_ORDER_H
#define AVH_AI_MARINE_BUILD_ORDER_H

#include "AvHAIConstants.h"
#include <string>
#include <vector>

extern bool CommanderHasAnnouncedBuildOrder;

typedef struct _BUILD_ORDER_ENTRY
{
	AvHAIDeployableStructureType StructureType;
	AvHAIDeployableStructureType StructureRequired;
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

#endif