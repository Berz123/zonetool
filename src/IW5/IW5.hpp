// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#pragma once

#include <ZoneUtils.hpp>
#include "Functions.hpp"
#include "Structs.hpp"
#include "Patches/FFCompression.hpp"
#include "Patches/AssetHandler.hpp"
#include "Zone.hpp"

namespace ZoneTool
{
	namespace IW5
	{
		class Linker : public ILinker
		{
		private:
			static void MessageLoop();

		public:
			Linker();
			~Linker();

			const char* Version() override;
			bool InUse() override;
			void Startup() override;
			std::shared_ptr<IZone> AllocZone(std::string& zone) override;
			std::shared_ptr<ZoneBuffer> AllocBuffer() override;
			void LoadZone(std::string& name) override;
			void UnloadZones() override;
			bool IsValidAssetType(std::string& type) override;
			std::int32_t TypeToInt(std::string type) override;
			std::string TypeToString(std::int32_t type) override;
            bool SupportsBuilding() override;

			void DumpZone(std::string& name) override;
			void VerifyZone(std::string& name) override;
		};
	}
}
