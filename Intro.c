#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <libraries/mathffp.h>
#include <clib/mathffp_protos.h> 
#include <clib/mathtrans_protos.h>

#include "custom_defines.h"

#include "System.h"

#include "clist.h"

#include "c2p_6b.h"

#define k_pi 3.14159265358979f

#define FP 10
#define FP_UNIT 1024

LONG fpsin[1024];
LONG fpcos[1024];

ULONG rowchunky[HEIGHT];
UBYTE frame = 0;
ULONG fps = 0;

ULONG random_v = 0x9c33fe02;

ULONG random(void)
{
  random_v = (random_v * (ULONG)2654435761);
  return random_v>>16;
}

void initMathLibs(void) {
    MathBase = OpenLibrary("mathffp.library", 0);
    MathTransBase = OpenLibrary("mathtrans.library",0);
}

void closeMathLibs(void) {
    if (MathBase != NULL)
        CloseLibrary(MathBase);
    if (MathTransBase != NULL) 
        CloseLibrary(MathTransBase);
}

void generateSineCosTable(void) {
    WORD k = 0;
    FLOAT prc, fsi;
    for (k = 0; k < 1024; k++) {
        prc = 2*k_pi*(k/1024.0);
        fsi = sin(prc);
        fpsin[k] = fsi*FP_UNIT;
        fsi = cos(prc);
        fpcos[k] = fsi*FP_UNIT;
    }
}

void glowline(UBYTE *chunky, LONG x0, LONG y0, LONG x1, LONG y1, ULONG lcol) {
  UWORD pcol0, pcol1, pcol2, pcol3, pcol4;
  LONG *pcol,ncol,ocol,ncol2,i;
  LONG dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
  LONG dy = abs(y1-y0), sy = y0<y1 ? 1 : -1; 
  WORD err = (dx>dy ? (dx>>1) : -(dy>>1)), e2;
  for(;;){
    if (!(x0 < 0 || x0 > 319 ||
          y0 < 0 || y0 > 179)) {
    
        i = x0+rowchunky[y0];
    
        chunky[i] = lcol;
        
        pcol = (ULONG*)&chunky[i-4];
        
        ocol = *pcol;
        ncol = 0;
        
        if (lcol == 0x3f3f3f3f) {
            if (ocol == 0) 
                ocol = 0x07070707;
            if ((ocol&0xff000000) == 0)
                ocol |= 0x07000000;
            if ((ocol&0x00ff0000) == 0) 
                ocol |= 0x00070000;
            if ((ocol&0x0000ff00) == 0)
                ocol |= 0x00000700;
            if ((ocol&0x000000ff) == 0)
                ocol |= 0x00000007;
        }
        
        ncol += ocol & 0xffffffff;
        ncol += ocol & 0xffffff00;
        ncol += ocol & 0xffff0000;

        ncol += lcol & 0x000000ff;
        ncol += lcol & 0x0000ffff;
        ncol += lcol & 0xffffffff;

        ncol>>=2;
        ncol &= 0x38383838;
        
        
        ncol2 = 0;
        
        ncol2 += ocol & 0x07070707;
        ncol2 += ocol & 0x07070700;
        ncol2 += ocol & 0x07070000;

        ncol2 += lcol & 0x00000007;
        ncol2 += lcol & 0x00000707;
        ncol2 += lcol & 0x07070707;
        
        ncol2>>=2;
        ncol2 &= 0x07070707;
        ncol |= ncol2;
        *pcol = ncol;
        
        
        
        pcol = (ULONG*)&chunky[i+1];
        
        ocol = *pcol;
        ncol = 0;
        
        if (lcol == 0x3f3f3f3f) {
            if (ocol == 0) 
                ocol = 0x07070707;
            if ((ocol&0xff000000) == 0)
                ocol |= 0x07000000;
            if ((ocol&0x00ff0000) == 0) 
                ocol |= 0x00070000;
            if ((ocol&0x0000ff00) == 0)
                ocol |= 0x00000700;
            if ((ocol&0x000000ff) == 0)
                ocol |= 0x00000007;
        }
        
        ncol += ocol & 0xffffffff;
        ncol += ocol & 0x00ffffff;
        ncol += ocol & 0x0000ffff;

        ncol += lcol & 0xff000000;
        ncol += lcol & 0xffff0000;
        ncol += lcol & 0xffffffff;
        
        ncol>>=2;
        ncol &= 0x38383838;
        
        
        ncol2 = 0;
        
        ncol2 += ocol & 0x07070707;
        ncol2 += ocol & 0x00070707;
        ncol2 += ocol & 0x00000707;

        ncol2 += lcol & 0x07000000;
        ncol2 += lcol & 0x07070000;
        ncol2 += lcol & 0x07070707;
        
        ncol2>>=2;
        ncol2 &= 0x07070707;
        
        ncol |= ncol2;
        
        *pcol = ncol;
    
    }
    if (x0==x1 && y0==y1) break;
    e2 = err;
    if (e2 >-dx) { err -= dy; x0 += sx; }
    if (e2 < dy) { err += dx; y0 += sy; }
  }
   
}

    

void setPaletteAlphaBlending(void) {
    UBYTE i, j, k;
    ULONG rgb1[8], rgb2[8];
    
    for (i = 0; i < 4; i++) { 
        ULONG lev = (i) * 255 / 6;
        ULONG rgb = (lev<<16)|lev;
        rgb1[i] = rgb;
    }
    for (i = 0; i < 4; i++) { 
        ULONG lev = (i) * 255 / 3;
        ULONG rgb = 0xff00ff|(lev<<8);
        rgb1[i+4] = rgb;
    }

    for (i = 0; i < 4; i++) { 
        ULONG lev = (i) * 255 / 5;
        ULONG rgb = (lev<<8)|lev;
        rgb2[i] = rgb;
    }
    
    for (i = 0; i < 4; i++) { 
        ULONG lev = (i) * 255 / 4;
        ULONG rgb = 0xffff|lev<<16;
        rgb2[i+4] = rgb;
    }
    
    k = 0;
    for (j = 0; j < 8; j++) {
        for (i = 0; i < 8; i++) { 
            UBYTE lred = (((i+1)*(rgb1[j]>>16))+((7-i)*(rgb2[j]>>16)))/9;
            UBYTE lgreen = (((i+1)*((rgb1[j]>>8)&0xff))+((7-i)*((rgb2[j]>>8)&0xff)))/9;
            UBYTE lblue = (((i+1)*(rgb1[j]&0xff))+((7-i)*(rgb2[j]&0xff)))/9;
  
            cop1[79+(k<<1)+(k>31?2:0)]  = ((lred&0xf0)<<4) | (lgreen&0xf0)      | ((lblue&0xf0)>>4);
            cop1[211+(k<<1)+(k>31?2:0)] = ((lred&0x0f)<<8) | ((lgreen&0x0f)<<4) | (lblue&0x0f);
            cop2[79+(k<<1)+(k>31?2:0)]  = ((lred&0xf0)<<4) | (lgreen&0xf0)      | ((lblue&0xf0)>>4);
            cop2[211+(k<<1)+(k>31?2:0)] = ((lred&0x0f)<<8) | ((lgreen&0x0f)<<4) | (lblue&0x0f);
            k++;
        }
    }
}

int main()
{
    UBYTE numstr[32] = "                              \n";
    
    UBYTE *chunky;
    UBYTE *ptr;
    UBYTE *ptr10 = NULL, *ptr11, *ptr12, *ptr13, *ptr14, *ptr15;
    UBYTE *ptr20 = NULL, *ptr21, *ptr22, *ptr23, *ptr24, *ptr25;
    ULONG *ptrl;
    
    UWORD coplistSize;
    
    WORD i, j, l, m;
    
    ULONG k;

    k = 0;
    for (i = 0; i < HEIGHT; i++) {
        rowchunky[i] = k;
        k += 320;
    } 
    
    // Open libs
    initMathLibs();
    
    IntuitionBase = (struct IntuitionBase*)OldOpenLibrary("intuition.library");
    
   	// open gfx lib and save original copperlist
  	GfxBase = (struct GfxBase*)OldOpenLibrary("graphics.library");
   	
    oldCopinit = GfxBase->copinit;
   	oldview = GfxBase->ActiView;
    
    // Alloc memory
    chunky = (UBYTE*)AllocMem(57600, MEMF_ANY | MEMF_CLEAR);
    
    // Alloc memory for 2 copper lists for double buffer
    cop1 = (UWORD*)AllocMem(sizeof(coplist), MEMF_CHIP | MEMF_CLEAR);
    cop2 = (UWORD*)AllocMem(sizeof(coplist), MEMF_CHIP | MEMF_CLEAR);
    
    // Copy content of copper list
    CopyMem(coplist, cop1, sizeof(coplist));
    CopyMem(coplist, cop2, sizeof(coplist));
    
    // Alloc bitplanes
    ptr10 = AllocMem(((WIDTH * HEIGHT) >> 3) * 6, MEMF_CHIP | MEMF_CLEAR);
    ptr11 = ptr10 + 7200;
    ptr12 = ptr11 + 7200;
    ptr13 = ptr12 + 7200;
    ptr14 = ptr13 + 7200;
    ptr15 = ptr14 + 7200;
    
    ptr20 = AllocMem(((WIDTH * HEIGHT) >> 3) * 6, MEMF_CHIP | MEMF_CLEAR);
    ptr21 = ptr20 + 7200;
    ptr22 = ptr21 + 7200;
    ptr23 = ptr22 + 7200;
    ptr24 = ptr23 + 7200;
    ptr25 = ptr24 + 7200;
    
    if (cop1          != NULL &&
        cop2          != NULL && 
        ptr10         != NULL && 
        ptr20         != NULL) {
            
        generateSineCosTable();
    
        setPaletteAlphaBlending();

        coplistSize = (sizeof(coplist)>>1)-4;

        palflag = SysBase->PowerSupplyFrequency < 59;
    
        vbr = (SysBase->AttnFlags & AFF_68010) ?
                (void *)Supervisor((void *)GetVBR) : NULL;

        LoadView(NULL);
        WaitTOF();
        WaitTOF();
        
        // Install input handler
        installInputHandler();
        
        Forbid();
        
        // Save interrupts and DMA
        oldInt = custom->intenar;
        oldDMA = custom->dmaconr;
        
        // disable all interrupts and DMA
        
        custom->intena = 0x7fff;
        custom->intreq = 0x7fff; 
        custom->intreq = 0x7fff;
        custom->dmacon = 0x7fff;
        
        custom->dmacon = DMAF_SETCLR | DMAF_MASTER | DMAF_RASTER | DMAF_BLITTER  | DMAF_COPPER;
        
        
        // Set up pointers for double buffer
        
        bplptr[0] = ptr20;
        bplptr[1] = ptr10;

        clist[0] = (ULONG)cop1;
        clist[1] = (ULONG)cop2;

        // Set up bitplanes in copper list
        
        cop1[BPL0]     = (ULONG)ptr10 >> 16;
        cop1[BPL0 + 2] = (ULONG)ptr10 & 0xffff;
        cop1[BPL1]     = (ULONG)ptr11 >> 16;
        cop1[BPL1 + 2] = (ULONG)ptr11 & 0xffff;
        cop1[BPL2]     = (ULONG)ptr12 >> 16;
        cop1[BPL2 + 2] = (ULONG)ptr12 & 0xffff;
        cop1[BPL3]     = (ULONG)ptr13 >> 16;
        cop1[BPL3 + 2] = (ULONG)ptr13 & 0xffff;
        cop1[BPL4]     = (ULONG)ptr14 >> 16;
        cop1[BPL4 + 2] = (ULONG)ptr14 & 0xffff;
        cop1[BPL5]     = (ULONG)ptr15 >> 16;
        cop1[BPL5 + 2] = (ULONG)ptr15 & 0xffff;
        
        cop2[BPL0]     = (ULONG)ptr20 >> 16;
        cop2[BPL0 + 2] = (ULONG)ptr20 & 0xffff;
        cop2[BPL1]     = (ULONG)ptr21 >> 16;
        cop2[BPL1 + 2] = (ULONG)ptr21 & 0xffff;
        cop2[BPL2]     = (ULONG)ptr22 >> 16;
        cop2[BPL2 + 2] = (ULONG)ptr22 & 0xffff;
        cop2[BPL3]     = (ULONG)ptr23 >> 16;
        cop2[BPL3 + 2] = (ULONG)ptr23 & 0xffff;
        cop2[BPL4]     = (ULONG)ptr24 >> 16;
        cop2[BPL4 + 2] = (ULONG)ptr24 & 0xffff;
        cop2[BPL5]     = (ULONG)ptr25 >> 16;
        cop2[BPL5 + 2] = (ULONG)ptr25 & 0xffff;
        
        
        c2p1x1_6_c5_030_init(320, 180, 0, 0, 40, 7200, 0);
        
        // Point to our initial copper list of double buffer and pointer to next frame bitplanes
        custom->cop1lc = (ULONG)cop1;
        ptr = ptr20;
        
        clearColor = 0;
        
        // Wait VBlank and install VB interrupt
        
        waitVBlank();
        installVBInterrupt();
        
        while ((custom->potinp&0x400) && (cia->ciapra & CIAF_GAMEPORT0)) { 

            k = 0;
            if ((fps&0x3f) < 0x1f) {
                k = random()&3;
                k <<= 4;
                k = (k << 24) | (k << 16) | (k << 8) | k;
            }
            glowline(chunky, 160+(fpsin[(fps<<2)&1023]>>3), 0, 160+(fpcos[(fps<<2)&1023]>>3), 180, 0x3f3f3f3f);
            glowline(chunky, 160+(fpsin[(fps<<1)&1023]>>4), 0, 160+(fpcos[(fps<<1)&1023]>>4), 180, 0x3f3f3f3f-k);
            glowline(chunky, 160+(fpsin[(fps<<3)&1023]>>5), 0, 160+(fpcos[(fps<<3)&1023]>>5), 180, 0x3f3f3f3f);
            glowline(chunky, 160+(fpcos[(fps<<2)&1023]>>3), 0, 160+(fpsin[(fps<<2)&1023]>>3), 180, 0x38383838);
            glowline(chunky, 160+(fpcos[(fps<<1)&1023]>>4), 0, 160+(fpsin[(fps<<1)&1023]>>4), 180, 0x38383838);
            glowline(chunky, 160+(fpcos[(fps<<3)&1023]>>4), 0, 160+(fpsin[(fps<<3)&1023]>>5), 180, 0x38383838-k);
          
            c2p1x1_6_c5_030(chunky, ptr);
            
            // Wait VBlank and swap buffers
            waitVBlank();
            
            frame ^= 1;
            custom->cop1lc = clist[frame];
            ptr = bplptr[frame];
            
            fps++;
            
            // Counters for sync intro scenes
            cvb = vbcounter;
            cvbinc = vbinc;
            vbinc = 0;
            
            // Clear the bitplantes
            ptrl = ((ULONG*)chunky);
            for (i = 0; i < 14400; i++) {
                *ptrl++ = clearColor;
            }
        }

    
        removeVBInterrupt();

    
        // restore DMA
        custom->dmacon = 0x7fff;
        custom->dmacon = oldDMA | DMAF_SETCLR | DMAF_MASTER;


        // restore original copper
        custom->cop1lc = (ULONG) oldCopinit;

        // restore interrupts
        custom->intena = oldInt | 0xc000;
        
        Permit();
        
        // Remove input handler
        removeInputHandler();
        
        LoadView(oldview);
        WaitTOF();
        WaitTOF();
       
    }
    else
        printf("Not enough memory\n");

    FreeMem(ptr10, ((WIDTH * HEIGHT) >> 3) * 6);
    FreeMem(ptr20, ((WIDTH * HEIGHT) >> 3) * 6);
    FreeMem(chunky, 57600);
  
    FreeMem(cop1, sizeof(coplist));
    FreeMem(cop2, sizeof(coplist));
 
    closeMathLibs();
    CloseLibrary((struct Library *)GfxBase);
    CloseLibrary((struct Library *)IntuitionBase);

    if (vbcounter > 0)
        printf("fps %2.2f\n",fps*50.0/vbcounter);
   return 0;
}

