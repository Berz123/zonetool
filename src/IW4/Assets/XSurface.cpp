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
	namespace IW4
	{
		ModelSurface* IXSurface::parse(const std::string& name, ZoneMemory* mem)
		{
			AssetReader read(mem);

			if (!read.open("XSurface\\" + name + ".xse"))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing xmodel surface \"%s\"...", name.c_str());

			auto asset = read.read_array<ModelSurface>();
			asset->name = read.read_string();

			asset->xSurficies = mem->Alloc<XSurface>(asset->xSurficiesCount);

			for (int i = 0; i < asset->xSurficiesCount; i++)
			{
				asset->xSurficies[i].tileMode = read.read_int();
				asset->xSurficies[i].deformed = read.read_int();
				asset->xSurficies[i].baseTriIndex = read.read_int();
				asset->xSurficies[i].baseVertIndex = read.read_int();

				for (int j = 0; j < 6; j++)
				{
					asset->xSurficies[i].partBits[j] = read.read_int();
				}

				// vertex
				auto vertexInfo = read.read_array<XSurfaceVertexInfo>();
				memcpy(&asset->xSurficies[i].vertexInfo, vertexInfo, sizeof XSurfaceVertexInfo);
				asset->xSurficies[i].vertexInfo.vertsBlend = read.read_array<unsigned short>();

				// verticies
				asset->xSurficies[i].vertCount = read.read_int();
				asset->xSurficies[i].verticies = read.read_array<GfxPackedVertex>();

				// triangles
				asset->xSurficies[i].triCount = read.read_int();
				asset->xSurficies[i].triIndices = read.read_array<Face>();

				// rigidVertLists
				asset->xSurficies[i].vertListCount = read.read_int();
				asset->xSurficies[i].rigidVertLists = read.read_array<XRigidVertList>();
				for (int vert = 0; vert < asset->xSurficies[i].vertListCount; vert++)
				{
					if (asset->xSurficies[i].rigidVertLists[vert].collisionTree)
					{
						asset->xSurficies[i].rigidVertLists[vert].collisionTree = read.read_array<XSurfaceCollisionTree>();

						if (asset->xSurficies[i].rigidVertLists[vert].collisionTree->leafs)
						{
							asset->xSurficies[i].rigidVertLists[vert].collisionTree->leafs = read.read_array<
								XSurfaceCollisionLeaf>();
						}

						if (asset->xSurficies[i].rigidVertLists[vert].collisionTree->nodes)
						{
							asset->xSurficies[i].rigidVertLists[vert].collisionTree->nodes = read.read_array<
								XSurfaceCollisionNode>();
						}
					}
					else
					{
						asset->xSurficies[i].rigidVertLists[vert].collisionTree = nullptr;
					}
				}
			}

			read.close();

			return asset;
		}

		void IXSurface::init(const std::string& name, ZoneMemory* mem)
		{
			this->m_name = name;
			this->m_asset = this->parse(name, mem);

			if (!this->m_asset)
			{
				this->m_asset = DB_FindXAssetHeader(this->type(), this->m_name.data()).xsurface;
			}
		}

		void IXSurface::init(void* asset, ZoneMemory* mem)
		{
			this->m_asset = reinterpret_cast<ModelSurface*>(asset);
			this->m_name = this->m_asset->name;
		}

		void IXSurface::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		void IXSurface::load_depending(IZone* zone)
		{
		}

		std::string IXSurface::name()
		{
			return this->m_name;
		}

		std::int32_t IXSurface::type()
		{
			return xmodelsurfs;
		}

		void IXSurface::write_xsurfices(IZone* zone, ZoneBuffer* buf, XSurface* data, XSurface* dest,
		                                std::uint16_t count)
		{
			assert(sizeof XSurface, 64);
			assert(sizeof Face, 6);
			assert(sizeof XRigidVertList, 12);
			assert(sizeof XSurfaceCollisionTree, 40);
			assert(sizeof XSurfaceCollisionLeaf, 2);
			assert(sizeof XSurfaceCollisionAabb, 12);
			assert(sizeof XSurfaceCollisionNode, 16);
			assert(sizeof GfxPackedVertex, 32);
			assert(sizeof XSurfaceVertexInfo, 12);

			for (std::int16_t surf = 0; surf < count; surf++)
			{
				if (data[surf].vertexInfo.vertsBlend)
				{
					dest[surf].vertexInfo.vertsBlend = buf->write_s(1, data[surf].vertexInfo.vertsBlend,
					                                                data[surf].vertexInfo.vertCount[0] +
					                                                (data[surf].vertexInfo.vertCount[1] * 3) + (data[
						                                                surf].vertexInfo.vertCount[2] * 5) + (data[
						                                                surf].vertexInfo.vertCount[3] * 7));
				}

				buf->push_stream(6);
				if (data[surf].verticies)
				{
					dest[surf].verticies = buf->write_s(15, data[surf].verticies, data[surf].vertCount);
				}
				buf->pop_stream();

				if (data[surf].rigidVertLists)
				{
					XRigidVertList* ct = nullptr;
					dest[surf].rigidVertLists = buf->write_s(3, data[surf].rigidVertLists, data[surf].vertListCount,
					                                         sizeof XRigidVertList, &ct);

					if (dest[surf].rigidVertLists == reinterpret_cast<XRigidVertList*>(-1))
					{
						for (int k = 0; k < data[surf].vertListCount; k++)
						{
							if (ct[k].collisionTree)
							{
								XSurfaceCollisionTree* entry = nullptr;
								ct[k].collisionTree = buf->write_s(3, ct[k].collisionTree, 1,
								                                   sizeof XSurfaceCollisionTree, &entry);

								if (ct[k].collisionTree == reinterpret_cast<XSurfaceCollisionTree*>(-1))
								{
									if (entry->nodes)
									{
										entry->nodes = buf->write_s(15, entry->nodes, entry->nodeCount);
									}

									if (entry->leafs)
									{
										entry->leafs = buf->write_s(1, entry->leafs, entry->leafCount);
									}
								}
							}
						}
					}
				}

				buf->push_stream(7);
				if (data[surf].triIndices)
				{
					dest[surf].triIndices = buf->write_s(15, data[surf].triIndices, data[surf].triCount);
				}
				buf->pop_stream();
			}
		}

		void IXSurface::write(IZone* zone, ZoneBuffer* buf)
		{
			auto data = this->m_asset;
			auto dest = buf->write<ModelSurface>(data);

			assert(sizeof ModelSurface, 36);

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->name = buf->write_str(this->name());

			if (data->xSurficies)
			{
				buf->align(3);
				auto destsurfaces = buf->write(data->xSurficies, data->xSurficiesCount);
				this->write_xsurfices(zone, buf, data->xSurficies, destsurfaces, data->xSurficiesCount);
				ZoneBuffer::ClearPointer(&dest->xSurficies);
			}

			END_LOG_STREAM;
			buf->pop_stream();
		}

		void IXSurface::dump(ModelSurface* asset)
		{
			AssetDumper dump;
			dump.open("XSurface\\"s + asset->name + ".xse");

			if (asset->name == "tntbomb_mp_low10"s)
			{
				return;
			}

			dump.dump_array(asset, 1);
			dump.dump_string(asset->name);

			for (int i = 0; i < asset->xSurficiesCount; i++)
			{
				dump.dump_int(asset->xSurficies[i].tileMode);
				dump.dump_int(asset->xSurficies[i].deformed);
				dump.dump_int(asset->xSurficies[i].baseTriIndex);
				dump.dump_int(asset->xSurficies[i].baseVertIndex);

				for (int j = 0; j < 6; j++)
				{
					dump.dump_int(asset->xSurficies[i].partBits[j]);
				}

				// vertex bs
				dump.dump_array(&asset->xSurficies[i].vertexInfo, 1);
				dump.dump_array(asset->xSurficies[i].vertexInfo.vertsBlend,
				           asset->xSurficies[i].vertexInfo.vertCount[0] +
				           (asset->xSurficies[i].vertexInfo.vertCount[1] * 3) +
				           (asset->xSurficies[i].vertexInfo.vertCount[2] * 5) +
				           (asset->xSurficies[i].vertexInfo.vertCount[3] * 7)
				);

				dump.dump_int(asset->xSurficies[i].vertCount);
				dump.dump_array(asset->xSurficies[i].verticies, asset->xSurficies[i].vertCount);

				dump.dump_int(asset->xSurficies[i].triCount);
				dump.dump_array(asset->xSurficies[i].triIndices, asset->xSurficies[i].triCount);

				dump.dump_int(asset->xSurficies[i].vertListCount);
				dump.dump_array(asset->xSurficies[i].rigidVertLists, asset->xSurficies[i].vertListCount);
				for (int vert = 0; vert < asset->xSurficies[i].vertListCount; vert++)
				{
					if (asset->xSurficies[i].rigidVertLists[vert].collisionTree)
					{
						dump.dump_array(asset->xSurficies[i].rigidVertLists[vert].collisionTree, 1);

						if (asset->xSurficies[i].rigidVertLists[vert].collisionTree->leafs)
						{
							dump.dump_array(asset->xSurficies[i].rigidVertLists[vert].collisionTree->leafs,
							           asset->xSurficies[i].rigidVertLists[vert].collisionTree->leafCount);
						}

						if (asset->xSurficies[i].rigidVertLists[vert].collisionTree->nodes)
						{
							dump.dump_array(asset->xSurficies[i].rigidVertLists[vert].collisionTree->nodes,
							           asset->xSurficies[i].rigidVertLists[vert].collisionTree->nodeCount);
						}
					}
				}
			}

			dump.close();
		}

		IXSurface::IXSurface()
		{
		}

		IXSurface::~IXSurface()
		{
		}
	}
}
