
if MCM and MCM.Keybinding and MCM.Keybinding.SetCallback then
    MCM.Keybinding.SetCallback("ZoomToggleKeyboardKey", function() end)
else
    Ext.Utils.PrintWarning("[TTPC] MCM.Keybinding.SetCallback API not found on client!")
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
                if preset[setting.Id] ~= nil then
                    local actual = Mods.BG3MCM.MCMAPI:GetSettingValue(setting.Id, ModuleUUID)
                    local value = (actual ~= nil) and actual or preset[setting.Id]
                    setting.Default = value

                    if Mods.BG3MCM.IMGUIAPI and Mods.BG3MCM.IMGUIAPI.findWidgetForSetting then
                        local widget = Mods.BG3MCM.IMGUIAPI:findWidgetForSetting(setting.Id, ModuleUUID)
                        if widget then
                            widget._defaultValue = value
                            if widget.UpdateResetButtonVisibility then
                                widget:UpdateResetButtonVisibility()
                            end
                        end
                    end
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
        Ext.Utils.PrintWarning("[TTPC] ApplyPresetToBlueprintDefaults (client) failed: " .. tostring(err))
    end
end

Ext.RegisterNetListener("TTPC_PresetDefaultsUpdated", function(_, payload)
    local ok, preset = pcall(Ext.Json.Parse, payload)
    if not ok or type(preset) ~= "table" then
        return
    end
    Ext.Timer.WaitForRealtime(100, function()
        ApplyPresetToBlueprintDefaults(preset)
    end)
end)

local PRESETS_DIR = "TrueThirdPersonCamera/Presets/"

local BUILTIN_PRESET_NAMES = { "Relysia Default Preset", "Hellblade", "Witcher 3", "Red Dead Redemption 2", "Elden Ring", "Batman: Arkham Knight", "Avatar: Frontiers of Pandora", "Split Screen" }

local function LoadCustomPresetNames()
    local manifestRaw = Ext.IO.LoadFile(PRESETS_DIR .. "_manifest.json")
    if not manifestRaw or manifestRaw == "" then
        return {}
    end
    local okManifest, names = pcall(Ext.Json.Parse, manifestRaw)
    if not okManifest or type(names) ~= "table" then
        return {}
    end

    local displayNames = {}
    for _, name in ipairs(names) do
        local raw = Ext.IO.LoadFile(PRESETS_DIR .. name .. ".json")
        if raw and raw ~= "" then
            local okParse, data = pcall(Ext.Json.Parse, raw)
            if okParse and type(data) == "table" then
                local displayName = (type(data["name"]) == "string" and data["name"] ~= "") and data["name"] or name
                table.insert(displayNames, displayName)
            end
        end
    end
    return displayNames
end

local function ApplyPresetByName(displayName)
    local ok, encoded = pcall(Ext.Json.Stringify, { name = displayName })
    if ok then
        Ext.Net.PostMessageToServer("TTPC_ApplyCustomPreset", encoded)
    end
end

local kPresetButtonSize = { 380, 0 }

local function BuildPresetPickerWindow()
    local window = Ext.IMGUI.NewWindow("True Third-Person Camera - Presets")
    window.Visible = false
    window.Closeable = true
    window.AlwaysAutoResize = true
    window:AddText("Click a preset to apply it immediately.")
    window:AddSeparator()
    return window
end

local presetButtons = {}

local function RefreshPresetButtons(window)
    for _, button in ipairs(presetButtons) do
        button:Destroy()
    end
    presetButtons = {}

    local allNames = {}
    for _, n in ipairs(BUILTIN_PRESET_NAMES) do
        table.insert(allNames, n)
    end
    for _, n in ipairs(LoadCustomPresetNames()) do
        table.insert(allNames, n)
    end

    for _, displayName in ipairs(allNames) do
        local button = window:AddButton(displayName)
        button.IDContext = ModuleUUID .. "_PresetPicker_" .. displayName
        button.Size = kPresetButtonSize
        button.OnClick = function()
            ApplyPresetByName(displayName)
        end
        table.insert(presetButtons, button)
    end
end

local presetPickerWindow = nil
if MCM and MCM.EventButton and MCM.EventButton.RegisterCallback then
    MCM.EventButton.RegisterCallback("OpenPresetPicker", function()
        if not presetPickerWindow then
            presetPickerWindow = BuildPresetPickerWindow()
        end
        RefreshPresetButtons(presetPickerWindow)
        presetPickerWindow.Open = true
        presetPickerWindow.Visible = true
    end)
else
    Ext.Utils.PrintWarning("[TTPC] MCM.EventButton.RegisterCallback API not found on client - the 'Open Preset Picker' button won't do anything!")
end

local CAMERA_STATE_PATH = "TrueThirdPersonCamera/camera_state.json"
local s_combatFocusTrackingEnabled = false
local s_combatActionPhase = false
local s_lastCameraStatePollTime = 0
local CAMERA_STATE_POLL_INTERVAL_MS = 250

local function PollCombatFocusTrackingEnabled()
    local now = Ext.Utils.MonotonicTime()
    if now - s_lastCameraStatePollTime < CAMERA_STATE_POLL_INTERVAL_MS then
        return
    end
    s_lastCameraStatePollTime = now

    local raw = Ext.IO.LoadFile(CAMERA_STATE_PATH)
    if not raw or raw == "" then
        return
    end
    local ok, data = pcall(Ext.Json.Parse, raw)
    if ok and type(data) == "table" then
        if type(data.CombatFocusTrackingEnabled) == "boolean" then
            s_combatFocusTrackingEnabled = data.CombatFocusTrackingEnabled
        end
        if type(data.CombatActionPhase) == "boolean" then
            s_combatActionPhase = data.CombatActionPhase
        end
    end
end

local function GetAllCameraBehaviorEntities()
    return Ext.Entity.GetAllEntitiesWithComponent("GameCameraBehavior")
end

local kRecenterHeightOffset = 0.75

local kRecenterWindowMs = 500
local s_recenterActiveUntil = {}

local kRecenterDriftThreshold = 0.3
local kRecenterMaxWindowMs = 3000

local function TargetHasDrifted(behavior)
    local ok, drifted = pcall(function()
        local trigger = behavior.Trigger and Ext.Entity.Get(behavior.Trigger)
        if not (trigger and trigger.Transform) then
            return false
        end
        local dest = behavior.TargetDestination
        if not dest then
            return true
        end
        local pos = trigger.Transform.Transform.Translate
        local dx = dest[1] - pos[1]
        local dy = dest[2] - (pos[2] + kRecenterHeightOffset)
        local dz = dest[3] - pos[3]
        return (dx * dx + dy * dy + dz * dz) > (kRecenterDriftThreshold * kRecenterDriftThreshold)
    end)
    return ok and drifted or false
end

local function DoRecenter(behavior)
    behavior.PlayerInControl = false
    local trigger = behavior.Trigger and Ext.Entity.Get(behavior.Trigger)
    if trigger and trigger.Transform then
        local pos = trigger.Transform.Transform.Translate
        local target = { pos[1], pos[2] + kRecenterHeightOffset, pos[3] }
        behavior.TargetDestination = target
        behavior.TargetCurrent = target
    end
end

Ext.Events.Tick:Subscribe(function()
    for i, camEntity in ipairs(GetAllCameraBehaviorEntities()) do
        if camEntity and camEntity.GameCameraBehavior then
            local behavior = camEntity.GameCameraBehavior

            if behavior.WasInSelectMode and not behavior.SelectMode then
                s_recenterActiveUntil[i] = Ext.Utils.MonotonicTime() + kRecenterWindowMs
            end

            if s_recenterActiveUntil[i] then
                if behavior.SelectMode then
                    s_recenterActiveUntil[i] = nil
                elseif Ext.Utils.MonotonicTime() >= s_recenterActiveUntil[i] then
                    s_recenterActiveUntil[i] = nil
                else
                    DoRecenter(behavior)
                end
            end
        end
    end
end)

local s_prevCombatActionPhase = false
local s_actionPhaseRecenter = {}
local s_prevIcActiveByCam = {}

Ext.Events.Tick:Subscribe(function()
    local justEnteredActionPhase = s_combatActionPhase and not s_prevCombatActionPhase
    s_prevCombatActionPhase = s_combatActionPhase

    for i, camEntity in ipairs(GetAllCameraBehaviorEntities()) do
        if camEntity and camEntity.GameCameraBehavior then
            local behavior = camEntity.GameCameraBehavior

            local camKey = tostring(camEntity)
            local trig = behavior.Trigger and Ext.Entity.Get(behavior.Trigger)
            local cc = trig and trig.ClientCharacter
            local ic = cc and cc.InputController
            local icActive = (ic and ic.Active) and true or false
            local rawUid = cc and (cc.ReservedUserID or cc.OwnerUserID)
            local ownTrigger = type(rawUid) == "number" and rawUid >= 0 and rawUid <= 255
            local icJustLocked = (s_prevIcActiveByCam[camKey] == true) and not icActive
            s_prevIcActiveByCam[camKey] = icActive

            local inputLockTrigger = icJustLocked and ownTrigger and s_combatFocusTrackingEnabled
            local now = Ext.Utils.MonotonicTime()

            if (justEnteredActionPhase or inputLockTrigger) and behavior.PlayerInControl then
                if not s_actionPhaseRecenter[camKey] then
                    s_actionPhaseRecenter[camKey] = { started = now, sawInputLock = false }
                end
            end

            local window = s_actionPhaseRecenter[camKey]
            if window then
                if not icActive then
                    window.sawInputLock = true
                end
                local elapsed = now - window.started
                local actionFinished = window.sawInputLock and icActive
                local timedOut = elapsed >= kRecenterMaxWindowMs
                    or (not window.sawInputLock and elapsed >= kRecenterWindowMs)

                if actionFinished or timedOut then
                    s_actionPhaseRecenter[camKey] = nil
                elseif TargetHasDrifted(behavior) then
                    DoRecenter(behavior)
                end
            end
        end
    end
end)

Ext.Events.Tick:Subscribe(function()
    PollCombatFocusTrackingEnabled()

    for _, camEntity in ipairs(GetAllCameraBehaviorEntities()) do
        if camEntity and camEntity.GameCameraBehavior then
            local behavior = camEntity.GameCameraBehavior
            if s_combatFocusTrackingEnabled or behavior.SelectMode then
                local wantTarget = behavior.Target or behavior.Trigger
                if wantTarget and #behavior.Targets == 0 then
                    behavior.Targets[1] = wantTarget
                end
            end
        end
    end
end)

local CLIENT_STATE_PATH = "TrueThirdPersonCamera/client_state.json"
local s_lastWrittenCameras = {}

local function WriteClientState(cameras)
    local ok, encoded = pcall(Ext.Json.Stringify, { Cameras = cameras })
    if ok then
        Ext.IO.SaveFile(CLIENT_STATE_PATH, encoded)
    else
        Ext.Utils.PrintWarning("[TTPC] WriteClientState Json.Stringify failed: " .. tostring(encoded))
    end
end

local WRITE_MIN_INTERVAL_MS = 150
local s_lastClientStateWriteTime = 0

local MAX_VALID_USER_ID = 255
local CAM_OWNER_FORGET_TICKS = 300
local s_camOwnerUserId = {}
local s_camAbsentTicks = {}

local function RawUserIdOf(cc)
    local raw = cc and (cc.ReservedUserID or cc.OwnerUserID)
    if type(raw) == "number" and raw >= 0 and raw <= MAX_VALID_USER_ID then
        return raw
    end
    return nil
end

local function ExpireAbsentCamOwners(presentKeys)
    for camKey in pairs(s_camOwnerUserId) do
        if presentKeys[camKey] then
            s_camAbsentTicks[camKey] = 0
        else
            local n = (s_camAbsentTicks[camKey] or 0) + 1
            if n >= CAM_OWNER_FORGET_TICKS then
                s_camOwnerUserId[camKey] = nil
                s_camAbsentTicks[camKey] = nil
            else
                s_camAbsentTicks[camKey] = n
            end
        end
    end
end

Ext.Events.Tick:Subscribe(function()
    local ok, err = pcall(function()
        local camEntities = GetAllCameraBehaviorEntities()

        local presentKeys = {}
        local claimedUserIds = {}
        for _, camEntity in ipairs(camEntities) do
            if camEntity and camEntity.GameCameraBehavior then
                local camKey = tostring(camEntity)
                presentKeys[camKey] = true
                local owned = s_camOwnerUserId[camKey]
                if owned then
                    claimedUserIds[owned] = true
                end
            end
        end
        ExpireAbsentCamOwners(presentKeys)

        local cameras = {}
        for _, camEntity in ipairs(camEntities) do
            if camEntity and camEntity.GameCameraBehavior then
                local behavior = camEntity.GameCameraBehavior
                local trig = behavior.Trigger and Ext.Entity.Get(behavior.Trigger)
                local cc = trig and trig.ClientCharacter

                local camKey = tostring(camEntity)
                local userId = s_camOwnerUserId[camKey]
                if not userId then
                    local raw = RawUserIdOf(cc)
                    if raw and not claimedUserIds[raw] then
                        s_camOwnerUserId[camKey] = raw
                        s_camAbsentTicks[camKey] = 0
                        claimedUserIds[raw] = true
                        userId = raw
                    end
                end

                if userId then
                    local ic = cc and cc.InputController
                    cameras[#cameras + 1] = {
                        UserID = userId,
                        SelectMode = behavior.SelectMode and true or false,
                        InputControllerActive = (ic and ic.Active) and true or false,
                        EnemyTurnActive = behavior.Target and true or false,
                    }
                end
            end
        end

        table.sort(cameras, function(a, b) return a.UserID < b.UserID end)

        local valueChanged = #cameras ~= #s_lastWrittenCameras
        local reason = valueChanged and ("count " .. #s_lastWrittenCameras .. "->" .. #cameras) or nil
        if not valueChanged then
            for i, cam in ipairs(cameras) do
                local last = s_lastWrittenCameras[i]
                if cam.UserID ~= last.UserID then
                    valueChanged = true
                    reason = "i=" .. i .. " UserID " .. tostring(last.UserID) .. "->" .. tostring(cam.UserID)
                    break
                end
                if cam.SelectMode ~= last.SelectMode then
                    valueChanged = true
                    reason = "i=" .. i .. " (UserID " .. tostring(cam.UserID) .. ") SelectMode " .. tostring(last.SelectMode) .. "->" .. tostring(cam.SelectMode)
                    break
                end
                if (cam.SelectMode or last.SelectMode or s_combatFocusTrackingEnabled) and (cam.InputControllerActive ~= last.InputControllerActive or cam.EnemyTurnActive ~= last.EnemyTurnActive) then
                    valueChanged = true
                    reason = "i=" .. i .. " (UserID " .. tostring(cam.UserID) .. ") InputControllerActive " .. tostring(last.InputControllerActive) .. "->" .. tostring(cam.InputControllerActive)
                        .. " / EnemyTurnActive " .. tostring(last.EnemyTurnActive) .. "->" .. tostring(cam.EnemyTurnActive)
                    break
                end
            end
        end

        if valueChanged then
            local now = Ext.Utils.MonotonicTime()
            if now - s_lastClientStateWriteTime >= WRITE_MIN_INTERVAL_MS then
                s_lastClientStateWriteTime = now
                s_lastWrittenCameras = cameras
                WriteClientState(cameras)
            end
        end
    end)
    if not ok then
        Ext.Utils.PrintWarning("[TTPC] SelectMode/InputControllerActive/EnemyTurnActive bridge Tick handler errored: " .. tostring(err))
    end
end)

