// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"
#include "../IW4/Assets/XAnimParts.hpp"

namespace ZoneTool
{
	namespace IW3
	{
		void IXAnimParts::dump(XAnimParts* anim)
		{
			if (anim)
			{
				auto asset = new IW4::XAnimParts;
				memset(asset, 0, sizeof IW4::XAnimParts);

#define XAE_CopyElement(name) asset->name = anim->name

				XAE_CopyElement(name);
				XAE_CopyElement(dataByteCount);
				XAE_CopyElement(dataShortCount);
				XAE_CopyElement(dataIntCount);
				XAE_CopyElement(randomDataByteCount);
				XAE_CopyElement(randomDataIntCount);
				XAE_CopyElement(framecount);

                asset->flags = 0;
                if (anim->bLoop)
                    asset->flags |= IW4::ANIM_LOOP;
                if (anim->bDelta)
                    asset->flags |= IW4::ANIM_DELTA;

				for (int i = 0; i < 10; i++)
					XAE_CopyElement(boneCount[i]);
				XAE_CopyElement(notifyCount);
				XAE_CopyElement(assetType);
				XAE_CopyElement(isDefault);
				XAE_CopyElement(randomDataShortCount);
				XAE_CopyElement(indexcount);
				XAE_CopyElement(framerate);
				XAE_CopyElement(frequency);
				XAE_CopyElement(tagnames);
				XAE_CopyElement(dataByte);
				XAE_CopyElement(dataShort);
				XAE_CopyElement(dataInt);
				XAE_CopyElement(randomDataShort);
				XAE_CopyElement(randomDataByte);
				XAE_CopyElement(randomDataInt);
				XAE_CopyElement(indices.data);
				asset->notify = (IW4::XAnimNotifyInfo*)anim->notify;
				asset->delta = (IW4::XAnimDeltaPart*)anim->delta;

				// dump asset
				IW4::IXAnimParts::dump(asset, SL_ConvertToString);

				// free memory
				delete[] asset;
			}
		}
	}
}
