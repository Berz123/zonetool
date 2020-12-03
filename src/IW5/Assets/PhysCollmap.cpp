// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"

namespace ZoneTool
{
	namespace IW5
	{
		PhysCollmap* IPhysCollmap::parse(const std::string& name, ZoneMemory* mem)
		{
			auto version = 4;
			auto path = "physcollmap/" + name + ".cme4";

			if (!FileSystem::FileExists(path))
			{
				version = 3;
				path = "physcollmap/" + name + ".cme3";

				if (!FileSystem::FileExists(path))
				{
					return nullptr;
				}
			}

			AssetReader read(mem);

			if (!read.open(path))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing phys_collmap \"%s\"...", name.c_str());

			auto* phys_collmap = read.read_array<PhysCollmap>();
			phys_collmap->name = mem->StrDup(name);

			if (read.read_int())
			{
				phys_collmap->info = read.read_array<PhysGeomInfo>();

				for (auto i = 0; i < phys_collmap->numInfo; i++)
				{
					if (read.read_int())
					{
						phys_collmap->info[i].brush = read.read_array<BrushWrapper>();

						if (version == 4)
						{
							if (read.read_int())
							{
								phys_collmap->info[i].brush->plane = read.read_array<cplane_s>();
							}
						}

						if (read.read_int())
						{
							phys_collmap->info[i].brush->side = read.read_array<cbrushside_t>();

							for (auto j = 0u; j < phys_collmap->info[i].brush->numPlaneSide; j++)
							{
								if (read.read_int())
								{
									phys_collmap->info[i].brush->side[j].plane = read.read_array<cplane_s>();
								}
							}
						}

						if (read.read_int())
						{
							phys_collmap->info[i].brush->edge = read.read_array<char>();
						}

						if (version == 3)
						{
							if (read.read_int())
							{
								phys_collmap->info[i].brush->plane = read.read_array<cplane_s>();
							}
						}
					}
				}
			}

			return phys_collmap;
		}

		void IPhysCollmap::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = name;
			this->asset_ = this->parse(name, mem);

			if (!this->asset_)
			{
				this->asset_ = DB_FindXAssetHeader(this->type(), this->name().data(), 1).physcollmap;
			}
		}

		void IPhysCollmap::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		void IPhysCollmap::load_depending(IZone* zone)
		{
		}

		std::string IPhysCollmap::name()
		{
			return this->name_;
		}

		std::int32_t IPhysCollmap::type()
		{
			return phys_collmap;
		}

		void IPhysCollmap::write_brush_wrapper(IZone* zone, ZoneBuffer* buf, BrushWrapper* data)
		{
			auto* dest = buf->write(data);

			if (data->side)
			{
				buf->align(3);
				auto* destsides = buf->write(data->side, data->numPlaneSide);

				for (auto i = 0; i < data->numPlaneSide; i++)
				{
					destsides[i].plane = buf->write_s(3, data->side[i].plane);
				}

				ZoneBuffer::clear_pointer(&dest->side);
			}

			if (data->edge)
			{
				buf->align(0);
				buf->write(data->edge, data->numEdge);
				ZoneBuffer::clear_pointer(&dest->edge);
			}

			if (data->plane)
			{
				dest->plane = buf->write_s(3, data->plane, data->numPlaneSide);
			}
		}

		void IPhysCollmap::write_phys_geom_info(IZone* zone, ZoneBuffer* buf, PhysGeomInfo* dest)
		{
			auto* data = dest;

			if (data->brush)
			{
				buf->align(3);
				this->write_brush_wrapper(zone, buf, data->brush);
				ZoneBuffer::clear_pointer(&dest->brush);
			}
		}

		void IPhysCollmap::write(IZone* zone, ZoneBuffer* buf)
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			buf->push_stream(3);

			dest->name = buf->write_str(this->name());

			if (data->info)
			{
				buf->align(3);
				auto* destinfo = buf->write(data->info, data->numInfo);

				for (auto i = 0; i < data->numInfo; i++)
				{
					this->write_phys_geom_info(zone, buf, &destinfo[i]);
				}
			}

			buf->pop_stream();
		}

		void IPhysCollmap::dump(PhysCollmap* asset)
		{
			const auto path = "physcollmap/" + std::string(asset->name) + ".cme4";

			AssetDumper dump;
			dump.open(path.data());
			dump.dump_array(asset, 1);

			if (asset->info)
			{
				dump.dump_int(1);
				dump.dump_array(asset->info, asset->numInfo);

				for (auto i = 0; i < asset->numInfo; i++)
				{
					if (asset->info[i].brush)
					{
						dump.dump_int(1);
						dump.dump_array(asset->info[i].brush, 1);

						if (asset->info[i].brush->plane)
						{
							dump.dump_int(1);
							dump.dump_array(asset->info[i].brush->plane, asset->info[i].brush->numPlaneSide);
						}
						else
						{
							dump.dump_int(0);
						}

						if (asset->info[i].brush->side)
						{
							dump.dump_int(1);
							dump.dump_array(asset->info[i].brush->side, asset->info[i].brush->numPlaneSide);

							for (auto j = 0u; j < asset->info[i].brush->numPlaneSide; j++)
							{
								if (asset->info[i].brush->side[j].plane)
								{
									dump.dump_int(1);
									dump.dump_array(asset->info[i].brush->side[j].plane, 1);
								}
								else
								{
									dump.dump_int(0);
								}
							}
						}
						else
						{
							dump.dump_int(0);
						}

						if (asset->info[i].brush->edge)
						{
							dump.dump_int(1);
							dump.dump_array(asset->info[i].brush->edge, asset->info[i].brush->numEdge);
						}
						else
						{
							dump.dump_int(0);
						}
					}
					else
					{
						dump.dump_int(0);
					}
				}
			}
			else
			{
				dump.dump_int(0);
			}

			dump.close();
		}
	}
}
