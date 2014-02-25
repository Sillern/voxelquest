// f00351_gamepageholder.h
//

#include "f00351_gamepageholder.e"
#define LZZ_INLINE inline
GamePageHolder::GamePageHolder ()
                         {
		usingPoolId = -1;
		hasTrans = false;
		hasSolids = false;
	}
void GamePageHolder::init (Singleton * _singleton, int _blockID, int _holderID, int trueX, int trueY, int trueZ)
          {

		pushTrace("GamePageHolder init()");

		int i;

		

		blockID = _blockID;
		holderID = _holderID;

		isDirty = false;

		singleton = _singleton;
		//thisHolderId = _thisHolderId;
		usingPoolId = -1;

		//trueOffsetInHolders.setIXYZ(trueX,trueY,trueZ);
		offsetInHolders.setIXYZ(trueX,trueY,trueZ);
		offsetInBlocks.copyFrom(&offsetInHolders);
		offsetInBlocks.intDivXYZ(singleton->blockSizeInHolders);
		
		gphMinInPixels.copyFrom(&offsetInHolders);
		gphMaxInPixels.copyFrom(&offsetInHolders);

		gphMaxInPixels.addXYZ(1);

		gphMinInPixels.multXYZ(singleton->holderSizeInPixels);
		gphMaxInPixels.multXYZ(singleton->holderSizeInPixels);


		holderSizeInPages = singleton->holderSizeInPages;
		iPageDataVolume = holderSizeInPages*holderSizeInPages*holderSizeInPages;
		pageData = new GamePage*[iPageDataVolume];

		for (i = 0; i < iPageDataVolume; i++) {
			pageData[i] = NULL;
		}



		
		fetchGeom();



		popTrace();

	}
void GamePageHolder::clearSet (bool forceClear)
                                       {
		int i;

		bool doClear = forceClear;

		if (usingPoolId == -1) {
			usingPoolId = singleton->requestPoolId(blockID,holderID);
			gpuRes = singleton->holderPoolItems[usingPoolId];

			doClear = true;
		}

		if (doClear) {
			for (i = 0; i < MAX_LAYERS; i++) {
				// clear fbo by binding it with auto flag
				singleton->bindFBODirect(gpuRes->getFBOS(i));
				singleton->unbindFBO();
			}
		}
		
	}
void GamePageHolder::refreshChildren (bool refreshImmediate)
                                                    {
		int i;

		clearSet(true);
		

		for (i = 0; i < iPageDataVolume; i++) {
			if (pageData[i] == NULL) {

			}
			else {

				if (refreshImmediate) {
					pageData[i]->generateVolume();
				}
				else {
					pageData[i]->curState = E_STATE_CREATESIMPLEXNOISE_END;
				}

				
			}
		}
	}
void GamePageHolder::fetchGeom ()
                         {
		int i;
		int j;
		int k;
		int bufSize = (singleton->visPageSizeInPixels*singleton->bufferMult)*2;
		
		GameBlock* curBlock;
		GamePageHolder* gph;
		FIVector4 start;
		FIVector4 end;
		GameGeom* geom;

		containsGeomIds.clear();

		for (i = -1; i <= 1; i++) {
			for (j = -1; j <= 1; j++) {
				curBlock = singleton->gw->getBlockAtCoords(
					offsetInBlocks.getIX()+i,
					offsetInBlocks.getIY()+j,
					true
				);

				for (k = 0; k < curBlock->gameGeom.size(); k++) {


					geom = curBlock->gameGeom[k];


					start.copyFrom( &(geom->moveMinInPixels) );
					end.copyFrom( &(geom->moveMaxInPixels) );

					start.addXYZ(-bufSize);
					end.addXYZ(bufSize);

					//start.intDivXYZ(singleton->holderSizeInPixels);
					//end.intDivXYZ(singleton->holderSizeInPixels);

					start.clampZ(0.0,singleton->maxBoundsInPixels.getFZ()-1.0f);
					end.clampZ(0.0,singleton->maxBoundsInPixels.getFZ()-1.0f);

					if (FIVector4::intersect(&start,&end,&gphMinInPixels,&gphMaxInPixels)) {
						containsGeomIds.push_back(intPair());
						containsGeomIds.back().v0 = curBlock->blockID;
						containsGeomIds.back().v1 = k;
					}


					

				}
			}			
		}
	}
void GamePageHolder::unbindGPUResources ()
                                  {
		//pushTraceND("unbindGPUResources");

		usingPoolId = -1;
		gpuRes = NULL;

		singleton->gw->threadpool.joinAll();

		int i;
		for (i = 0; i < iPageDataVolume; i++) {
			if (pageData[i] == NULL) {

			}
			else {
				if (pageData[i]->curState >= E_STATE_GENERATEVOLUME_LAUNCH) {
					pageData[i]->curState = E_STATE_CREATESIMPLEXNOISE_END;
					pageData[i]->nextState = E_STATE_WAIT;
				}
			}
		}


		// TODO: clear color/depth buffers on release

		//popTraceND();

	}
#undef LZZ_INLINE
 