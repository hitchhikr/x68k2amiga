# --------------------------------
# x68k2amiga v2.5
# Written by Franck "hitchhikr" Charlet.
# --------------------------------

# --------------------------------
_LIB_VERSION    =       0x14
_AttnFlags      =       0x128
_LVOCacheClearU =       -636

# --------------------------------
                .global _depack_lzx_0_42
                .global _depack_lzx_1_04
                .global _depack_lzx_unk
                .global _depack_lzx_0_31
                .global _depack_capcom_1
                .global _depack_capcom_2
                .global _depack_valis2
                .global _depack_lzp
                .global _flush_cache

# --------------------------------
# depacker data offset
# packed source
# depacked dest
# depacked reloc infos
# --------------------------------
_depack_lzx_1_04:
                movem.l d0-d7/a0-a6,-(a7)
                movem.l 4+(8*4)+(7*4)(a7),d7/a0/a2/a3
                lea     (a0),a4
                move.l  8(a0),d2
                add.l   d2,a4
                lea     (a2),a5
                lea     (a3),a2
                move.l  d7,d0
                add.l   #0xde,d0
                lea     (a4,d0.l),a6
                move.l  d7,d0
                add.l   #0xa6,d0
                add.w   (a4,d0.l),a6
                bsr     _flush_cache
                moveq   #0,d1
                moveq   #0,d2
                moveq   #0,d3
                moveq   #0,d4
                moveq   #0,d5
                moveq   #0,d6
                move.l  d7,d0
                add.l   #0x5a,d0
                move.l  (a4,d0.l),d2
                move.b  (a6)+,d6
                move.l  d7,d0
                add.l   #0xd9,d0
                move.b  (a4,d0.l),d5
                moveq   #0,d7
                moveq   #0,d0
                bsr.b   depack_lzx_1_04
                bsr     _flush_cache
                movem.l (a7)+,d0-d7/a0-a6
                rts
depack_lzx_1_04:
                subq.w  #1,d5
                bcc.b   lb_0005490e
                move.b  (a6)+,d6
                moveq   #7,d5
lb_0005490e:
                add.b   d6,d6
                bcc.b   lb_00054916
                move.b  (a6)+,(a5)+
                bra.b   depack_lzx_1_04
lb_00054916:
                moveq   #-1,d3
                moveq   #0,d4
                subq.w  #1,d5
                bcc.b   lb_00054922
                move.b  (a6)+,d6
                moveq   #7,d5
lb_00054922:
                add.b   d6,d6
                bcs.b   lb_0005494e
                subq.w  #1,d5
                bcc.b   lb_0005492e
                move.b  (a6)+,d6
                moveq   #7,d5
lb_0005492e:
                add.b   d6,d6
                addx.w  d4,d4
                subq.w  #1,d5
                bcc.b   lb_0005493a
                move.b  (a6)+,d6
                moveq   #7,d5
lb_0005493a:
                add.b   d6,d6
                addx.w  d4,d4
                move.b  (a6)+,d3
lb_00054940:
                addq.w  #1,d4
lb_00054942:
                lea     (a5,d3.w),a4
lb_00054946:
                move.b  (a4)+,(a5)+
                dbf     d4,lb_00054946
                bra.b   depack_lzx_1_04
lb_0005494e:
                move.b  (a6)+,d3
                asl.w   #8,d3
                move.b  (a6)+,d3
                move.b  d3,d4
                asr.l   #3,d3
                and.w   #7,d4
                bne.b   lb_00054940
                move.b  (a6)+,d4
                bne.b   lb_00054942
                tst.l   d2
                beq.b   lb_00054986
                move.l  d7,a4
                moveq   #0,d6
                move.b  (a6)+,d4
lb_0005496c:
                moveq   #0,d5
lb_0005496e:
                lsr.b   #1,d4
                bcc.b   lb_0005497a
                addx.l  d4,d5
                asl.l   #7,d5
                move.b  (a6)+,d4
                bra.b   lb_0005496e
lb_0005497a:
                add.l   d4,d5
                add.l   d5,d6
                move.l  d6,(a2)+
                move.b  (a6)+,d4
                bne.b   lb_0005496c
lb_00054986:
                move.l  #-1,(a2)
                rts

# --------------------------------
# depacker data offset
# packed source
# depacked dest
# depacked reloc infos
# --------------------------------
_depack_lzx_0_42:
                movem.l d0-d7/a0-a6,-(a7)
                movem.l 4+(8*4)+(7*4)(a7),d7/a0/a2/a3
                lea     (a0),a4
                move.l  8(a0),d2
                add.l   d2,a4
                lea     (a2),a5
                lea     (a3),a2
                move.l  d7,d0
                add.l   #0xe0,d0
                lea     (a4,d0.l),a6
                bsr     _flush_cache
                moveq   #0,d1
                moveq   #0,d2
                moveq   #0,d3
                moveq   #0,d4
                moveq   #0,d5
                moveq   #0,d6
                move.l  d7,d0
                add.l   #0x5a,d0
                move.l  (a4,d0.l),d2
                move.b  (a6)+,d6
                move.l  d7,d0
                add.l   #0xc7,d0
                move.b  (a4,d0.l),d5
                moveq   #0,d7
                moveq   #0,d0
                bsr.b   lb_00056288
                bsr     _flush_cache
                movem.l (a7)+,d0-d7/a0-a6
                rts
depack_lzx_0_42:
                subq.w  #1,d5
                bcc.b   lb_0005626a
                move.b  (a6)+,d6
                moveq   #7,d5
lb_0005626a:
                add.b   d6,d6
                addx.w  d4,d4
                subq.w  #1,d5
                bcc.b   lb_00056276
                move.b  (a6)+,d6
                moveq   #7,d5
lb_00056276:    
                add.b   d6,d6
                addx.w  d4,d4
                move.b  (a6)+,d3
lb_0005627c:
                addq.w  #1,d4
lb_0005627e:    
                lea     (a5,d3.w),a4
lb_00056282:
                move.b  (a4)+,(a5)+
                dbf     d4,lb_00056282
lb_00056288:
                subq.w  #1,d5
                bcc.b   lb_00056290
                move.b  (a6)+,d6
                moveq   #7,d5
lb_00056290:
                add.b   d6,d6
                bcc.b   lb_00056298
                move.b  (a6)+,(a5)+
                bra.b   lb_00056288
lb_00056298:
                moveq   #-1,d3
                moveq   #0,d4
                subq.w  #1,d5
                bcc.b   lb_000562a4
                move.b  (a6)+,d6
                moveq   #7,d5
lb_000562a4:
                add.b   d6,d6
                bcc.b   depack_lzx_0_42
                move.b  (a6)+,d3
                asl.w   #8,d3
                move.b  (a6)+,d3
                move.b  d3,d4
                asr.l   #3,d3
                and.w   #7,d4
                bne.b   lb_0005627c
                move.b  (a6)+,d4
                bne.b   lb_0005627e
                tst.l   d2
                beq.b   lb_000562e4
                move.l  d7,a4
                moveq   #0,d6
                move.b  (a6)+,d4
lb_000562c6:
                moveq   #0,d5
lb_000562c8:
                lsr.b   #1,d4
                bcc.b   lb_000562d4
                addx.l  d4,d5
                asl.l   #7,d5
                move.b  (a6)+,d4
                bra.b   lb_000562c8
lb_000562d4:
                add.l   d4,d5
                add.l   d5,d6
                move.l  d6,(a2)+
                move.b  (a6)+,d4
                bne.b   lb_000562c6
lb_000562e4:
                move.l  #-1,(a2)
                rts

# --------------------------------
# depacker data offset
# packed source
# depacked dest
# depacked reloc infos
# --------------------------------
_depack_lzx_unk:
                movem.l d0-d7/a0-a6,-(a7)
                movem.l 4+(8*4)+(7*4)(a7),d7/a0/a2/a3
                lea     (a0),a4
                move.l  8(a0),d2
                add.l   d2,a4
                lea     (a2),a5
                lea     (a3),a2
                move.l  d7,d0
                add.l   #0x76,d0
                lea     (a4,d0.l),a6
                move.l  0xc(a0),d2
                sub.l   #0xde,d2
                add.l   d2,d7
                move.l  d7,d0
                add.l   #0x40,d0
                lea     (a4,d0.l),a3
                bsr     _flush_cache
                moveq   #0,d0
                moveq   #0,d1
                moveq   #0,d2
                moveq   #0,d3
                moveq   #0,d4
                moveq   #0,d5
                moveq   #0,d6
                moveq   #0,d7
                bsr.b   depack_lzx_unk
                bsr     _flush_cache
                movem.l (a7)+,d0-d7/a0-a6
                rts
depack_lzx_unk:
                move.l  a5,d7
                move.b  (a6)+,d6
                moveq   #8,d5
lb_00008c08:
                subq.w  #1,d5
                bcc.b   lb_00008c10
                move.b  (a6)+,d6
                moveq   #7,d5
lb_00008c10:
                lsl.b   #1,d6
                bcc.b   lb_00008c18
                move.b  (a6)+,(a5)+
                bra.b   lb_00008c08
lb_00008c18:
                moveq   #-1,d3
                clr.w   d4
                subq.w  #1,d5
                bcc.b   lb_00008c24
                move.b  (a6)+,d6
                moveq   #7,d5
lb_00008c24:          
                lsl.b   #1,d6
                bcs.b   lb_00008c46
                subq.w  #1,d5
                bcc.b   lb_00008c30
                move.b  (a6)+,d6
                moveq   #7,d5
lb_00008c30:
                lsl.b   #1,d6
                roxl.w  #1,d4
                subq.w  #1,d5
                bcc.b   lb_00008c3c
                move.b  (a6)+,d6
                moveq   #7,d5
lb_00008c3c:
                lsl.b   #1,d6
                roxl.w  #1,d4
                addq.w  #1,d4
                move.b  (a6)+,d3
                bra.b   lb_00008c60
lb_00008c46:
                move.b  (a6)+,d3
                asl.w   #5,d3
                move.b  (a6)+,d2
                move.b  d2,d4
                lsr.b   #3,d2
                or.b    d2,d3
                and.w   #7,d4
                beq.b   lb_00008c5c
                addq.w  #1,d4
                bra.b   lb_00008c60
lb_00008c5c:
                move.b  (a6)+,d4
                beq.b   lb_00008c7e
lb_00008c60:      
                lea     (a5,d3.w),a4
lb_00008c64:      
                move.b  (a4)+,(a5)+
                dbra    d4,lb_00008c64
                cmpa.l  a6,a3
                bcc.b   lb_00008c08
                rts
lb_00008c7e:
                movea.l d7,a4
                moveq   #0,d4
                moveq   #0,d6
lb_00008c84:
                moveq   #0,d5
                move.b  (a6)+,d4
                beq.b   lb_00008ca0
lb_00008c8a:
                lsr.b   #1,d4
                bcc.b   lb_00008c96
                addx.l  d4,d5
                asl.l   #7,d5
                move.b  (a6)+,d4
                bra.b   lb_00008c8a
lb_00008c96:
                add.l   d4,d5
                add.l   d5,d6
                move.l  d6,(a2)+
                bra.b   lb_00008c84
lb_00008ca0:
                move.l  #-1,(a2)
                rts


# --------------------------------
# depacker data offset
# packed source
# depacked dest
# depacked reloc infos
# --------------------------------
_depack_lzx_0_31:
                movem.l d0-d7/a0-a6,-(a7)
                movem.l 4+(8*4)+(7*4)(a7),d7/a0/a2/a3
                lea     (a0),a4
                move.l  8(a0),d2
                add.l   d2,a4
                lea     (a2),a5
                lea     (a3),a2
                move.l  d7,d0
                add.l   #0xb6,d0
                lea     (a4,d0.l),a6
                bsr     _flush_cache
                moveq   #0,d0
                moveq   #0,d1
                moveq   #0,d2
                moveq   #0,d3
                moveq   #0,d4
                moveq   #0,d5
                moveq   #0,d6
                moveq   #0,d7
                bsr.b   depack_lzx_0_31
                bsr     _flush_cache
                movem.l (a7)+,d0-d7/a0-a6
                rts
depack_lzx_0_31:
                move.l  a5,d7
                move.b  (a6)+,d6
                moveq   #8,d5
lb_0006c43c:
                subq.w  #1,d5
                bcc.b   lb_0006c444
                move.b  (a6)+,d6
                moveq   #7,d5
lb_0006c444:
                add.b   d6,d6
                bcc.b   lb_0006c44c
                move.b  (a6)+,(a5)+
                bra.b   lb_0006c43c
lb_0006c44c:
                moveq   #-1,d3
                clr.w   d4
                subq.w  #1,d5
                bcc.b   lb_0006c458
                move.b  (a6)+,d6
                moveq   #7,d5
lb_0006c458:
                add.b   d6,d6
                bcs.b   lb_0006c47a
                subq.w  #1,d5
                bcc.b   lb_0006c464
                move.b  (a6)+,d6
                moveq   #7,d5
lb_0006c464:
                add.b   d6,d6
                addx.w  d4,d4
                subq.w  #1,d5
                bcc.b   lb_0006c470
                move.b  (a6)+,d6
                moveq   #7,d5
lb_0006c470:
                add.b   d6,d6
                addx.w  d4,d4
                addq.w  #1,d4
                move.b  (a6)+,d3
                bra.b   lb_0006c494
lb_0006c47a:
                move.b  (a6)+,d3
                asl.w   #5,d3
                move.b  (a6)+,d2
                move.b  d2,d4
                lsr.b   #3,d2
                or.b    d2,d3
                and.w   #7,d4
                beq.b   lb_0006c490
                addq.w  #1,d4
                bra.b   lb_0006c494
lb_0006c490:
                move.b  (a6)+,d4
                beq.b   lb_0006c4a0
lb_0006c494:
                lea     (a5,d3.w),a4
lb_0006c498:
                move.b  (a4)+,(a5)+
                dbf     d4,lb_0006c498
                bra.b   lb_0006c43c
lb_0006c4a0:
                move.l  d7,a4
                moveq   #0,d4
                moveq   #0,d6
                move.b  (a6)+,d4
lb_0006c4ac:
                moveq   #0,d5
lb_0006c4ae:
                lsr.b   #1,d4
                bcc.b   lb_0006c4ba
                addx.l  d4,d5
                asl.l   #7,d5
                move.b  (a6)+,d4
                bra.b   lb_0006c4ae
lb_0006c4ba:
                add.l   d4,d5
                add.l   d5,d6
                move.l  d6,(a2)+
                move.b  (a6)+,d4
                bne.b   lb_0006c4ac
                move.l  #-1,(a2)
                rts

# --------------------------------
# packed size
# packed source
# temp buffer
# --------------------------------
# returns depacked size
# --------------------------------
_depack_lzp:
                movem.l d1-d7/a0-a6,-(a7)
                movem.l 4+(7*4)+(7*4)(a7),d0/a1/a6
                bsr     _flush_cache
                moveq   #0,d1
                moveq   #0,d2
                moveq   #0,d3
                moveq   #0,d4
                moveq   #0,d5
                moveq   #0,d6
                moveq   #0,d7
                bsr.b   depack_lzp
                bsr     _flush_cache
                movem.l (a7)+,d1-d7/a0-a6
                rts
depack_lzp:      
                movem.l d1-d6/a0-a6,-(a7)
                movea.l a1,a2
                move.l  a2,-(a7)
                move.w  d0,d4
                move.w  d0,d3
                subq.w  #4,d4
                addq.l  #4,a2
                movea.l a6,a1
                moveq   #0,d1
                moveq   #128-1,d0
lb_00003D4E:      
                move.l  d1,(a1)+
                move.l  d1,(a1)+
                move.l  d1,(a1)+
                move.l  d1,(a1)+
                move.l  d1,(a1)+
                move.l  d1,(a1)+
                move.l  d1,(a1)+
                move.l  d1,(a1)+
                dbra    d0,lb_00003D4E
lb_00003D62:      
                cmpi.w  #256,d4
                bcs.b   lb_00003D86
                moveq   #8-1,d0
lb_00003D6A:      
                move.l  (a2)+,(a1)+
                move.l  (a2)+,(a1)+
                move.l  (a2)+,(a1)+
                move.l  (a2)+,(a1)+
                move.l  (a2)+,(a1)+
                move.l  (a2)+,(a1)+
                move.l  (a2)+,(a1)+
                move.l  (a2)+,(a1)+
                dbra    d0,lb_00003D6A
                subi.w  #256,d4
                beq.b   lb_00003D8C
                bra.b   lb_00003D62
lb_00003D86:      
                move.b  (a2)+,(a1)+
                subq.w  #1,d4
                bne.b   lb_00003D86
lb_00003D8C:      
                move.w  #0xFEE,d2
                moveq   #0,d4
                move.w  d3,d4
                moveq   #0,d3
                subq.w  #4,d4
                addq.l  #4,a2
                moveq   #0,d6
                movea.l a6,a0
                lea     (4096,a0),a2
                movea.l (a7),a1
lb_00003DAA:      
                lsr.w   #1,d3
                btst    #8,d3
                bne.b   lb_00003DBA
                bsr.b   lb_00003E12
                move.w  #0xFF00,d3
                move.b  d0,d3
lb_00003DBA:      
                btst    #0,d3
                beq.b   lb_00003DD2
                bsr.b   lb_00003E12
                move.b  d0,(a1)+
                addq.w  #1,d6
                move.b  d0,(a0,d2.w)
                addq.w  #1,d2
                andi.w  #0xFFF,d2
                bra.b   lb_00003DAA
lb_00003DD2:      
                bsr.b   lb_00003E12
                moveq   #0,d1
                move.b  d0,d1
                bsr.b   lb_00003E12
                move.w  d0,-(a7)
                andi.b  #0xF0,d0
                lsl.w   #4,d0
                or.w    d0,d1
                move.w  (a7)+,d0
                andi.b  #15,d0
                addq.w  #2,d0
                moveq   #0,d5
lb_00003DEE:      
                move.w  d5,-(a7)
                add.w   d1,d5
                andi.w  #0xFFF,d5
                move.b  (a0,d5.w),(a1)+
                addq.w  #1,d6
                move.b  (a0,d5.w),(a0,d2.w)
                addq.w  #1,d2
                andi.w  #0xFFF,d2
                move.w  (a7)+,d5
                addq.w  #1,d5
                cmp.w   d0,d5
                bls.b   lb_00003DEE
                bra.b   lb_00003DAA
lb_00003E12:      
                tst.w   d4
                beq.b   lb_00003E1E
                moveq   #0,d0
                move.b  (a2)+,d0
                subq.w  #1,d4
                rts
lb_00003E1E:      
                addq.l  #4,a7
                movea.l (a7)+,a2
                move.l  a1,d0
                sub.l   a2,d0
                movem.l (a7)+,d1-d6/a0-a6
                rts

# --------------------------------
# packed length
# packed source
# depacked dest
# depacked reloc offset 
# bss size
# depacked reloc infos
# --------------------------------
# returns depacked size
# --------------------------------
_depack_capcom_1:
                movem.l d1-d7/a0-a6,-(a7)
                movem.l 4+(7*4)+(7*4)(a7),d0/d7/a0/a2/a3/a4
                move.l  d7,a6
                move.l  a0,d7
                move.l  a2,a0
                add.l   d7,a0
                add.l   a0,a3
                move.l  a4,a2
                bsr     _flush_cache
                bsr.b   depack_capcom_1
                bsr     _flush_cache
                movem.l (a7)+,d1-d7/a0-a6
                rts
depack_capcom_1:
                move.l  d7,a5
                moveq   #-1,d1
lb_0007a7c8:
                add.w   d1,d1
                bcc.b   lb_0007a7d6
                move.w  #0x100,d1
                subq.l  #1,d0
                bcs.b   lb_0007a802
                move.b  (a6)+,d1
lb_0007a7d6:
                tst.b   d1
                bpl.b   lb_0007a7e0
                subq.l  #1,d0
                move.b  (a6)+,(a5)+
                bra.b   lb_0007a7c8
lb_0007a7e0:
                subq.l  #2,d0
                bcs.b   lb_0007a802
                clr.w   d2
                move.b  (a6)+,d2
                move.w  d2,d3
                andi.w  #0xf,d3
                lsl.w   #4,d2
                move.b  (a6)+,d2
                move.l  a5,a1
                suba.w  d2,a1
                move.b  (a1)+,(a5)+
                move.b  (a1)+,(a5)+
lb_0007a7fa:
                move.b  (a1)+,(a5)+
                dbf     d3,lb_0007a7fa
                bra.b   lb_0007a7c8
lb_0007a802:
                sub.l   a1,a1
                lea     (a0),a6
                bra.b   lb_0007a81c
lb_0007a80a:
                moveq   #0,d0
lb_0007a80c:
                move.b  (a6),d0
                clr.b   (a6)+
                add.b   d0,d0
                bcs.b   lb_0007a818
                lsl.l   #7,d0
                bra.b   lb_0007a80c
lb_0007a818:
                adda.l  d0,a1
                move.l  a1,(a2)+
lb_0007a81c:
                cmpa.l  a5,a6
                bcs.b   lb_0007a80a
                move.l  #-1,(a2)
                lea     (a3),a1
lb_0007a824:
                clr.b   (a6)+
                cmpa.l  a1,a6
                bcs.b   lb_0007a824
                move.l  a6,d0
                sub.l   d7,d0
                rts

# --------------------------------
# packed length
# packed source
# depacked dest
# depacked reloc offset 
# bss size
# depacked reloc infos
# --------------------------------
# returns depacked size
# --------------------------------
_depack_capcom_2:
                movem.l d1-d7/a0-a6,-(a7)
                movem.l 4+(7*4)+(7*4)(a7),d0/d7/a0/a2/a3/a4
                move.l  d7,a6
                move.l  a0,d7
                move.l  a2,a0
                add.l   d7,a0
                add.l   a0,a3
                move.l  a4,a2
                bsr     _flush_cache
                bsr.b   depack_capcom_2
                bsr     _flush_cache
                movem.l (a7)+,d1-d7/a0-a6
                rts
depack_capcom_2:
                move.l	d7,a5
                moveq	#0,d4
                bra.b	lb_000639c0
lb_000639be:
                move.b	(a6)+,(a5)+
lb_000639c0:
                dbf	    d4,lb_000639c8
                addq.w	#8,d4
                move.b	(a6)+,d1
lb_000639c8:
                add.b	d1,d1
                bcs.b	lb_000639be
                moveq	#-1,d2
                dbf	    d4,lb_000639d6
                addq.w	#8,d4
                move.b	(a6)+,d1
lb_000639d6:
                add.b	d1,d1
                bcs.b	lb_000639f8
                move.b	(a6)+,d2
                moveq	#7,d3
                and.b	d2,d3
                bne.b	lb_000639e8
                move.b	(a6)+,d3
                beq.b	lb_00063a22
                subq.w	#1,d3
lb_000639e8:
                lsl.w	#5,d2
lb_000639ea:
                move.b	(a6)+,d2
                lea	    (a5,d2.w),a1
lb_000639f0:
                move.b	(a1)+,(a5)+
                dbf	    d3,lb_000639f0
                bra.b	lb_000639c0
lb_000639f8:
                moveq	#0,d3
                dbf	    d4,lb_00063a02
                addq.w	#8,d4
                move.b	(a6)+,d1
lb_00063a02:
                add.b	d1,d1
                addx.b	d3,d3
                dbf	    d4,lb_00063a0e
                addq.w	#8,d4
                move.b	(a6)+,d1
lb_00063a0e:
                add.b	d1,d1
                addx.b	d3,d3
                dbf	    d4,lb_00063a1a
                addq.w	#8,d4
                move.b	(a6)+,d1
lb_00063a1a:
                add.b	d1,d1
                addx.b	d3,d3
                addq.w	#1,d3
                bra.b	lb_000639ea
lb_00063a22:
                sub.l	a1,a1
                lea     (a0),a6
                moveq	#0,d1
                bra.b	lb_00063a3c
lb_00063a2c:
                moveq	#0,d0
lb_00063a2e:
                move.b	(a6)+,d0
                add.b	d0,d0
                bcs.b	lb_00063a38
                lsl.l	#7,d0
                bra.b	lb_00063a2e
lb_00063a38:
                adda.l  d0,a1
                move.l  a1,(a2)+
lb_00063a3c:
                cmpa.l	a5,a6
                bcs.b	lb_00063a2c
                move.l  #-1,(a2)
                lea     (a0),a6
                lea     (a3),a1
                bra.b	lb_00063a4e
lb_00063a4a:
                move.l	d1,(a6)+
                move.l	d1,(a6)+
lb_00063a4e:
                cmpa.l	a1,a6
                bcs.b	lb_00063a4a
                move.l  a6,d0
                sub.l   d7,d0
                rts

# --------------------------------
# depacked length
# packed source
# depacked dest
# temporary buffer
# --------------------------------
# returns depacked size
# --------------------------------
_depack_valis2:
                movem.l d0-d7/a0-a6,-(a7)
                movem.l 4+(8*4)+(7*4)(a7),d0/a1/a2/a3
                lea     (a3),a0
                bsr     _flush_cache
                moveq   #0,d1
                moveq   #0,d2
                moveq   #0,d3
                moveq   #0,d4
                moveq   #0,d5
                moveq   #0,d6
                moveq   #0,d7
                bsr.b   depack_valis2
                bsr     _flush_cache
                movem.l (a7)+,d0-d7/a0-a6
                rts
depack_valis2:
                lea      (a2,d0.l),a3
                move.w   #0xF,d4
                move.w   #0xFFF,d5
                move.w   #0xFEE,d6
lb_0000A40:
                move.b   (a1)+,d7
                lsr.b    #1,d7
                bcc.b    lb_0000A54
                move.b   (a1)+,d1
                move.b   d1,(a2)+
                move.b   d1,(a0,d6.w)
                addq.w   #1,d6
                and.w    d5,d6
                bra.b    lb_0000A7C
lb_0000A54:
                move.b   (1,a1),d1
                move.b   d1,d2
                and.w    d4,d2
                addq.w   #2,d2
                lsl.w    #4,d1
                move.b   (a1),d1
                and.w    d5,d1
                addq.w   #2,a1
lb_0000A66:
                move.b   (a0,d1.w),d3
                addq.w   #1,d1
                and.w    d5,d1
                move.b   d3,(a2)+
                move.b   d3,(a0,d6.w)
                addq.w   #1,d6
                and.w    d5,d6
                dbra     d2,lb_0000A66
lb_0000A7C:
                lsr.b    #1,d7
                bcc.b    lb_0000A8E
                move.b   (a1)+,d1
                move.b   d1,(a2)+
                move.b   d1,(a0,d6.w)
                addq.w   #1,d6
                and.w    d5,d6
                bra.b    lb_0000AB6
lb_0000A8E:
                move.b   (1,a1),d1
                move.b   d1,d2
                and.w    d4,d2
                addq.w   #2,d2
                lsl.w    #4,d1
                move.b   (a1),d1
                and.w    d5,d1
                addq.w   #2,a1
lb_0000AA0:
                move.b   (a0,d1.w),d3
                addq.w   #1,d1
                and.w    d5,d1
                move.b   d3,(a2)+
                move.b   d3,(a0,d6.w)
                addq.w   #1,d6
                and.w    d5,d6
                dbra     d2,lb_0000AA0
lb_0000AB6:
                lsr.b    #1,d7
                bcc.b    lb_0000AC8
                move.b   (a1)+,d1
                move.b   d1,(a2)+
                move.b   d1,(a0,d6.w)
                addq.w   #1,d6
                and.w    d5,d6
                bra.b    lb_0000AF0
lb_0000AC8:
                move.b   (1,a1),d1
                move.b   d1,d2
                and.w    d4,d2
                addq.w   #2,d2
                lsl.w    #4,d1
                move.b   (a1),d1
                and.w    d5,d1
                addq.w   #2,a1
lb_0000ADA:
                move.b   (a0,d1.w),d3
                addq.w   #1,d1
                and.w    d5,d1
                move.b   d3,(a2)+
                move.b   d3,(a0,d6.w)
                addq.w   #1,d6
                and.w    d5,d6
                dbra     d2,lb_0000ADA
lb_0000AF0:
                lsr.b    #1,d7
                bcc.b    lb_0000B02
                move.b   (a1)+,d1
                move.b   d1,(a2)+
                move.b   d1,(a0,d6.w)
                addq.w   #1,d6
                and.w    d5,d6
                bra.b    lb_0000B2A
lb_0000B02:
                move.b   (1,a1),d1
                move.b   d1,d2
                and.w    d4,d2
                addq.w   #2,d2
                lsl.w    #4,d1
                move.b   (a1),d1
                and.w    d5,d1
                addq.w   #2,a1
lb_0000B14:
                move.b   (a0,d1.w),d3
                addq.w   #1,d1
                and.w    d5,d1
                move.b   d3,(a2)+
                move.b   d3,(a0,d6.w)
                addq.w   #1,d6
                and.w    d5,d6
                dbra     d2,lb_0000B14
lb_0000B2A:
                lsr.b    #1,d7
                bcc.b    lb_0000B3C
                move.b   (a1)+,d1
                move.b   d1,(a2)+
                move.b   d1,(a0,d6.w)
                addq.w   #1,d6
                and.w    d5,d6
                bra.b    lb_0000B64
lb_0000B3C:
                move.b   (1,a1),d1
                move.b   d1,d2
                and.w    d4,d2
                addq.w   #2,d2
                lsl.w    #4,d1
                move.b   (a1),d1
                and.w    d5,d1
                addq.w   #2,a1
lb_0000B4E:
                move.b   (a0,d1.w),d3
                addq.w   #1,d1
                and.w    d5,d1
                move.b   d3,(a2)+
                move.b   d3,(a0,d6.w)
                addq.w   #1,d6
                and.w    d5,d6
                dbra     d2,lb_0000B4E
lb_0000B64:
                lsr.b    #1,d7
                bcc.b    lb_0000B76
                move.b   (a1)+,d1
                move.b   d1,(a2)+
                move.b   d1,(a0,d6.w)
                addq.w   #1,d6
                and.w    d5,d6
                bra.b    lb_0000B9E
lb_0000B76:
                move.b   (1,a1),d1
                move.b   d1,d2
                and.w    d4,d2
                addq.w   #2,d2
                lsl.w    #4,d1
                move.b   (a1),d1
                and.w    d5,d1
                addq.w   #2,a1
lb_0000B88:
                move.b   (a0,d1.w),d3
                addq.w   #1,d1
                and.w    d5,d1
                move.b   d3,(a2)+
                move.b   d3,(a0,d6.w)
                addq.w   #1,d6
                and.w    d5,d6
                dbra     d2,lb_0000B88
lb_0000B9E:
                lsr.b    #1,d7
                bcc.b    lb_0000BB0
                move.b   (a1)+,d1
                move.b   d1,(a2)+
                move.b   d1,(a0,d6.w)
                addq.w   #1,d6
                and.w    d5,d6
                bra.b    lb_0000BD8
lb_0000BB0:
                move.b   (1,a1),d1
                move.b   d1,d2
                and.w    d4,d2
                addq.w   #2,d2
                lsl.w    #4,d1
                move.b   (a1),d1
                and.w    d5,d1
                addq.w   #2,a1
lb_0000BC2:
                move.b   (a0,d1.w),d3
                addq.w   #1,d1
                and.w    d5,d1
                move.b   d3,(a2)+
                move.b   d3,(a0,d6.w)
                addq.w   #1,d6
                and.w    d5,d6
                dbra     d2,lb_0000BC2
lb_0000BD8:
                lsr.b    #1,d7
                bcc.b    lb_0000BEA
                move.b   (a1)+,d1
                move.b   d1,(a2)+
                move.b   d1,(a0,d6.w)
                addq.w   #1,d6
                and.w    d5,d6
                bra.b    lb_0000C12
lb_0000BEA:
                move.b   (1,a1),d1
                move.b   d1,d2
                and.w    d4,d2
                addq.w   #2,d2
                lsl.w    #4,d1
                move.b   (a1),d1
                and.w    d5,d1
                addq.w   #2,a1
lb_0000BFC:
                move.b   (a0,d1.w),d3
                addq.w   #1,d1
                and.w    d5,d1
                move.b   d3,(a2)+
                move.b   d3,(a0,d6.w)
                addq.w   #1,d6
                and.w    d5,d6
                dbra     d2,lb_0000BFC
lb_0000C12:
                cmpa.l   a3,a2
                bcs.w    lb_0000A40
                rts

# --------------------------------
_flush_cache:
                movem.l d0-d7/a0-a6,-(a7)
                move.l  4.w,a6
                move.b  _AttnFlags+1(a6),d1
                btst.l  #1,d1
                beq.b   no_68020
                cmp.w   #37,_LIB_VERSION(a6)
                blt.b   no_68020
                jsr     _LVOCacheClearU(a6)
no_68020:
                movem.l (a7)+,d0-d7/a0-a6
                rts
