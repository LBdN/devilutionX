#include "diablo.h"

DEVILUTION_BEGIN_NAMESPACE

void town_special(BYTE *pBuff, int nCel)
{
#if 0
	CelBlitFrame(pBuff, pSpecialCels, nCel, 64);
#endif
}

/**
 * This function it self causes rendering issues since it will render some walls a secound time after all items have been drawn.
 *
 * @brief Avoid actors sticking threw the walls when walking east
 */
void town_draw_e_flag(BYTE *pBuff, int x, int y, int capChunks, int sx, int sy)
{
	int i;
	BYTE *dst;
	MICROS *pMap;

	dst = pBuff;
	pMap = &dpiece_defs_map_2[x][y];

	for (i = 0; i < 7; i++) {
		if (capChunks >= i) {
			level_cel_block = pMap->mt[2 * i];
			if (level_cel_block != 0) {
				drawUpperScreen(dst);
			}
			level_cel_block = pMap->mt[2 * i + 1];
			if (level_cel_block != 0) {
				drawUpperScreen(dst + 32);
			}
		}
		dst -= BUFFER_WIDTH * 32;
	}

	town_draw_town_all(pBuff, x, y, sx, sy, 0);
}

void town_draw_town_all(BYTE *pBuff, int x, int y, int sx, int sy, int eflag)
{
	int mi, px, py;
	char bv;

	if (dItem[x][y] != 0) {
		bv = dItem[x][y] - 1;
		px = sx - item[bv]._iAnimWidth2;
		if (bv == pcursitem) {
			CelBlitOutline(181, px, sy, item[bv]._iAnimData, item[bv]._iAnimFrame, item[bv]._iAnimWidth);
		}
		/// ASSERT: assert(item[bv]._iAnimData);
		CelClippedDraw(px, sy, item[bv]._iAnimData, item[bv]._iAnimFrame, item[bv]._iAnimWidth);
	}
	if (dFlags[x][y] & BFLAG_MONSTLR) {
		mi = -(dMonster[x][y - 1] + 1);
		px = sx - towner[mi]._tAnimWidth2;
		if (mi == pcursmonst) {
			CelBlitOutline(166, px, sy, towner[mi]._tAnimData, towner[mi]._tAnimFrame, towner[mi]._tAnimWidth);
		}
		/// ASSERT: assert(towner[mi]._tAnimData);
		CelClippedDraw(px, sy, towner[mi]._tAnimData, towner[mi]._tAnimFrame, towner[mi]._tAnimWidth);
	}
	if (dMonster[x][y] > 0) {
		mi = dMonster[x][y] - 1;
		px = sx - towner[mi]._tAnimWidth2;
		if (mi == pcursmonst) {
			CelBlitOutline(166, px, sy, towner[mi]._tAnimData, towner[mi]._tAnimFrame, towner[mi]._tAnimWidth);
		}
		/// ASSERT: assert(towner[mi]._tAnimData);
		CelClippedDraw(px, sy, towner[mi]._tAnimData, towner[mi]._tAnimFrame, towner[mi]._tAnimWidth);
	}
	if (dFlags[x][y] & BFLAG_PLAYERLR) {
		bv = -(dPlayer[x][y - 1] + 1);
		px = sx + plr[bv]._pxoff - plr[bv]._pAnimWidth2;
		py = sy + plr[bv]._pyoff;
		if (bv == pcursplr) {
			Cl2DrawOutline(165, px, py, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth);
		}
		/// ASSERT: assert(plr[bv]._pAnimData);
		Cl2Draw(px, py, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth);
		if (eflag && plr[bv]._peflag) {
			town_draw_e_flag(pBuff - 64, x - 1, y + 1, 8, sx - 64, sy);
		}
	}
	if (dFlags[x][y] & BFLAG_DEAD_PLAYER) {
		DrawDeadPlayer(x, y, sx, sy);
	}
	if (dPlayer[x][y] > 0) {
		bv = dPlayer[x][y] - 1;
		px = sx + plr[bv]._pxoff - plr[bv]._pAnimWidth2;
		py = sy + plr[bv]._pyoff;
		if (bv == pcursplr) {
			Cl2DrawOutline(165, px, py, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth);
		}
		/// ASSERT: assert(plr[bv]._pAnimData);
		Cl2Draw(px, py, plr[bv]._pAnimData, plr[bv]._pAnimFrame, plr[bv]._pAnimWidth);
		if (eflag && plr[bv]._peflag) {
			town_draw_e_flag(pBuff - 64, x - 1, y + 1, 8, sx - 64, sy);
		}
	}
	if (dFlags[x][y] & BFLAG_MISSILE) {
		DrawMissile(x, y, sx, sy, 0);
	}
	if (dArch[x][y] != 0) {
		town_special(pBuff, dArch[x][y]);
	}
}

void town_draw(int x, int y, int sx, int sy, int chunks, int dPieceRow)
{
	int i, j;
	BYTE *dst;
	MICROS *pMap;

	/// ASSERT: assert(gpBuffer);

	if (dPieceRow & 1) {
		x--;
		y++;
		sx -= 32;
		chunks++;
	}

	for (j = 0; j < chunks; j++) {
		if (y >= 0 && y < MAXDUNY && x >= 0 && x < MAXDUNX) {
			level_cel_block = dPiece[x][y];
			if (level_cel_block != 0) {
				dst = &gpBuffer[sx + BUFFER_WIDTH * sy];
				pMap = &dpiece_defs_map_2[x][y];
				for (i = 0; i < MicroTileLen >> 1; i++) {
					level_cel_block = pMap->mt[2 * i];
					if (level_cel_block != 0) {
						drawUpperScreen(dst);
					}
					level_cel_block = pMap->mt[2 * i + 1];
					if (level_cel_block != 0) {
						drawUpperScreen(dst + 32);
					}
					dst -= BUFFER_WIDTH * 32;
				}
				town_draw_town_all(&gpBuffer[sx + BUFFER_WIDTH * sy], x, y, sx, sy, 1);
			} else {
				world_draw_black_tile(&gpBuffer[sx + BUFFER_WIDTH * sy]);
			}
		} else {
			world_draw_black_tile(&gpBuffer[sx + BUFFER_WIDTH * sy]);
		}
		x++;
		y--;
		sx += 64;
	}
}

void SetTownMicros()
{
	int i, x, y, lv;
	WORD *pPiece;
	MICROS *pMap;

	MicroTileLen = 16;

	for (y = 0; y < MAXDUNY; y++) {
		for (x = 0; x < MAXDUNX; x++) {
			lv = dPiece[x][y];
			pMap = &dpiece_defs_map_2[x][y];
			if (lv != 0) {
				lv--;
				pPiece = (WORD *)&pLevelPieces[32 * lv];
				for (i = 0; i < 16; i++) {
					pMap->mt[i] = SDL_SwapLE16(pPiece[(i & 1) + 14 - (i & 0xE)]);
				}
			} else {
				for (i = 0; i < 16; i++) {
					pMap->mt[i] = 0;
				}
			}
		}
	}

	if (zoomflag) {
		scr_pix_width = SCREEN_WIDTH;
		scr_pix_height = VIEWPORT_HEIGHT;
	} else {
		scr_pix_width = ZOOM_WIDTH;
		scr_pix_height = ZOOM_HEIGHT;
	}
}

void T_FillSector(BYTE *P3Tiles, BYTE *pSector, int xi, int yi, int w, int h)
{
	int i, j, xx, yy, nMap;
	long v1, v2, v3, v4, ii;
	WORD *Sector;

	ii = 4;
	yy = yi;
	for (j = 0; j < h; j++) {
		xx = xi;
		for (i = 0; i < w; i++) {
			WORD *Map;

			Map = (WORD *)&pSector[ii];
			nMap = SDL_SwapLE16(*Map);
			if (nMap) {
				Sector = (((WORD *)&P3Tiles[(nMap - 1) * 8]));
				v1 = SDL_SwapLE16(*(Sector + 0)) + 1;
				v2 = SDL_SwapLE16(*(Sector + 1)) + 1;
				v3 = SDL_SwapLE16(*(Sector + 2)) + 1;
				v4 = SDL_SwapLE16(*(Sector + 3)) + 1;

			} else {
				v1 = 0;
				v2 = 0;
				v3 = 0;
				v4 = 0;
			}
			dPiece[xx][yy] = v1;
			dPiece[xx + 1][yy] = v2;
			dPiece[xx][yy + 1] = v3;
			dPiece[xx + 1][yy + 1] = v4;
			xx += 2;
			ii += 2;
		}
		yy += 2;
	}
}

void T_FillTile(BYTE *P3Tiles, int xx, int yy, int t)
{
	long v1, v2, v3, v4;
	WORD *Tiles;

	Tiles = ((WORD *)&P3Tiles[(t - 1) * 8]);
	v1 = SDL_SwapLE16(*(Tiles + 0)) + 1;
	v2 = SDL_SwapLE16(*(Tiles + 1)) + 1;
	v3 = SDL_SwapLE16(*(Tiles + 2)) + 1;
	v4 = SDL_SwapLE16(*(Tiles + 3)) + 1;

	dPiece[xx][yy] = v1;
	dPiece[xx + 1][yy] = v2;
	dPiece[xx][yy + 1] = v3;
	dPiece[xx + 1][yy + 1] = v4;
}

void T_Pass3()
{
	int xx, yy, x;
	BYTE *P3Tiles, *pSector;

	for (yy = 0; yy < MAXDUNY; yy += 2) {
		for (xx = 0; xx < MAXDUNX; xx += 2) {
			dPiece[xx][yy] = 0;
			dPiece[xx + 1][yy] = 0;
			dPiece[xx][yy + 1] = 0;
			dPiece[xx + 1][yy + 1] = 0;
		}
	}

	P3Tiles = LoadFileInMem("Levels\\TownData\\Town.TIL", NULL);
	pSector = LoadFileInMem("Levels\\TownData\\Sector1s.DUN", NULL);
	T_FillSector(P3Tiles, pSector, 46, 46, 25, 25);
	mem_free_dbg(pSector);
	pSector = LoadFileInMem("Levels\\TownData\\Sector2s.DUN", NULL);
	T_FillSector(P3Tiles, pSector, 46, 0, 25, 23);
	mem_free_dbg(pSector);
	pSector = LoadFileInMem("Levels\\TownData\\Sector3s.DUN", NULL);
	T_FillSector(P3Tiles, pSector, 0, 46, 23, 25);
	mem_free_dbg(pSector);
	pSector = LoadFileInMem("Levels\\TownData\\Sector4s.DUN", NULL);
	T_FillSector(P3Tiles, pSector, 0, 0, 23, 23);
	mem_free_dbg(pSector);

#ifndef SPAWN
	if (gbMaxPlayers == 1) {
		if (!(plr[myplr].pTownWarps & 1)) {
#endif
			T_FillTile(P3Tiles, 48, 20, 320);
#ifndef SPAWN
		}
		if (!(plr[myplr].pTownWarps & 2)) {
#endif
			T_FillTile(P3Tiles, 16, 68, 332);
			T_FillTile(P3Tiles, 16, 70, 331);
#ifndef SPAWN
		}
		if (!(plr[myplr].pTownWarps & 4)) {
#endif
			for (x = 36; x < 46; x++) {
				T_FillTile(P3Tiles, x, 78, random(0, 4) + 1);
			}
#ifndef SPAWN
		}
	}
#endif

	if (quests[QTYPE_PW]._qactive != 3 && quests[QTYPE_PW]._qactive) {
		T_FillTile(P3Tiles, 60, 70, 342);
	} else {
		T_FillTile(P3Tiles, 60, 70, 71);
	}

	mem_free_dbg(P3Tiles);
}

void CreateTown(int entry)
{
	int x, y;

	dminx = 10;
	dminy = 10;
	dmaxx = 84;
	dmaxy = 84;

	if (entry == 0) {
		ViewX = 75;
		ViewY = 68;
	} else if (entry == 1) {
		ViewX = 25;
		ViewY = 31;
	} else if (entry == 7) {
		if (TWarpFrom == 5) {
			ViewX = 49;
			ViewY = 22;
		}
		if (TWarpFrom == 9) {
			ViewX = 18;
			ViewY = 69;
		}
		if (TWarpFrom == 13) {
			ViewX = 41;
			ViewY = 81;
		}
	}

	T_Pass3();
	memset(dLight, 0, sizeof(dLight));
	memset(dFlags, 0, sizeof(dFlags));
	memset(dPlayer, 0, sizeof(dPlayer));
	memset(dMonster, 0, sizeof(dMonster));
	memset(dObject, 0, sizeof(dObject));
	memset(dItem, 0, sizeof(dItem));
	memset(dArch, 0, sizeof(dArch));

	for (y = 0; y < MAXDUNY; y++) {
		for (x = 0; x < MAXDUNX; x++) {
			if (dPiece[x][y] == 360) {
				dArch[x][y] = 1;
			} else if (dPiece[x][y] == 358) {
				dArch[x][y] = 2;
			} else if (dPiece[x][y] == 129) {
				dArch[x][y] = 6;
			} else if (dPiece[x][y] == 130) {
				dArch[x][y] = 7;
			} else if (dPiece[x][y] == 128) {
				dArch[x][y] = 8;
			} else if (dPiece[x][y] == 117) {
				dArch[x][y] = 9;
			} else if (dPiece[x][y] == 157) {
				dArch[x][y] = 10;
			} else if (dPiece[x][y] == 158) {
				dArch[x][y] = 11;
			} else if (dPiece[x][y] == 156) {
				dArch[x][y] = 12;
			} else if (dPiece[x][y] == 162) {
				dArch[x][y] = 13;
			} else if (dPiece[x][y] == 160) {
				dArch[x][y] = 14;
			} else if (dPiece[x][y] == 214) {
				dArch[x][y] = 15;
			} else if (dPiece[x][y] == 212) {
				dArch[x][y] = 16;
			} else if (dPiece[x][y] == 217) {
				dArch[x][y] = 17;
			} else if (dPiece[x][y] == 216) {
				dArch[x][y] = 18;
			}
		}
	}

	SetTownMicros();
}

DEVILUTION_END_NAMESPACE
