
local STATE_PATH = "TrueThirdPersonCamera/state.json"

local PRESET_DEFAULT_EXTRAS = {
    ZoomToggleSelectorCloseValue = 2.9,
    ZoomToggleCombatFOV          = 40.0,
    ZoomToggleCastMinZoom        = 5.0,
    ZoomToggleCombatActionFOV    = 50.0,
    ZoomToggleCombatActionZoom   = 5.0,
    ZoomToggleFarValue           = 10.0,
    ZoomToggleFarFOV             = 55.0,
}

local CLOSE_PRESETS = {
    ["Relysia Default Preset"] = {
        ZoomToggleCloseValue            = 2.5,
        ZoomToggleCloseFOV              = 40.0,
        ZoomToggleRunningFOVIncrease    = 5.000,
        ZoomToggleCloseHorizontalOffset = 0.500,
        ZoomToggleCloseVerticalOffset   = 0.740,
        ZoomToggleCrouchVerticalOffset  = -0.20,
        ZoomToggleRunningZoomIncrease             = -0.100,
        ZoomToggleRunningHorizontalOffsetIncrease = 0.0,
        ZoomToggleRunningVerticalOffsetIncrease   = 0.0,
    },
    Hellblade = {
        ZoomToggleCloseValue            = 2.200,
        ZoomToggleCloseFOV              = 38.000,
        ZoomToggleRunningFOVIncrease    = 5.000,
        ZoomToggleCloseHorizontalOffset = 0.560,
        ZoomToggleCloseVerticalOffset   = 0.725,
        ZoomToggleCrouchVerticalOffset  = -0.180,
        ZoomToggleRunningZoomIncrease             = -0.250,
        ZoomToggleRunningHorizontalOffsetIncrease = 0.0,
        ZoomToggleRunningVerticalOffsetIncrease   = 0.0,
    },
    ["Witcher 3"] = {
        ZoomToggleCloseValue            = 2.400,
        ZoomToggleCloseFOV              = 48.000,
        ZoomToggleRunningFOVIncrease    = 4.000,
        ZoomToggleCloseHorizontalOffset = 0.800,
        ZoomToggleCloseVerticalOffset   = 0.690,
        ZoomToggleCrouchVerticalOffset  = -0.175,
        ZoomToggleRunningZoomIncrease             = 0.200,
        ZoomToggleRunningHorizontalOffsetIncrease = -0.100,
        ZoomToggleRunningVerticalOffsetIncrease   = 0.0,
    },
    ["Red Dead Redemption 2"] = {
        ZoomToggleCloseValue            = 2.200,
        ZoomToggleCloseFOV              = 44.000,
        ZoomToggleRunningFOVIncrease    = 5.000,
        ZoomToggleCloseHorizontalOffset = 0.620,
        ZoomToggleCloseVerticalOffset   = 0.785,
        ZoomToggleCrouchVerticalOffset  = -0.200,
        ZoomToggleRunningZoomIncrease             = 0.300,
        ZoomToggleRunningHorizontalOffsetIncrease = 0.0,
        ZoomToggleRunningVerticalOffsetIncrease   = -0.030,
    },
    ["Elden Ring"] = {
        ZoomToggleCloseValue            = 3.500,
        ZoomToggleCloseFOV              = 52.00,
        ZoomToggleRunningFOVIncrease    = 4.000,
        ZoomToggleCloseHorizontalOffset = 0.000,
        ZoomToggleCloseVerticalOffset   = 0.800,
        ZoomToggleCrouchVerticalOffset  = -0.170,
        ZoomToggleRunningZoomIncrease             = 0.500,
        ZoomToggleRunningHorizontalOffsetIncrease = 0.0,
        ZoomToggleRunningVerticalOffsetIncrease   = 0.0,
    },
    ["Batman: Arkham Knight"] = {
        ZoomToggleCloseValue            = 2.000,
        ZoomToggleCloseFOV              = 40.000,
        ZoomToggleRunningFOVIncrease    = 3.000,
        ZoomToggleCloseHorizontalOffset = 0.720,
        ZoomToggleCloseVerticalOffset   = 0.690,
        ZoomToggleCrouchVerticalOffset  = -0.160,
        ZoomToggleRunningZoomIncrease             = 3.000,
        ZoomToggleRunningHorizontalOffsetIncrease = -0.800,
        ZoomToggleRunningVerticalOffsetIncrease   = 0.080,
    },
    ["Avatar: Frontiers of Pandora"] = {
        ZoomToggleCloseValue            = 2.500,
        ZoomToggleCloseFOV              = 44.000,
        ZoomToggleRunningFOVIncrease    = 4.000,
        ZoomToggleCloseHorizontalOffset = 0.550,
        ZoomToggleCloseVerticalOffset   = 0.740,
        ZoomToggleCrouchVerticalOffset  = -0.170,
        ZoomToggleRunningZoomIncrease             = 0.300,
        ZoomToggleRunningHorizontalOffsetIncrease = -0.150,
        ZoomToggleRunningVerticalOffsetIncrease   = 0.000,
    },
    ["Split Screen"] = {
        ZoomToggleCloseValue            = 2.700,
        ZoomToggleCloseFOV              = 42.066,
        ZoomToggleCloseHorizontalOffset = 0.300,
        ZoomToggleCloseVerticalOffset   = 0.770,
        ZoomToggleRunningFOVIncrease    = 5.000,
        ZoomToggleCrouchVerticalOffset  = -0.20,
        ZoomToggleRunningZoomIncrease             = -0.100,
        ZoomToggleRunningHorizontalOffsetIncrease = 0.0,
        ZoomToggleRunningVerticalOffsetIncrease   = 0.0,
    },
}

for _, preset in pairs(CLOSE_PRESETS) do
    for settingId, value in pairs(PRESET_DEFAULT_EXTRAS) do
        if preset[settingId] == nil then
            preset[settingId] = value
        end
    end
end
local PRESETS_DIR = "TrueThirdPersonCamera/Presets/"

local PRESET_FIELD_MAP = {
    ["normal_zoom"]              = "ZoomToggleCloseValue",
    ["combat_zoom"]              = "ZoomToggleSelectorCloseValue",
    ["normal_field_of_view"]     = "ZoomToggleCloseFOV",
    ["running_field_of_view"]    = "ZoomToggleRunningFOVIncrease",
    ["horizontal_offset"]        = "ZoomToggleCloseHorizontalOffset",
    ["vertical_offset"]          = "ZoomToggleCloseVerticalOffset",
    ["crouch_vertical_offset"]   = "ZoomToggleCrouchVerticalOffset",
    ["running_zoom"]             = "ZoomToggleRunningZoomIncrease",
    ["running_horizontal_offset"] = "ZoomToggleRunningHorizontalOffsetIncrease",
    ["running_vertical_offset"]  = "ZoomToggleRunningVerticalOffsetIncrease",
    ["combat_field_of_view"]     = "ZoomToggleCombatFOV",
    ["cast_zoom"]                = "ZoomToggleCastMinZoom",
    ["selector_field_of_view"]   = "ZoomToggleCombatActionFOV",
    ["selector_zoom"]            = "ZoomToggleCombatActionZoom",
    ["far_zoom"]                 = "ZoomToggleFarValue",
    ["far_field_of_view"]        = "ZoomToggleFarFOV",
}

local function LoadCustomPresets()
    local manifestRaw = Ext.IO.LoadFile(PRESETS_DIR .. "_manifest.json")
    if not manifestRaw or manifestRaw == "" then
        return
    end

    local okManifest, names = pcall(Ext.Json.Parse, manifestRaw)
    if not okManifest or type(names) ~= "table" then
        Ext.Utils.PrintWarning("[TTPC] Couldn't parse Presets/_manifest.json, skipping custom presets")
        return
    end

    for _, name in ipairs(names) do
        local raw = Ext.IO.LoadFile(PRESETS_DIR .. name .. ".json")
        if raw and raw ~= "" then
            local okParse, data = pcall(Ext.Json.Parse, raw)
            if okParse and type(data) == "table" then
                local preset = {}
                for friendlyKey, settingId in pairs(PRESET_FIELD_MAP) do
                    if type(data[friendlyKey]) == "number" then
                        preset[settingId] = data[friendlyKey]
                    end
                end
                if next(preset) then
                    local displayName = (type(data["name"]) == "string" and data["name"] ~= "") and data["name"] or name
                    CLOSE_PRESETS[displayName] = preset
                    Ext.Utils.Print("[TTPC] Custom preset '" .. displayName .. "' (file '" .. name .. "') registered")
                else
                    Ext.Utils.PrintWarning("[TTPC] Custom preset '" .. name .. "' has none of the expected fields, ignoring it")
                end
            else
                Ext.Utils.PrintWarning("[TTPC] Custom preset '" .. name .. "' isn't valid JSON, ignoring it")
            end
        end
    end
end

local loadCustomPresetsOk, loadCustomPresetsErr = pcall(LoadCustomPresets)
if not loadCustomPresetsOk then
    Ext.Utils.PrintError("[TTPC] LoadCustomPresets crashed: " .. tostring(loadCustomPresetsErr))
end

local function ApplyPresetToBlueprintDefaults(preset)
    local ok, err = pcall(function()
        local blueprint = Mods.BG3MCM and Mods.BG3MCM.MCMAPI and Mods.BG3MCM.MCMAPI:GetModBlueprint(ModuleUUID)
        if not blueprint then
            return
        end
        local function applyToList(settings)
            if not settings then
                return
            end
            for _, setting in ipairs(settings) do
                local value = preset[setting.Id]
                if value ~= nil then
                    setting.Default = value
                end
            end
        end
        for _, tab in ipairs(blueprint.Tabs or {}) do
            applyToList(tab.Settings)
            for _, section in ipairs(tab.Sections or {}) do
                applyToList(section.Settings)
            end
        end
    end)
    if not ok then
        Ext.Utils.PrintWarning("[TTPC] ApplyPresetToBlueprintDefaults (server) failed: " .. tostring(err))
    end
end

local function ApplyClosePreset(presetName)
    local preset = CLOSE_PRESETS[presetName]
    if not preset then
        Ext.Utils.PrintWarning("[TTPC] Unknown close camera preset: " .. tostring(presetName))
        return
    end
    for settingId, value in pairs(preset) do
        MCM.Set(settingId, value, nil, true)
    end
    ApplyPresetToBlueprintDefaults(preset)
    local ok, encoded = pcall(Ext.Json.Stringify, preset)
    if ok and Ext.ServerNet and Ext.ServerNet.BroadcastMessage then
        Ext.ServerNet.BroadcastMessage("TTPC_PresetDefaultsUpdated", encoded)
    end
end

Ext.RegisterNetListener("TTPC_ApplyCustomPreset", function(_, payload)
    local ok, data = pcall(Ext.Json.Parse, payload)
    if not ok or type(data) ~= "table" or type(data.name) ~= "string" then
        Ext.Utils.PrintWarning("[TTPC] TTPC_ApplyCustomPreset: malformed payload")
        return
    end
    if not CLOSE_PRESETS[data.name] then
        Ext.Utils.PrintWarning("[TTPC] TTPC_ApplyCustomPreset: unknown preset '" .. data.name .. "'")
        return
    end
    ApplyClosePreset(data.name)
end)

local BEAST_RACE_GUID = "27a5799e-1f5c-4c2c-9adf-fe1c0276a64e"
local ELEMENTAL_RACE_GUID = "24dfab7b-7d7e-4df9-aa23-7212b4ef8980"

local UNSTABLE_BOUNDS_STATUS_IDS = {
    COL_RESONANCESTONE_BUFF = true, -- "Steeped In Bliss"
}

local WILDSHAPE_SIZE_PUSH = {
    WILDSHAPE_CAT_PLAYER = { zoom = 0.200, vert = 0.040 },
    WILDSHAPE_MYRMIDON_AIR_PLAYER = { zoom = 0.700, vert = 0.070 },
    WILDSHAPE_MYRMIDON_EARTH_PLAYER = { zoom = 0.700, vert = 0.070 },
    WILDSHAPE_MYRMIDON_FIRE_PLAYER = { zoom = 0.700, vert = 0.070 },
    WILDSHAPE_MYRMIDON_WATER_PLAYER = { zoom = 0.700, vert = 0.070 },
    WILDSHAPE_RAVEN_PLAYER = { zoom = 1.000, vert = 0.100 },
    WILDSHAPE_SABERTOOTH_TIGER_PLAYER = { zoom = 1.000, vert = 0.100 },
    WILDSHAPE_PANTHER_PLAYER = { zoom = 1.000, vert = 0.100 },
    WILDSHAPE_DILOPHOSAURUS_PLAYER = { zoom = 1.300, vert = 0.000 },
    WILDSHAPE_BADGER_PLAYER = { zoom = 1.500, vert = 0.000 },
    WILDSHAPE_WOLF_DIRE_PLAYER = { zoom = 2.000, vert = 0.000 },
    WILDSHAPE_BEAR_POLAR_PLAYER = { zoom = 2.000, vert = 0.000 },
    WILDSHAPE_DEEP_ROTHE_PLAYER = { zoom = 2.500, vert = 0.000 },
    WILDSHAPE_SPIDER_GIANT_PLAYER = { zoom = 3.200, vert = 0.000 },
    WILDSHAPE_OWLBEAR_PLAYER = { zoom = 4.000, vert = 0.400 },
}

local PLANAR_ALLY_OFFSET_PUSH = {
    Cambion_Female_PlanarAlly = { zoom = 2.000, vert = -0.180, horiz = -0.100, elevateOnFly = false },
    Cambion_PlanarAlly = { zoom = 2.000, vert = -0.180, horiz = -0.100, elevateOnFly = false },
    Deva_Female_PlanarAlly = { zoom = 2.000, vert = -0.180, horiz = -0.100, elevateOnFly = true },
    Deva_PlanarAlly = { zoom = 2.000, vert = -0.180, horiz = -0.100, elevateOnFly = true },
    Djinni_Female_PlanarAlly = { zoom = 0.000, vert = -0.220, horiz = 0.000, elevateOnFly = true },
    Djinni_PlanarAlly = { zoom = 0.300, vert = -0.240, horiz = 0.000, elevateOnFly = true },
}

local persistentState = {
    ZoomToggleFTBActive = false,
    IsControllingBeastForm = false,
    IsControllingElementalForm = false,
    HasUnstableBoundsStatus = false,
    BeastZoomPushOverride = -1,
    BeastVerticalPushOverride = -1,
    PlanarAllyActive = false,
    PlanarAllyZoomPush = 0,
    PlanarAllyVerticalPush = 0,
    PlanarAllyHorizontalPush = 0,
    PlanarAllyElevateOnFly = false,
}

local function WriteState()
    if not (Ext and Ext.IO and Ext.IO.SaveFile and Ext.Json and Ext.Json.Stringify) then
        Ext.Utils.PrintError("[TTPC Debug] Ext.IO.SaveFile/Ext.Json.Stringify is not available!")
        return
    end
    pcall(Ext.IO.SaveFile, STATE_PATH, Ext.Json.Stringify(persistentState))
end

local FTB_DETECTION_ENABLED = true
local ftbTagEntities = {}
local turnBasedTagEntities = {}
local clientControlEntities = {}
local currentFtbAggregateState = nil
local pendingClearGeneration = 0

local function EntityKey(entity)
    return tostring(entity)
end

local function ComputeRawInFtb()
    for entityKey in pairs(clientControlEntities) do
        if ftbTagEntities[entityKey] or turnBasedTagEntities[entityKey] then
            return true
        end
    end
    return false
end

local function ApplyFtbState(inFtb)
    if not FTB_DETECTION_ENABLED then
        return
    end
    if inFtb ~= currentFtbAggregateState then
        currentFtbAggregateState = inFtb
        persistentState.ZoomToggleFTBActive = inFtb
        WriteState()
    end
end

local function EntityHasUnstableBoundsStatus(entity)
    local ok, statuses = pcall(function() return entity.ServerCharacter.StatusManager.Statuses end)
    if not ok or not statuses then
        return false
    end
    for _, status in ipairs(statuses) do
        if UNSTABLE_BOUNDS_STATUS_IDS[status.StatusId] then
            return true
        end
    end
    return false
end

local function GetWildshapeSizePush(entity)
    local ok, statuses = pcall(function() return entity.ServerCharacter.StatusManager.Statuses end)
    if not ok or not statuses then
        return nil
    end
    for _, status in ipairs(statuses) do
        local key = status.StatusId:match("^(.-_PLAYER)")
        local push = key and WILDSHAPE_SIZE_PUSH[key]
        if push then
            return push
        end
    end
    return nil
end

local function GetPlanarAllyOffsetPush(entity)
    local ok, name = pcall(function() return entity.ServerCharacter.Template.Name end)
    if not ok or not name then
        return nil
    end
    return PLANAR_ALLY_OFFSET_PUSH[name]
end

local function ApplySpecialRaceState(entity)
    local ok, raceGuid = pcall(function() return entity.Race.Race end)
    local isBeast = ok and raceGuid == BEAST_RACE_GUID
    local isElemental = ok and raceGuid == ELEMENTAL_RACE_GUID
    local hasUnstableBounds = EntityHasUnstableBoundsStatus(entity)
    local wildshapePush = GetWildshapeSizePush(entity)
    local zoomOverride = wildshapePush and wildshapePush.zoom or -1
    local vertOverride = wildshapePush and wildshapePush.vert or -1
    local planarAllyPush = GetPlanarAllyOffsetPush(entity)
    local planarActive = planarAllyPush ~= nil
    local planarZoomPush = planarAllyPush and planarAllyPush.zoom or 0
    local planarVertPush = planarAllyPush and planarAllyPush.vert or 0
    local planarHorizPush = planarAllyPush and planarAllyPush.horiz or 0
    local planarElevateOnFly = planarActive and planarAllyPush.elevateOnFly or false

    local changed = false
    if isBeast ~= persistentState.IsControllingBeastForm then
        persistentState.IsControllingBeastForm = isBeast
        changed = true
    end
    if isElemental ~= persistentState.IsControllingElementalForm then
        persistentState.IsControllingElementalForm = isElemental
        changed = true
    end
    if hasUnstableBounds ~= persistentState.HasUnstableBoundsStatus then
        persistentState.HasUnstableBoundsStatus = hasUnstableBounds
        changed = true
    end
    if zoomOverride ~= persistentState.BeastZoomPushOverride then
        persistentState.BeastZoomPushOverride = zoomOverride
        changed = true
    end
    if vertOverride ~= persistentState.BeastVerticalPushOverride then
        persistentState.BeastVerticalPushOverride = vertOverride
        changed = true
    end
    if planarActive ~= persistentState.PlanarAllyActive then
        persistentState.PlanarAllyActive = planarActive
        changed = true
    end
    if planarZoomPush ~= persistentState.PlanarAllyZoomPush then
        persistentState.PlanarAllyZoomPush = planarZoomPush
        changed = true
    end
    if planarVertPush ~= persistentState.PlanarAllyVerticalPush then
        persistentState.PlanarAllyVerticalPush = planarVertPush
        changed = true
    end
    if planarHorizPush ~= persistentState.PlanarAllyHorizontalPush then
        persistentState.PlanarAllyHorizontalPush = planarHorizPush
        changed = true
    end
    if planarElevateOnFly ~= persistentState.PlanarAllyElevateOnFly then
        persistentState.PlanarAllyElevateOnFly = planarElevateOnFly
        changed = true
    end
    if changed then
        WriteState()
    end
end

local lastSpecialRacePollTime = 0
local SPECIAL_RACE_POLL_INTERVAL_MS = 500
if Ext.Events and Ext.Events.Tick then
    Ext.Events.Tick:Subscribe(function()
        local now = Ext.Utils.MonotonicTime()
        if now - lastSpecialRacePollTime < SPECIAL_RACE_POLL_INTERVAL_MS then
            return
        end
        lastSpecialRacePollTime = now

        if not (Ext.Entity and Ext.Entity.GetAllEntitiesWithComponent) then
            return
        end
        local controlled = Ext.Entity.GetAllEntitiesWithComponent("ClientControl")
        if not controlled or #controlled == 0 then
            return
        end
        local anyBeast, anyElemental, anyUnstableBounds = false, false, false
        local anyWildshapePush = nil
        local anyPlanarAllyPush = nil
        for _, entity in ipairs(controlled) do
            local ok, raceGuid = pcall(function() return entity.Race.Race end)
            if ok then
                if raceGuid == BEAST_RACE_GUID then
                    anyBeast = true
                elseif raceGuid == ELEMENTAL_RACE_GUID then
                    anyElemental = true
                end
            end
            if EntityHasUnstableBoundsStatus(entity) then
                anyUnstableBounds = true
            end
            if not anyWildshapePush then
                anyWildshapePush = GetWildshapeSizePush(entity)
            end
            if not anyPlanarAllyPush then
                anyPlanarAllyPush = GetPlanarAllyOffsetPush(entity)
            end
        end
        local zoomOverride = anyWildshapePush and anyWildshapePush.zoom or -1
        local vertOverride = anyWildshapePush and anyWildshapePush.vert or -1
        local planarActive = anyPlanarAllyPush ~= nil
        local planarZoomPush = anyPlanarAllyPush and anyPlanarAllyPush.zoom or 0
        local planarVertPush = anyPlanarAllyPush and anyPlanarAllyPush.vert or 0
        local planarHorizPush = anyPlanarAllyPush and anyPlanarAllyPush.horiz or 0
        local planarElevateOnFly = planarActive and anyPlanarAllyPush.elevateOnFly or false
        local changed = false
        if anyBeast ~= persistentState.IsControllingBeastForm then
            persistentState.IsControllingBeastForm = anyBeast
            changed = true
        end
        if anyElemental ~= persistentState.IsControllingElementalForm then
            persistentState.IsControllingElementalForm = anyElemental
            changed = true
        end
        if anyUnstableBounds ~= persistentState.HasUnstableBoundsStatus then
            persistentState.HasUnstableBoundsStatus = anyUnstableBounds
            changed = true
        end
        if zoomOverride ~= persistentState.BeastZoomPushOverride then
            persistentState.BeastZoomPushOverride = zoomOverride
            changed = true
        end
        if vertOverride ~= persistentState.BeastVerticalPushOverride then
            persistentState.BeastVerticalPushOverride = vertOverride
            changed = true
        end
        if planarActive ~= persistentState.PlanarAllyActive then
            persistentState.PlanarAllyActive = planarActive
            changed = true
        end
        if planarZoomPush ~= persistentState.PlanarAllyZoomPush then
            persistentState.PlanarAllyZoomPush = planarZoomPush
            changed = true
        end
        if planarVertPush ~= persistentState.PlanarAllyVerticalPush then
            persistentState.PlanarAllyVerticalPush = planarVertPush
            changed = true
        end
        if planarHorizPush ~= persistentState.PlanarAllyHorizontalPush then
            persistentState.PlanarAllyHorizontalPush = planarHorizPush
            changed = true
        end
        if planarElevateOnFly ~= persistentState.PlanarAllyElevateOnFly then
            persistentState.PlanarAllyElevateOnFly = planarElevateOnFly
            changed = true
        end
        if changed then
            WriteState()
        end
    end)
else
    Ext.Utils.PrintWarning("[TTPC] Ext.Events.Tick not available server-side - beast/elemental re-check on in-place transforms won't work, only on real party swaps")
end

local function RecalculateFtbAggregateState()
    local inFtb = ComputeRawInFtb()

    if inFtb then
        pendingClearGeneration = pendingClearGeneration + 1
        ApplyFtbState(true)
        return
    end

    pendingClearGeneration = pendingClearGeneration + 1
    local myGeneration = pendingClearGeneration
    Ext.Timer.WaitFor(700, function()
        if myGeneration ~= pendingClearGeneration then
            return
        end
        if not ComputeRawInFtb() then
            ApplyFtbState(false)
        end
    end)
end

local FTB_TABLE_RESYNC_INTERVAL_MS = 2000
local lastFtbResyncTime = 0
if FTB_DETECTION_ENABLED and Ext.Events and Ext.Events.Tick and Ext.Entity and Ext.Entity.GetAllEntitiesWithComponent then
    Ext.Events.Tick:Subscribe(function()
        local now = Ext.Utils.MonotonicTime()
        if now - lastFtbResyncTime < FTB_TABLE_RESYNC_INTERVAL_MS then
            return
        end
        lastFtbResyncTime = now

        local function RebuiltKeySet(componentName)
            local fresh = {}
            local entities = Ext.Entity.GetAllEntitiesWithComponent(componentName)
            if entities then
                for _, entity in ipairs(entities) do
                    fresh[EntityKey(entity)] = true
                end
            end
            return fresh
        end

        ftbTagEntities = RebuiltKeySet("IsInFTB")
        turnBasedTagEntities = RebuiltKeySet("IsInTurnBasedMode")
        clientControlEntities = RebuiltKeySet("ClientControl")

        local inFtb = ComputeRawInFtb()
        if inFtb ~= currentFtbAggregateState then
            ApplyFtbState(inFtb)
        end
    end)
end

if Ext.Entity and Ext.Entity.OnCreate and Ext.Entity.OnDestroy then
    Ext.Entity.OnCreate("IsInFTB", function(entity)
        ftbTagEntities[EntityKey(entity)] = true
        RecalculateFtbAggregateState()
    end)
    Ext.Entity.OnDestroy("IsInFTB", function(entity)
        ftbTagEntities[EntityKey(entity)] = nil
        RecalculateFtbAggregateState()
    end)

    Ext.Entity.OnCreate("IsInTurnBasedMode", function(entity)
        turnBasedTagEntities[EntityKey(entity)] = true
        RecalculateFtbAggregateState()
    end)
    Ext.Entity.OnDestroy("IsInTurnBasedMode", function(entity)
        turnBasedTagEntities[EntityKey(entity)] = nil
        RecalculateFtbAggregateState()
    end)

    Ext.Entity.OnCreate("ClientControl", function(entity)
        clientControlEntities[EntityKey(entity)] = true
        RecalculateFtbAggregateState()
        ApplySpecialRaceState(entity)
    end)
    Ext.Entity.OnDestroy("ClientControl", function(entity)
        clientControlEntities[EntityKey(entity)] = nil
        RecalculateFtbAggregateState()
    end)
else
    Ext.Utils.PrintError("[TTPC] Ext.Entity.OnCreate/OnDestroy not available on this bg3se version - server-side FTB detection DISABLED!")
end
