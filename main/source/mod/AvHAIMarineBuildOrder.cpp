#include "AvHAIMarineBuildOrder.h"
#include <string>
#include <algorithm>

std::vector<Marine_build_order> AIBO_MarineBuildOrders;
Marine_build_order* AIBO_CurrentBuildOrder = nullptr;
bool CommanderHasAnnouncedBuildOrder = false;

bool bo_rng_initialized = false;

int AIBO_IntRandomRange(int MinValue, int MaxValue)
{
    if (MinValue == MaxValue) { return MinValue; }
    return MinValue + (rand() % (MaxValue - MinValue + 1));
}

float AIBO_FloatRandomRange(float MinValue, float MaxValue)
{
    if (MinValue == MaxValue) { return MinValue; }
    return ((float(rand()) / float(RAND_MAX)) * (MaxValue - MinValue)) + MinValue;
}

AvHAIDeployableStructureType AIBO_MapStringToStructure(const std::string& StructureName)
{
    if (!stricmp(StructureName.c_str(), "CommChair"))
        return STRUCTURE_MARINE_COMMCHAIR;
    if (!stricmp(StructureName.c_str(), "InfantryPortal"))
        return STRUCTURE_MARINE_INFANTRYPORTAL;
    if (!stricmp(StructureName.c_str(), "Armoury") || !stricmp(StructureName.c_str(), "Armory"))
        return STRUCTURE_MARINE_ARMOURY;
    if (!stricmp(StructureName.c_str(), "TurretFactory"))
        return STRUCTURE_MARINE_TURRETFACTORY;
    if (!stricmp(StructureName.c_str(), "Turrets"))
        return STRUCTURE_MARINE_TURRET;
    if (!stricmp(StructureName.c_str(), "Resourcetower") || !stricmp(StructureName.c_str(), "Restower") || !stricmp(StructureName.c_str(), "Resourcetowers") || !stricmp(StructureName.c_str(), "Restowers"))
        return STRUCTURE_MARINE_RESTOWER;
    if (!stricmp(StructureName.c_str(), "Armslab"))
        return STRUCTURE_MARINE_ARMSLAB;
    if (!stricmp(StructureName.c_str(), "Observatory"))
        return STRUCTURE_MARINE_OBSERVATORY;
    if (!stricmp(StructureName.c_str(), "Phasegate"))
        return STRUCTURE_MARINE_PHASEGATE;
    if (!stricmp(StructureName.c_str(), "AdvancedArmoury") || !stricmp(StructureName.c_str(), "AdvancedArmory"))
        return STRUCTURE_MARINE_ADVARMOURY;
    if (!stricmp(StructureName.c_str(), "ProtoLab"))
        return STRUCTURE_MARINE_PROTOTYPELAB;

    return STRUCTURE_NONE;
}

static void AIBO_LoadHardCodedMarineBuildOrder()
{
    Marine_build_order defaultOrder;
    defaultOrder.BuildOrder = {
        {STRUCTURE_MARINE_INFANTRYPORTAL, STRUCTURE_MARINE_COMMCHAIR},
        {STRUCTURE_MARINE_ARMOURY, STRUCTURE_MARINE_COMMCHAIR},
        {STRUCTURE_MARINE_TURRETFACTORY, STRUCTURE_MARINE_COMMCHAIR},
        {STRUCTURE_MARINE_TURRET, STRUCTURE_MARINE_COMMCHAIR},
        {STRUCTURE_MARINE_RESTOWER, STRUCTURE_MARINE_COMMCHAIR},
        {STRUCTURE_MARINE_ARMSLAB, STRUCTURE_MARINE_COMMCHAIR},
        {STRUCTURE_MARINE_OBSERVATORY, STRUCTURE_MARINE_COMMCHAIR},
        {STRUCTURE_MARINE_PHASEGATE, STRUCTURE_MARINE_COMMCHAIR},
        {STRUCTURE_MARINE_ADVARMOURY, STRUCTURE_MARINE_COMMCHAIR},
        {STRUCTURE_MARINE_PROTOTYPELAB, STRUCTURE_MARINE_COMMCHAIR},
    };
    defaultOrder.BuildOrderName = "Default Hard-Coded Build-Order";
    defaultOrder.Weight = 1.0f;
    AIBO_MarineBuildOrders.push_back(defaultOrder);
	AIBO_CurrentBuildOrder = &AIBO_MarineBuildOrders[0]; // Set the current build order to the default one
}

void AIBO_ParseMarineBuildOrder()
{
    CommanderHasAnnouncedBuildOrder = false;
    AIBO_MarineBuildOrders.clear();

    std::string BuildOrderFileString = std::string(getModDirectory()) + "/marine_build_orders.txt";
    std::ifstream BuildOrderFile(BuildOrderFileString.c_str());

    if (!BuildOrderFile.is_open())
    {
		g_engfuncs.pfnServerPrint("Failed to open marine build order file. Using default.\n");

		// Create a default build order
		AIBO_LoadHardCodedMarineBuildOrder();
        return;
    }

    std::string Line;
    Marine_build_order currentOrder;
    bool inBuildOrder = false;

    while (std::getline(BuildOrderFile, Line))
    {
        // Remove whitespace from both ends
        Line.erase(Line.begin(), std::find_if(Line.begin(), Line.end(), [](int ch) { return !std::isspace(ch); }));
        Line.erase(std::find_if(Line.rbegin(), Line.rend(), [](int ch) { return !std::isspace(ch); }).base(), Line.end());

        if (Line.empty() || Line[0] == '#')
            continue;

        if (Line == "BuildOrderStart")
        {
            inBuildOrder = true;
            currentOrder = Marine_build_order(); // reset
            continue;
        }
        if (Line == "BuildOrderEnd")
        {
            if (inBuildOrder)
            {
                AIBO_MarineBuildOrders.push_back(currentOrder);
                inBuildOrder = false;
            }
            continue;
        }
        if (!inBuildOrder)
            continue;

        if (Line.find("name=") == 0)
        {
            currentOrder.BuildOrderName = Line.substr(5);
        }
        else if (Line.find("weight=") == 0)
        {
            currentOrder.Weight = std::stof(Line.substr(7));
        }
        else if (Line.find("entry=") == 0)
        {
            std::string entryName = Line.substr(6);
            AvHAIDeployableStructureType structure = AIBO_MapStringToStructure(entryName);
            Build_order_entry entry = { structure, STRUCTURE_NONE };
            currentOrder.BuildOrder.push_back(entry);
        }
    }

	BuildOrderFile.close();

}


void AIBO_ResetMarineBuildOrder() {
	AIBO_CurrentBuildOrder = nullptr; // Reset current build order
    CommanderHasAnnouncedBuildOrder = false;
}

void AIBO_SelectBuildOrderRandomly() {
    if (!bo_rng_initialized)
    {
		unsigned int seed = static_cast<unsigned int>(time(nullptr));
        srand(seed);
        bo_rng_initialized = true;
    }

    if (AIBO_MarineBuildOrders.empty()) {
        AIBO_LoadHardCodedMarineBuildOrder();
    }

    std::vector<float> cumulativeWeights;
    cumulativeWeights.reserve(AIBO_MarineBuildOrders.size());
    float sumWeights = 0.0f;

    for (auto& order : AIBO_MarineBuildOrders)
    {
        sumWeights += order.Weight;
        cumulativeWeights.push_back(sumWeights);
    }

    float randomValue = AIBO_FloatRandomRange(0.0f, sumWeights);
    auto it = std::lower_bound(cumulativeWeights.begin(), cumulativeWeights.end(), randomValue);

    if (it != cumulativeWeights.end()) {
        size_t index = std::distance(cumulativeWeights.begin(), it);
        AIBO_CurrentBuildOrder = &AIBO_MarineBuildOrders[index];
    }
    else {
        AIBO_CurrentBuildOrder = &AIBO_MarineBuildOrders.back();
    }

    // AIBO_CurrentBuildOrder = &AIBO_MarineBuildOrders[AIBO_IntRandomRange(0, AIBO_MarineBuildOrders.size() - 1)];
    CommanderHasAnnouncedBuildOrder = false;

    std::string boMessage = "Selected build order: " + AIBO_CurrentBuildOrder->BuildOrderName + "\n";
    char LoadedMsg[128];
    sprintf(LoadedMsg, "%s", boMessage.c_str());
    g_engfuncs.pfnServerPrint(LoadedMsg);

}
