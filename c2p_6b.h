
#ifndef _C2P_H
#define	_C2P_H


void __asm c2p1x1_6_c5_030_smcinit( register __d0 ULONG chunky_x,
                                    register __d1 ULONG chunky_y,
                                    register __d2 ULONG offset_x,
                                    register __d3 ULONG offset_y,
                                    register __d4 ULONG rowlen,
                                    register __d5 ULONG bplsize,
                                    register __d6 ULONG chunkylen);


void __asm c2p1x1_6_c5_030_init(register __d0 ULONG chunky_x,
                                register __d1 ULONG chunky_y,
                                register __d2 ULONG offset_x,
                                register __d3 ULONG offset_y,
                                register __d4 ULONG rowlen,
                                register __d5 ULONG bplsize,
                                register __d6 ULONG chunkylen);


void __asm c2p1x1_6_c5_030(register __a0 UBYTE *chunky_buffer,
						   register __a1 UBYTE *bitplanes);


#endif
