#include "PresetScanner.h"
#include <nlohmann/json.hpp>

namespace Settings
{
	using json = nlohmann::json;

	static std::filesystem::path PresetsDir()
	{
		wchar_t localAppData[MAX_PATH];
		if (GetEnvironmentVariableW(L"LOCALAPPDATA", localAppData, MAX_PATH) == 0) {
			return {};
		}
		return std::filesystem::path(localAppData) / L"Larian Studios" / L"Baldur's Gate 3" / L"Script Extender" / L"TrueThirdPersonCamera" / L"Presets";
	}

	static std::string WideToUtf8(const std::wstring& wide)
	{
		if (wide.empty()) {
			return {};
		}
		const int size = WideCharToMultiByte(CP_UTF8, 0, wide.data(), static_cast<int>(wide.size()), nullptr, 0, nullptr, nullptr);
		if (size <= 0) {
			return {};
		}
		std::string out(static_cast<size_t>(size), '\0');
		WideCharToMultiByte(CP_UTF8, 0, wide.data(), static_cast<int>(wide.size()), out.data(), size, nullptr, nullptr);
		return out;
	}

	static std::string NormalizePresetName(const std::string& name)
	{
		std::string out;
		out.reserve(name.size());
		for (unsigned char c : name) {
			if (c == ' ' || c == '_' || c == '-' || c == ':') {
				continue;
			}
			out += static_cast<char>(std::tolower(c));
		}
		return out;
	}

	static bool IsReservedPresetName(const std::string& stem)
	{
		static const std::array<const char*, 7> reserved = { "default", "hellblade", "witcher3", "reddeadredemption2", "eldenring", "batmanarkhamknight", "avatarfrontiersofpandora" };
		const auto normalized = NormalizePresetName(stem);
		for (auto name : reserved) {
			if (normalized == name) {
				return true;
			}
		}
		return false;
	}

	static void WriteExampleCustomPreset(const std::filesystem::path& path)
	{
		nlohmann::ordered_json tpl = {
			{ "name", "New Custom Preset" },
			{ "normal_zoom", 2.5 },
			{ "normal_field_of_view", 40.0 },
			{ "horizontal_offset", 0.40 },
			{ "vertical_offset", 0.75 },
			{ "crouch_vertical_offset", -0.20 },
			{ "running_zoom", 0.0 },
			{ "running_field_of_view", 5.5 },
			{ "running_horizontal_offset", 0.0 },
			{ "running_vertical_offset", 0.0 },
			{ "cast_zoom", 5.0 },
			{ "combat_zoom", 2.9 },
			{ "combat_field_of_view", 40.0 },
			{ "selector_field_of_view", 50.0 },
			{ "selector_zoom", 5.0 },
			{ "far_zoom", 10.0 },
			{ "far_field_of_view", 55.0 }
		};
		std::ofstream file(path);
		if (file.is_open()) {
			file << tpl.dump(4);
		}
	}

	void ScanCustomPresets()
	{
		const auto dir = PresetsDir();
		if (dir.empty()) {
			WARN("Couldn't resolve %LOCALAPPDATA% - skipping custom preset scan"sv);
			return;
		}

		std::error_code ec;
		std::filesystem::create_directories(dir, ec);

		bool folderHasAnyPreset = false;
		for (const auto& entry : std::filesystem::directory_iterator(dir, ec)) {
			if (ec) {
				break;
			}
			if (!entry.is_regular_file() || entry.path().extension() != L".json") {
				continue;
			}
			const auto stem = entry.path().stem().wstring();
			if (stem.empty() || stem.front() == L'_') {
				continue;
			}
			folderHasAnyPreset = true;
			break;
		}

		if (!folderHasAnyPreset) {
			const auto exampleFile = dir / L"custom.json";
			WriteExampleCustomPreset(exampleFile);
			INFO("Wrote starter custom preset: {}", exampleFile.string());
		}

		json manifest = json::array();
		for (const auto& entry : std::filesystem::directory_iterator(dir, ec)) {
			if (ec) {
				break;
			}
			if (!entry.is_regular_file()) {
				continue;
			}
			const auto& path = entry.path();
			if (path.extension() != L".json") {
				continue;
			}

			const auto stem = WideToUtf8(path.stem().wstring());
			if (stem.empty() || stem.front() == '_') {
				continue;
			}
			if (IsReservedPresetName(stem)) {
				WARN("Custom preset file '{}' has the same name as a built-in preset, skipping it", stem);
				continue;
			}
			manifest.push_back(stem);
		}

		std::ofstream out(dir / L"_manifest.json");
		if (out.is_open()) {
			out << manifest.dump();
		}

		INFO("Custom preset scan done, found {} preset(s)", manifest.size());
	}
}
