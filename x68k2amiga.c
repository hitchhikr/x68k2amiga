/* --------------------------------
   x68k2amiga v2.5
   Written by Franck "hitchhikr" Charlet.
   -------------------------------- */

/* --------------------------------
   includes
   -------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <proto/dos.h>

/* --------------------------------
   constants
   -------------------------------- */
#define MAGIC_X 0x4855
#define MAGIC_Z 0x601a

#define CODE_SIZE 0
#define DATA_SIZE 1
#define BSS_SIZE 2
#define RELOC_SIZE 3
#define SYM_SIZE 4

#define DEC_NUM "\2335;32m%d\2330;31m"
#define HEX_NUM "\2335;32m$%lx\2330;31m"
#define STRING "\2335;32m%s\2330;31m"

/* --------------------------------
   functions
   -------------------------------- */
void depack_lzx_0_42(unsigned long offset, unsigned char *source, unsigned char *dest, unsigned long *relocs);
void depack_lzx_1_04(unsigned long offset, unsigned char *source, unsigned char *dest, unsigned long *relocs);
void depack_lzx_unk(unsigned long offset, unsigned char *source, unsigned char *dest, unsigned long *relocs);
void depack_lzx_0_31(unsigned long offset, unsigned char *source, unsigned char *dest, unsigned long *relocs);
int depack_capcom_1(unsigned long size, unsigned char *source, unsigned char *dest, unsigned long offset_relocs, unsigned long size_bss, unsigned long *relocs);
int depack_capcom_2(unsigned long size, unsigned char *source, unsigned char *dest, unsigned long offset_relocs, unsigned long size_bss, unsigned long *relocs);
void depack_valis2(unsigned long depacked_length, unsigned char *source, unsigned char *dest, unsigned char *temp);
int depack_lzp(unsigned long size, unsigned char *source, unsigned char *temp);
void flush_cache();

/* --------------------------------
   enumerations
   -------------------------------- */
enum packer
{
    PACK_NONE, LZX_0_31, LZX_0_42, LZX_1_04, LZX_UNK, LZX_CAPCOM_1, LZX_CAPCOM_2, PACK_VALIS2, LZP
};

/* --------------------------------
   structures
   -------------------------------- */
struct x68_x_header
{
    unsigned short magic;           // 0x4855
    unsigned char reserved1;        // 0
    unsigned char loadmode;         // 0 = normal
                                    // 1 = minimal memory
                                    // 2 = high address
    unsigned long base;             // base address (0)
    unsigned long entrypoint;
    unsigned long size[5];          // code, data, bss, reloc, symbols
	unsigned long db_line;	        // size of debugging info (line #)
	unsigned long db_syms;	        // size of debugging info (symbol)
	unsigned long db_str;	        // size of debugging info (string)
	unsigned long reserved2[4];	    // 0
	unsigned long bindlist;	        // bind list offset
} __attribute__((packed));          // 64/0x40 bytes

struct x68_z_header
{
	unsigned short magic;           // 0x601a
	unsigned long code;
	unsigned long data;
	unsigned long bss;
	unsigned char reserved[8];
	unsigned long base;
	unsigned short padding;
} __attribute__((packed));          // 28/0x1c bytes

struct symbol
{
    unsigned char location;         // 0 = external 2 = local
    unsigned char segment;          // 1 = TEXT
                                    // 2 = DATA
                                    // 3 = BSS
                                    //
    unsigned long offset;           // (Always from code segment start)
} __attribute__((packed));          // 6 bytes

/* --------------------------------
   variables
   -------------------------------- */
char sym_name[512];
char multiple_name[512];
char source_name[512];
char dest_name[512];
char reconstr_name[512];
unsigned char sar_name[16];
unsigned char packer_sign[180 + 8];
FILE *inf;
FILE *outf;
FILE *recf;
FILE *multiplef;
unsigned char *src = 0;
unsigned char *mem_code = 0;
unsigned char *mem_data = 0;
unsigned long *depacked_relocs = 0;
unsigned char *temp_depack_buffer = 0;
struct AnchorPath AP;
struct x68_x_header rec_head;

/* --------------------------------
   free all allocated resources
   -------------------------------- */
void free_stuff()
{
    if(temp_depack_buffer) free(temp_depack_buffer);
    temp_depack_buffer = NULL;
    if(depacked_relocs) free(depacked_relocs);
    depacked_relocs = NULL;
    if(src) free(src);
    src = NULL;
    if(mem_data) free(mem_data);
    mem_data = NULL;
    if(mem_code) free(mem_code);
    mem_code = NULL;
    if(inf) fclose(inf);
    inf = NULL;
    if(outf) fclose(outf);
    outf = NULL;
    if(recf) fclose(recf);
    recf = NULL;
}

/* --------------------------------
   exit & clean everything
   -------------------------------- */
void exit_func()
{
    MatchEnd(&AP);
    free_stuff();
}

/* --------------------------------
   get a segment number from it's offset in the file
   -------------------------------- */
int get_segment_number(struct x68_x_header *ah, unsigned long offset)
{
    if(offset >= 0 && offset < ah->size[CODE_SIZE])
    {
        return 0;
    }
    if(offset >= ah->size[CODE_SIZE] && offset < (ah->size[CODE_SIZE] + ah->size[DATA_SIZE]))
    {
        return 1;
    }
    if(offset >= (ah->size[CODE_SIZE] + ah->size[DATA_SIZE]) && offset < (ah->size[CODE_SIZE] + ah->size[DATA_SIZE] + ah->size[BSS_SIZE]))
    {
        return 2;
    }
    return 0;
}

/* --------------------------------
   get a segment offset in the file from it's number
   -------------------------------- */
unsigned long get_segment_base_address(struct x68_x_header *ah, int segment)
{
    switch(segment)
    {
        case 0:
            return 0;
        case 1:
            return ah->size[CODE_SIZE];
        case 2:
            return ah->size[CODE_SIZE] + ah->size[DATA_SIZE];
    }
    return 0;
}

/* --------------------------------
   write the data of a reloc hunk
   -------------------------------- */
int write_reloc_hunk_data(unsigned char *mem_block, struct x68_x_header *ah, int hunk_number,
                          int dest_hunk, int written_hunk, int relocs_count, int base_seg)
{
    unsigned long x68_reloc_size;
    int r;
    unsigned short reloc_offset;
    unsigned long reloc_offset_long;
    unsigned long l;
    unsigned short first_short;
    unsigned long first_long;
    unsigned long rel_long;
    int was_long_jump;
    int cur_segment;

    if(relocs_count)
    {
        // go to relocations segment
        fseek(inf, sizeof(struct x68_x_header) + ah->size[CODE_SIZE] + ah->size[DATA_SIZE], SEEK_SET);
        r = fread((char *) &first_short, 1, sizeof(short), inf);
        if(r != sizeof(short))
        {
            return 0;
        }
        was_long_jump = 0;
        first_long = first_short;
        fwrite((char *) &relocs_count, sizeof(long), 1, outf);
        l = written_hunk;
        fwrite((char *) &l, sizeof(long), 1, outf);
        // write code relocs
        x68_reloc_size = ah->size[RELOC_SIZE];
        while(x68_reloc_size)
        {
            x68_reloc_size -= sizeof(short);
            if(was_long_jump)
            {
                x68_reloc_size -= sizeof(unsigned long);
                was_long_jump = 0;
            }
            // reloc is located in this segment
            if(hunk_number == get_segment_number(ah, first_long))
            {
                cur_segment = get_segment_number(ah, *((unsigned long *) (mem_block + first_long)));
                // reloc is pointing to the asked segment
                if(cur_segment == dest_hunk)
                {
                    rel_long = first_long - base_seg;
                    fwrite((char *) &rel_long, sizeof(long), 1, outf);
                }
            }
            if(!x68_reloc_size) break;
            r = fread((char *) &reloc_offset, 1, sizeof(short), inf);
            if(r != sizeof(short))
            {
                return 0;
            }
            if(reloc_offset == 1)
            {
                r = fread(&reloc_offset_long, 1, sizeof(unsigned long), inf);
                if(r != sizeof(unsigned long))
                {
                    return 0;
                }
                was_long_jump = 1;
                first_long += reloc_offset_long;
            }
            else
            {
                first_long += reloc_offset;
            }
        }
    }
    return 1;
}

/* --------------------------------
   write the data of a symbol hunk (if any)
   -------------------------------- */
int write_debug_symbols(struct x68_x_header *ah, int base_reg, int hunk_number)
{
    /* debug symbols are present */
    if(ah->size[SYM_SIZE])
    {
        struct symbol sym;
        int name_len;
        int padding;
        int pad_name_len;
        int count;
        int r;
        unsigned long l;
        int hunk_written = 0;

        count = ah->size[SYM_SIZE];
        if(count)
        {
            printf("Writing SYMBOL hunk...");
            fseek(inf, sizeof(struct x68_x_header) + ah->size[CODE_SIZE] + ah->size[DATA_SIZE] + ah->size[RELOC_SIZE], SEEK_SET);
            while(count)
            {
                r = fread((char *) &sym, 1, sizeof(sym), inf);
                count -= sizeof(sym);
                if(r != sizeof(sym))
                {
                    return 0;
                }
                /* read the first char of the name */
                memset(sym_name, 0, sizeof(sym_name));
                name_len = 0;
                r = fread((char *) &sym_name[name_len], 1, sizeof(char), inf);
                if(!count)
                {
                    break;
                }
                count--;
                if(r != sizeof(char))
                {
                    return 0;
                }
                // read the rest of the name up to 0
                while(sym_name[name_len])
                {
                    name_len++;
                    r = fread((char *) &sym_name[name_len], 1, sizeof(char), inf);
                    count--;
                    if(r != sizeof(char))
                    {
                        return 0;
                    }
                }
                name_len++;
                if(hunk_number == (sym.segment - 1))
                {
                    if(!hunk_written)
                    {
                        l = 0x000003f0;
                        fwrite((char *) &l, sizeof(long), 1, outf);     /* hunk_symbol */
                        hunk_written = 1;                               /* never again */
                    }
                    /* length of symbol in longs */
                    pad_name_len = strlen(sym_name);
                    pad_name_len += 3;
                    pad_name_len &= 0xfffffffc;
                    l = pad_name_len >> 2;
                    fwrite((char *) &l, sizeof(long), 1, outf);
                    fwrite((char *) &sym_name, 1, pad_name_len, outf);
                    sym.offset = sym.offset - base_reg;
                    fwrite((char *) &sym.offset, sizeof(long), 1, outf);
                }
                if(name_len & 1)
                {
                    r = fread((char *) &padding, 1, sizeof(char), inf);
                    count--;
                    if(r != sizeof(char))
                    {
                        return 0;
                    }
                }
            }
            if(hunk_written)
            {
                l = 0x00000000;
                fwrite((char *) &l, sizeof(long), 1, outf);                 /* terminator */
            }
            printf(" done.\n");
        }
    }
    return 1;
}

/* --------------------------------
   write the complete data of a hunk
   -------------------------------- */
int write_hunk(unsigned char *mem_block, int write_offset, int write_size, int real_size, struct x68_x_header *ah, int hunk_number)
{
    unsigned short first_short;
    unsigned long first_long;
    unsigned char *mem_to_patch;
    unsigned long real_count_code_relocs = 0;
    unsigned long real_count_data_relocs = 0;
    unsigned long real_count_bss_relocs = 0;
    unsigned long x68_reloc_size;
    int mem_size;
    int cur_segment = 0;
    int r;
    int cur_hunk = 0;
    int was_long_jump;
    unsigned short reloc_offset;
    unsigned long reloc_offset_long;
    unsigned long l;
    unsigned long pad = 0;

    mem_size = ah->size[CODE_SIZE] + ah->size[DATA_SIZE] + ah->size[BSS_SIZE];
    mem_to_patch = (unsigned char *) malloc(mem_size);
    if(!mem_to_patch)
    {
        printf("\nCannot allocate " DEC_NUM " bytes of memory.\n", mem_size);
        exit(1);
    }
    memcpy(mem_to_patch, mem_block, mem_size);

    // Patch the segment and write it
    if(ah->size[RELOC_SIZE])
    {
        fseek(inf, sizeof(struct x68_x_header) + ah->size[CODE_SIZE] + ah->size[DATA_SIZE], SEEK_SET);
        r = fread((char *) &first_short, 1, sizeof(short), inf);    // read first reloc short
        if(r != sizeof(short))
        {
            free(mem_to_patch);
            return 0;
        }
        was_long_jump = 0;
        first_long = first_short;
        if(first_long)
        {
            x68_reloc_size = ah->size[RELOC_SIZE];
            while(x68_reloc_size)
            {
                x68_reloc_size -= sizeof(short);
                if(was_long_jump)
                {
                    x68_reloc_size -= sizeof(unsigned long);
                    was_long_jump = 0;
                }
                // check if the reloc is applied into this segment
                if(hunk_number == get_segment_number(ah, first_long))
                {
                    // make the displacement relative to this segment
                    cur_segment = get_segment_number(ah, *((unsigned long *) (mem_block + first_long)));
                    *((unsigned long *) (mem_to_patch + first_long)) = *((unsigned long *) (mem_block + first_long)) - get_segment_base_address(ah, cur_segment);
                    // count the references
                    switch(cur_segment)
                    {
                        case 0:
                            real_count_code_relocs++;
                            break;
                        case 1:
                            real_count_data_relocs++;
                            break;
                        case 2:
                            real_count_bss_relocs++;
                            break;
                    }
                }
                if(!x68_reloc_size) break;
                r = fread(&reloc_offset, 1, sizeof(short), inf);
                if(r != sizeof(short))
                {
                    free(mem_to_patch);
                    return 0;
                }
                if(reloc_offset == 1)
                {
                    r = fread(&reloc_offset_long, 1, sizeof(unsigned long), inf);
                    if(r != sizeof(unsigned long))
                    {
                        free(mem_to_patch);
                        return 0;
                    }
                    was_long_jump = 1;
                    first_long += reloc_offset_long;
                }
                else
                {
                    first_long += reloc_offset;
                }
            }
        }
    }
    fwrite(mem_to_patch + write_offset, 1, real_size, outf);
    if((write_size - real_size) > 0)
    {
        fwrite(&pad, 1, write_size - real_size, outf);
    }
    printf(" done.\n");

    // write the reloc data
    if(ah->size[RELOC_SIZE])
    {
        fseek(inf, sizeof(struct x68_x_header) + ah->size[CODE_SIZE] + ah->size[DATA_SIZE], SEEK_SET);
        r = fread((char *) &first_short, 1, sizeof(short), inf);
        if(r != sizeof(short))
        {
            free(mem_to_patch);
            return 0;
        }
        first_long = first_short;
        if(first_long && (real_count_code_relocs + real_count_data_relocs + real_count_bss_relocs))
        {
            printf("Writing RELOC32 hunk...");
            // reloc hunk
            l = 0x000003ec;
            fwrite((char *) &l, sizeof(long), 1, outf);             // hunk_reloc32
            if(ah->size[CODE_SIZE])
            {
                if(!write_reloc_hunk_data(mem_block, ah, hunk_number, 0, cur_hunk, real_count_code_relocs, write_offset))
                {
                    free(mem_to_patch);
                    return 0;
                }
                cur_hunk++;
            }
            if(ah->size[DATA_SIZE])
            {
                if(!write_reloc_hunk_data(mem_block, ah, hunk_number, 1, cur_hunk, real_count_data_relocs, write_offset))
                {
                    free(mem_to_patch);
                    return 0;
                }
                cur_hunk++;
            }
            if(ah->size[BSS_SIZE])
            {
                if(!write_reloc_hunk_data(mem_block, ah, hunk_number, 2, cur_hunk, real_count_bss_relocs, write_offset))
                {
                    free(mem_to_patch);
                    return 0;
                }
            }
            // complete the hunk
            l = 0x00000000;
            fwrite((char *) &l, sizeof(long), 1, outf);     // end the relocs
            printf(" done.\n");
        }
    }
    if(!write_debug_symbols(ah, write_offset, hunk_number))
    {
        free(mem_to_patch);
        return 0;
    }
    l = 0x000003f2;
    fwrite((char *) &l, sizeof(long), 1, outf);             // end the hunk
    free(mem_to_patch);
    return 1;
}

/* --------------------------------
   print arguments
   -------------------------------- */
void print_usage()
{
    printf("Convert Sharp X68000 exec/data files to Amiga.\n"
           "For reverse engineering purposes.\n"
           "\n\2335;32mUsage: x68k2amiga <X68000 file> [amiga file]"
           "\2330;31m\n\n"
          );
    exit(1);
}

/* --------------------------------
   program entry point
   -------------------------------- */
int main(int argc, char **argv)
{
    struct x68_x_header ah;
    struct x68_z_header zh;
    unsigned long code_size;                                // in longwords, not bytes
    unsigned long data_size;
    unsigned long packed_size;
    unsigned long archive_size;
    unsigned long bss_size;
    unsigned long orig_code_size;
    unsigned long total_code_data_bss_size;
    unsigned long count;
    unsigned long real_entrypoint;
    int r;
    int i;
    unsigned short word_data;
    unsigned long l;
    unsigned long num_hunks;
    int packed; 
    int pack_dat_offset;
    int pack_method;
    unsigned long size_depacked;
    unsigned long offset_relocs;
    unsigned long size_bss;
    unsigned long size_depacked_padded;
    unsigned char packer_magic[4];
    unsigned char zpd_magic[8];
    unsigned short zpd_idx;
    unsigned long zpd_smp_offset;
    unsigned long zpd_smp_size;
    unsigned long zpd_next_offset;
    int x_reloc;
    unsigned short word_x_reloc;
    unsigned short word_1 = 1;
    unsigned long real_x_reloc_size;

    printf("\n\2335;32mx68k2amiga\2330;31m v2.5\n");
    printf("Written by Franck Charlet (\2335;32mfranck@hitchhikr.net\2330;31m).\n\n");
    if(argc < 2 || argc > 3)
    {
        print_usage();
    }

    AP.ap_Strlen = 0;
    if(!MatchFirst((STRPTR) argv[1], &AP))
    {
        atexit(exit_func);
        do
        {
            if(strlen((char *) AP.ap_Info.fib_FileName))
            {
                // check if the first filename is the same as the given argument
                if((stricmp(argv[1], (char *) AP.ap_Info.fib_FileName) != 0) && argc == 3)
                {
                    // if it's not then a wildcard was used so we need to generate
                    // a destination name for each source name
                    argc = 2;
                }
                
                // free possible previous data
                free_stuff();
                // retrieve filename
                strcpy(source_name, (char *) AP.ap_Info.fib_FileName);
                code_size = 0;
                data_size = 0;
                bss_size = 0;

                inf = fopen(source_name, "rb");
                if(!inf)
                {
                    printf("Error opening file '" STRING "'.\n", source_name);
                    exit(1);
                }

                // create a filename if none was supplied
                if(argc != 3)
                {
                    strcpy(dest_name, source_name);
                    strcat(dest_name, ".am");
                }
                else
                {
                    strcpy(dest_name, argv[2]);
                }

                // (used in case of packed executable)
                strcpy(reconstr_name, source_name);
                // Human68k won't accept double dots in filename
                if(reconstr_name[strlen(reconstr_name) - 2] == '.')
                {
                    reconstr_name[strlen(reconstr_name) - 2] = '_';
                }
                strcat(reconstr_name, ".x");

                printf("Converting '" STRING "' to '" STRING "'...\n", source_name, dest_name);

                fseek(inf, 0, SEEK_SET);
                r = fread((char *) &packer_magic, 1, sizeof(packer_magic), inf);
                if(r != sizeof(packer_magic))
                {
                    goto err_file_reading;
                }

                // retrieve it's size
                fseek(inf, 0, SEEK_END);
                data_size = ftell(inf);
                fseek(inf, 0, SEEK_SET);

                // check if it's a SAR packed archive file
                // (i think it stands for Sorcerian ARchive, or maybe not)
                if(packer_magic[0] == 'S' &&
                   packer_magic[1] == 'A' &&
                   packer_magic[2] == 'R' &&
                   packer_magic[3] == 0x1a)
                {
                    printf("\n");
                    printf("File type: \2335;32mSAR packed archive\2330;31m\n");
                    printf("     size: " DEC_NUM " bytes\n", (int) data_size);
                    // directory offset
                    i = 16;
                    for(;;)
                    {
                        fseek(inf, i, SEEK_SET);
                        // read file name
                        memset(sar_name, 0, sizeof(sar_name));
                        r = fread(sar_name, 1, 12, inf);
                        if(r != 12)
                        {
                            goto err_file_reading;
                        }
                        // end of archive directory
                        if(sar_name[0] == 0xff)
                        {
                            break;
                        }
                        r = fread(&word_data, 1, 2, inf);
                        if(r != 2)
                        {
                            goto err_file_reading;
                        }
                        pack_dat_offset = word_data;
                        pack_dat_offset *= 256;
                        r = fread(&word_data, 1, 2, inf);
                        if(r != 2)
                        {
                            goto err_file_reading;
                        }
                        data_size = word_data;
                        fseek(inf, pack_dat_offset, SEEK_SET);
                        src = (unsigned char *) malloc(data_size);
                        if(!src)
                        {
                            printf("\nCannot allocate " DEC_NUM " bytes of memory.\n", (int) data_size);
                            exit(1);
                        }
                        r = fread(src, 1, data_size, inf);
                        if(r != data_size)
                        {
                            goto err_file_reading;
                        }
                        multiplef = fopen((char *) sar_name, "wb");
                        if(!multiplef)
                        {
                            printf("\nError opening file '" STRING "'.\n", sar_name);
                            exit(1);
                        }
                        if(i == 16)
                        {
                            printf("\n");
                        }
                        printf("Writing " DEC_NUM " bytes in file '" STRING "'...", (int) data_size, sar_name);
                        if(src[0] == 'L' &&
                           src[1] == 'Z' &&
                           src[2] == 'P' &&
                           src[3] == 0x1a)
                        {
                            printf(" (LZP packed file)");
                        }
                        fwrite(src, 1, data_size, multiplef);
                        printf(" done.\n");
                        fclose(multiplef);
                        multiplef = NULL;
                        free(src);
                        src = NULL;
                        // next file in directory
                        i += 16;
                    }
                    goto archive_bail_out;
                }

                outf = fopen(dest_name, "wb");
                if(!outf)
                {
                    printf("Error opening file '" STRING "'.\n", dest_name);
                    exit(1);
                }

                // check if it's an LZP packed data file
                if(packer_magic[0] == 'L' &&
                   packer_magic[1] == 'Z' &&
                   packer_magic[2] == 'P' &&
                   packer_magic[3] == 0x1a)
                {
                    printf("\n");
                    printf("File type: \2335;32mLZP packed data\2330;31m\n");
                    printf("     size: " DEC_NUM " bytes\n", (int) data_size);
                    // depacker depacks 'in-place'
                    src = (unsigned char *) malloc(data_size + (128 * 1024));
                    if(!src)
                    {
                        printf("\nCannot allocate " DEC_NUM " bytes of memory.\n", (int) data_size + (128 * 1024));
                        exit(1);
                    }
                    memset(src, 0, data_size + (128 * 1024));
                    // allocate the temporary buffer
                    mem_data = (unsigned char *) malloc((128 * 1024));
                    if(!mem_data)
                    {
                        printf("\nCannot allocate " DEC_NUM " bytes of memory.\n", (int) (128 * 1024));
                        exit(1);
                    }
                    memset(mem_data, 0, (128 * 1024));
                    // read the packed data
                    r = fread(src, 1, data_size, inf);
                    if(r != data_size)
                    {
                        goto err_file_reading;
                    }
                    printf("\nDepacking...");
                    data_size = depack_lzp(data_size, src, mem_data);
                    printf(" done.\n");
                    printf("\nWriting " DEC_NUM " bytes...", (int) data_size);
                    fwrite(src, 1, data_size, outf);
                    printf(" done.\n");
                    goto archive_bail_out;
                }

                packed_size = *((unsigned long *) packer_magic);
                // Valis 2 single packed file
                if(packed_size <= (data_size - 8))
                {
                    if(packed_size < (data_size - 8))
                    {
                        // look for multiple files
                        fseek(inf, packed_size + 8, SEEK_SET);
                        r = fread(&archive_size, 1, 4, inf);
                        if(r != 4)
                        {
                            goto not_an_archive;
                        }
                        if((packed_size + archive_size) <= (data_size - 8))
                        {
                            // probably an archive
                            printf("\n");
                            printf("File type: \2335;32mValis II multiple packed data\2330;31m\n");
                            printf("     size: " DEC_NUM " bytes\n", (int) data_size);

                            temp_depack_buffer = (unsigned char *) malloc(64 * 1024);
                            if(!temp_depack_buffer)
                            {
                                printf("\nCannot allocate " DEC_NUM " bytes of memory.\n", (int) (64 * 1024));
                                exit(1);
                            }

                            // we don't need that one
                            fclose(outf);
                            outf = NULL;
                            sprintf(multiple_name, "delete \"%s\" >nil:", dest_name);
                            system(multiple_name);

                            fseek(inf, 0, SEEK_SET);
                            // extract everything
                            i = 0;
                            do {
                                sprintf(multiple_name, "%s_%.3d", dest_name, i);
                                r = fread(&packed_size, 1, 4, inf);
                                if(feof(inf))
                                {
                                    break;
                                }
                                if(r != 4)
                                {
                                    goto err_file_reading;
                                }
                                multiplef = fopen(multiple_name, "wb");
                                if(!multiplef)
                                {
                                    printf("\nError opening file '" STRING "'.\n", multiple_name);
                                    exit(1);
                                }
                                src = (unsigned char *) malloc(packed_size - 8);
                                if(!src)
                                {
                                    printf("\nCannot allocate " DEC_NUM " bytes of memory.\n", (int) packed_size - 8);
                                    exit(1);
                                }
                                memset(src, 0, packed_size - 8);
                                r = fread(&size_depacked, 1, 4, inf);
                                if(r != 4)
                                {
                                    goto err_file_reading;
                                }
                                r = fread(src, 1, packed_size, inf);
                                if(r != packed_size)
                                {
                                    goto err_file_reading;
                                }
                                mem_data = (unsigned char *) malloc(size_depacked + 16);
                                if(!mem_data)
                                {
                                    printf("\nCannot allocate " DEC_NUM " bytes of memory.\n", (int) size_depacked + 16);
                                    exit(1);
                                }
                                memset(mem_data, 0, size_depacked + 16);
                                memset(temp_depack_buffer, 0, (64 * 1024));
                                printf("\nDepacking...");
                                depack_valis2(size_depacked, src, mem_data, temp_depack_buffer);
                                printf(" done.\n");
                                printf("Writing " DEC_NUM " bytes in file '" STRING "'...", (int) size_depacked, multiple_name);
                                fwrite(mem_data, 1, size_depacked, multiplef);
                                printf(" done.\n");
                                fclose(multiplef);
                                multiplef = NULL;
                                free(mem_data);
                                mem_data = NULL;
                                free(src);
                                src = NULL;
                                i++;
                            } while(!feof(inf));
                            goto archive_bail_out;
                        }
                    }
not_an_archive:
                    printf("\n");
                    printf("File type: \2335;32mValis II single packed data\2330;31m\n");
                    printf("     size: " DEC_NUM " bytes\n", (int) data_size);

                    src = (unsigned char *) malloc(data_size - 4);
                    if(!src)
                    {
                        printf("\nCannot allocate " DEC_NUM " bytes of memory.\n", (int) data_size - 4);
                        exit(1);
                    }
                    memset(src, 0, data_size - 4);
                    // read the packed data
                    fseek(inf, 4, SEEK_SET);
                    r = fread(src, 1, data_size - 4, inf);
                    if(r != data_size - 4)
                    {
                        goto err_file_reading;
                    }
                    size_depacked = *((unsigned long *) src);
                    mem_data = (unsigned char *) malloc(size_depacked + 16);
                    if(!mem_data)
                    {
                        printf("\nCannot allocate " DEC_NUM " bytes of memory.\n", (int) size_depacked + 16);
                        exit(1);
                    }
                    memset(mem_data, 0, size_depacked + 16);
                    temp_depack_buffer = (unsigned char *) malloc(64 * 1024);
                    if(!temp_depack_buffer)
                    {
                        printf("\nCannot allocate " DEC_NUM " bytes of memory.\n", (int) (64 * 1024));
                        exit(1);
                    }
                    memset(temp_depack_buffer, 0, (64 * 1024));
                    printf("\nDepacking...");
                    depack_valis2(size_depacked, src + 4, mem_data, temp_depack_buffer);
                    printf(" done.\n");
                    printf("\nWriting binary data...");
                    fwrite(mem_data, 1, size_depacked, outf);
                    printf(" done.\n");
                    goto archive_bail_out;
                }

                fseek(inf, 0, SEEK_SET);
                r = fread((char *) &zpd_magic, 1, sizeof(zpd_magic), inf);
                if(r != sizeof(zpd_magic))
                {
                    goto err_file_reading;
                }
                
                if(zpd_magic[0] == 0x10 &&
                   zpd_magic[1] == 'Z' &&
                   zpd_magic[2] == 'm' &&
                   zpd_magic[3] == 'A' &&
                   zpd_magic[4] == 'd' &&
                   zpd_magic[5] == 'p' &&
                   zpd_magic[6] == 'C' &&
                   zpd_magic[7] == 'm'
                  )
                {
                    printf("\n");
                    printf("File type: \2335;32mZMUSIC ADPCM samples pack\2330;31m\n");
                    printf("     size: " DEC_NUM " bytes\n", (int) data_size);

                    // we don't need that one
                    fclose(outf);
                    outf = NULL;
                    sprintf(multiple_name, "delete \"%s\" >nil:", dest_name);
                    system(multiple_name);

                    r = fread((char *) &zpd_idx, 1, sizeof(zpd_idx), inf);
                    if(r != sizeof(zpd_idx))
                    {
                        goto err_file_reading;
                    }

                    i = 0;
                    while(zpd_idx != 0xffff)
                    {
                        r = fread((char *) &zpd_smp_offset, 1, sizeof(zpd_smp_offset), inf);
                        if(r != sizeof(zpd_smp_offset))
                        {
                            goto err_file_reading;
                        }
                        zpd_smp_offset += ftell(inf);
                        
                        r = fread((char *) &zpd_smp_size, 1, sizeof(zpd_smp_size), inf);
                        if(r != sizeof(zpd_smp_size))
                        {
                            goto err_file_reading;
                        }
                        zpd_next_offset = ftell(inf);

                        fseek(inf, zpd_smp_offset, SEEK_SET);
                        
                        temp_depack_buffer = (unsigned char *) malloc(zpd_smp_size);
                        if(!temp_depack_buffer)
                        {
                            printf("\nCannot allocate " DEC_NUM " bytes of memory.\n", (int) (zpd_smp_size));
                            exit(1);
                        }
                        printf("\nReading " DEC_NUM " bytes from offset " DEC_NUM "...", (int) zpd_smp_size, (int) zpd_smp_offset);

                        r = fread(temp_depack_buffer, 1, zpd_smp_size, inf);
                        if(r != zpd_smp_size)
                        {
                            goto err_file_reading;
                        }
                        printf(" done.\n");

                        sprintf(multiple_name, "%s_%.3d.adpcm", dest_name, i);

                        multiplef = fopen((char *) multiple_name, "wb");
                        if(!multiplef)
                        {
                            printf("\nError opening file '" STRING "'.\n", multiple_name);
                            exit(1);
                        }
                        printf("Writing " DEC_NUM " bytes to file '" STRING "'...", (int) zpd_smp_size, multiple_name);

                        fwrite(temp_depack_buffer, 1, zpd_smp_size, multiplef);
                        printf(" done.\n");
                        fclose(multiplef);

                        free(temp_depack_buffer);
                        // make sure it's not freed twice
                        temp_depack_buffer = NULL;

                        i++;
                        // next entry in the list
                        fseek(inf, zpd_next_offset, SEEK_SET);
                        r = fread((char *) &zpd_idx, 1, sizeof(zpd_idx), inf);
                        if(r != sizeof(zpd_idx))
                        {
                            goto err_file_reading;
                        }
                    }
                    goto archive_bail_out;
                }

                fseek(inf, 0, SEEK_SET);
                r = fread((char *) &ah, 1, sizeof(ah), inf);
                if(r != sizeof(ah))
                {
err_file_reading:
                    printf("\nError while reading file '" STRING "'.\n", source_name);
                    exit(1);
                }

                if(ah.magic != MAGIC_X &&
                   ah.magic != MAGIC_Z)
                {
                    printf("\n'" STRING "' is not a supported X68000 file.\n", source_name);
                    // early exit
                    fclose(outf);
                    outf = NULL;
                    sprintf(multiple_name, "delete \"%s\" >nil:", dest_name);
                    system(multiple_name);
                    // proceed with next ine (eventually)
                    goto archive_bail_out;
                }

                data_size = 0;
                printf("\n");
                
                switch(ah.magic)
                {
                    case MAGIC_Z:
                    
                        // absolute address file
                        fseek(inf, 0, SEEK_SET);
                        r = fread((char *) &zh, 1, sizeof(zh), inf);
                        if(r != sizeof(zh))
                        {
                            goto err_file_reading;
                        }
                        printf("File type: \2335;32mabsolute\2330;31m\n\n");
                        printf("Code size: " DEC_NUM " bytes\n", (int) zh.code);
                        printf("Data size: " DEC_NUM " bytes\n", (int) zh.data);
                        printf("Bss  size: " DEC_NUM " bytes\n", (int) zh.bss);
                        printf("  Address: " HEX_NUM " \n", zh.base);
                        code_size = zh.code + zh.data + zh.bss;
                        src = (unsigned char *) malloc(code_size);
                        if(!src)
                        {
                            printf("\nCannot allocate " DEC_NUM " bytes of memory.\n", (int) code_size);
                            exit(1);
                        }
                        memset(src, 0, code_size);
                        code_size -= zh.bss;
                        r = fread(src, 1, code_size, inf);
                        if(r != code_size)
                        {
                            goto err_file_reading;
                        }
                        printf("\nWriting binary data...");
                        code_size += zh.bss;
                        fwrite(src, 1, code_size, outf);
                        printf(" done.\n");
                        break;

                    case MAGIC_X:

                        real_entrypoint = ah.entrypoint - ah.base;

                        // relocatable file
                        printf(" File  type: \2335;32mrelocatable\2330;31m\n\n");
                        printf(" Code  size: " DEC_NUM " bytes\n", (int) ah.size[CODE_SIZE]);
                        printf(" Data  size: " DEC_NUM " bytes\n", (int) ah.size[DATA_SIZE]);
                        printf(" Bss   size: " DEC_NUM " bytes\n", (int) ah.size[BSS_SIZE]);
                        printf(" Reloc size: " DEC_NUM " bytes\n", (int) ah.size[RELOC_SIZE]);
                        printf(" Debug size: " DEC_NUM " bytes\n", (int) ah.size[SYM_SIZE]);
                        if(real_entrypoint)
                        {
                            printf("Entry point: " HEX_NUM "\n", real_entrypoint);
                        }

                        if(ah.size[CODE_SIZE] == 0)
                        {
                            printf("\nFile appears to be corrupted.\n");
                            exit(1);
                        }

                        packed = 0;
                        pack_method = PACK_NONE;

                        /* look for packers signature */
                        fseek(inf, 0, SEEK_END);
                        if(ftell(inf) >= (180 + sizeof(ah) + real_entrypoint))
                        {
                            fseek(inf, sizeof(ah) + real_entrypoint, SEEK_SET);
                            r = fread(packer_sign, 1, 180, inf);
                            if(r != 180)
                            {
                                goto err_file_reading;
                            }

                            for(i = 0; i < 180; i++)
                            {
                                if(packer_sign[i] == 'L' &&
                                   packer_sign[i + 1] == 'Z' &&
                                   packer_sign[i + 2] == 'X' &&
                                   packer_sign[i + 3] == ' ' &&
                                   packer_sign[i + 4] == '0' &&
                                   packer_sign[i + 5] == '.' &&
                                   packer_sign[i + 6] == '3' &&
                                   packer_sign[i + 7] == '1'
                                  )
                                {
                                    printf("\nFile packed with: \2335;32mLZX v0.31\2330;31m.\n");
                                    packed = 1;
                                    pack_dat_offset = i;
                                    pack_method = LZX_0_31;
                                    break;
                                }
                                if(packer_sign[i] == 'L' &&
                                   packer_sign[i + 1] == 'Z' &&
                                   packer_sign[i + 2] == 'X' &&
                                   packer_sign[i + 3] == ' ' &&
                                   packer_sign[i + 4] == '0' &&
                                   packer_sign[i + 5] == '.' &&
                                   packer_sign[i + 6] == '4' &&
                                   packer_sign[i + 7] == '2'
                                  )
                                {
                                    printf("\nFile packed with: \2335;32mLZX v0.42\2330;31m.\n");
                                    packed = 1;
                                    pack_dat_offset = i;
                                    pack_method = LZX_0_42;
                                    break;
                                }
                                // only used once (i think)
                                if(packer_sign[i] == '1' &&
                                   packer_sign[i + 1] == '9' &&
                                   packer_sign[i + 2] == '9' &&
                                   packer_sign[i + 3] == '2' &&
                                   packer_sign[i + 4] == '_' &&
                                   packer_sign[i + 5] == 'E' &&
                                   packer_sign[i + 6] == 'm' &&
                                   packer_sign[i + 7] == 'u'
                                  )
                                {
                                    printf("\nFile packed with: \2335;32mLZX v0.42\2330;31m.\n");
                                    packed = 1;
                                    pack_dat_offset = i;
                                    pack_method = LZX_0_42;
                                    break;
                                }
                                if(packer_sign[i] == 'L' &&
                                   packer_sign[i + 1] == 'Z' &&
                                   packer_sign[i + 2] == 'X' &&
                                   packer_sign[i + 3] == ' ' &&
                                   packer_sign[i + 4] == '1' &&
                                   packer_sign[i + 5] == '.' &&
                                   packer_sign[i + 6] == '0' &&
                                   packer_sign[i + 7] == '4'
                                  )
                                {
                                    printf("\nFile packed with: \2335;32mLZX v1.04\2330;31m.\n");
                                    packed = 1;
                                    pack_dat_offset = i;
                                    pack_method = LZX_1_04;
                                    break;
                                }
                                if(packer_sign[i] == 0x2a &&
                                   packer_sign[i + 1] == 0x4c &&
                                   packer_sign[i + 2] == 0xd9 &&
                                   packer_sign[i + 3] == 0xfa &&
                                   packer_sign[i + 4] == 0x00 &&
                                   packer_sign[i + 5] == 0x2a &&
                                   packer_sign[i + 6] == 0x48 &&
                                   packer_sign[i + 7] == 0xe7
                                  )
                                {
                                    printf("\nFile packed with: \2335;32mLZX (unknown version)\2330;31m.\n");
                                    packed = 1;
                                    pack_dat_offset = i;
                                    pack_method = LZX_UNK;
                                    break;
                                }
                                if(packer_sign[i] == 0x4b &&
                                   packer_sign[i + 1] == 0xfa &&
                                   packer_sign[i + 2] == 0xff &&
                                   packer_sign[i + 3] == 0xee &&
                                   packer_sign[i + 4] == 0x2e &&
                                   packer_sign[i + 5] == 0x0d &&
                                   packer_sign[i + 6] == 0x28 &&
                                   packer_sign[i + 7] == 0x5d
                                  )
                                {
                                    printf("\nFile packed with: \2335;32mCapcom Packer (Variant 1)\2330;31m.\n");
                                    packed = 1;
                                    pack_dat_offset = 0x3c;
                                    pack_method = LZX_CAPCOM_1;
                                    fseek(inf, sizeof(ah), SEEK_SET);
                                    break;
                                }
                                if(packer_sign[i] == 0x4b &&
                                   packer_sign[i + 1] == 0xe8 &&
                                   packer_sign[i + 2] == 0x01 &&
                                   packer_sign[i + 3] == 0x00 &&
                                   packer_sign[i + 4] == 0x2e &&
                                   packer_sign[i + 5] == 0x0d &&
                                   packer_sign[i + 6] == 0x58 &&
                                   packer_sign[i + 7] == 0x4d
                                  )
                                {
                                    printf("\nFile packed with: \2335;32mCapcom Packer (Variant 2)\2330;31m.\n");
                                    packed = 1;
                                    pack_dat_offset = 0xce;
                                    pack_method = LZX_CAPCOM_2;
                                    fseek(inf, sizeof(ah) + 4, SEEK_SET);
                                    break;
                                }
                            }
                        }
                        if(packed)
                        {
                            if(pack_method == LZX_CAPCOM_1 || pack_method == LZX_CAPCOM_2)
                            {
                                // Retrieve second header infos
                                r = fread((char *) &ah.entrypoint, 1, sizeof(unsigned long), inf);
                                if(r != sizeof(unsigned long))
                                {
                                    goto err_file_reading;
                                }
                                r = fread((char *) &offset_relocs, 1, sizeof(unsigned long), inf);
                                if(r != sizeof(unsigned long))
                                {
                                    goto err_file_reading;
                                }
                                r = fread((char *) &size_bss, 1, sizeof(unsigned long), inf);
                                if(r != sizeof(unsigned long))
                                {
                                    goto err_file_reading;
                                }
                                if(pack_method == LZX_CAPCOM_2)
                                {
                                    // The data packed size isn't in the header,
                                    // subtract the header and depacker size.
                                    code_size = ah.size[CODE_SIZE] - 206 - 156;
                                }
                                else
                                {
                                    r = fread((char *) &code_size, 1, sizeof(unsigned long), inf);
                                    if(r != sizeof(unsigned long))
                                    {
                                        goto err_file_reading;
                                    }
                                }
                                if(ah.entrypoint)
                                {
                                    printf("Depacked entry point: " HEX_NUM "\n", ah.entrypoint);
                                }
                                orig_code_size = code_size;
                                if(code_size & 1)
                                {
                                    code_size++;
                                }
                                src = (unsigned char *) malloc(code_size);
                                if(!src)
                                {
                                    printf("\nCannot allocate " DEC_NUM " bytes of memory.\n", (int) code_size);
                                    exit(1);
                                }
                                memset(src, 0, code_size);
                                // read the packed data
                                fseek(inf, sizeof(ah) + pack_dat_offset, SEEK_SET);
                                r = fread(src, 1, code_size, inf);
                                if(r != code_size)
                                {
                                    goto err_file_reading;
                                }
                                size_depacked_padded = (((size_bss + (code_size * 6)) + 3) >> 2) << 2;
                                mem_code = (unsigned char *) malloc(size_depacked_padded);
                                if(!mem_code)
                                {
                                    printf("\nCannot allocate " DEC_NUM " bytes of memory.\n", (int) size_depacked_padded);
                                    exit(1);
                                }
                                memset(mem_code, 0, size_depacked_padded);
                                depacked_relocs = (unsigned long *) malloc(code_size << 2);
                                if(!depacked_relocs)
                                {
                                    printf("\nCannot allocate " DEC_NUM " bytes of memory.\n", (int) size_bss);
                                    exit(1);
                                }
                                memset(depacked_relocs, 0, code_size << 2);
                                printf("\nDepacking (packed size: " DEC_NUM " bytes)...", (int) code_size);
                                if(pack_method == LZX_CAPCOM_2)
                                {
                                    code_size = depack_capcom_2(orig_code_size, src, mem_code, offset_relocs, size_bss, depacked_relocs);
                                }
                                else
                                {
                                    code_size = depack_capcom_1(orig_code_size, src, mem_code, offset_relocs, size_bss, depacked_relocs);
                                }
                                size_depacked_padded = ((code_size + 3) >> 2) << 2;
                            }
                            else
                            {
                                code_size = ah.size[CODE_SIZE] + ah.size[DATA_SIZE] + ah.size[RELOC_SIZE] + sizeof(ah);
                                fseek(inf, 0, SEEK_SET);
                                switch(pack_method)
                                {
                                    case LZX_0_31:
                                        ah.entrypoint = (packer_sign[14 + 0] << 24) |
                                                        (packer_sign[14 + 1] << 16) |
                                                        (packer_sign[14 + 2] << 8) |
                                                        (packer_sign[14 + 3])
                                                        ;
                                        break;
                                    case LZX_0_42:
                                        ah.entrypoint = (packer_sign[14 + 0] << 24) |
                                                        (packer_sign[14 + 1] << 16) |
                                                        (packer_sign[14 + 2] << 8) |
                                                        (packer_sign[14 + 3])
                                                        ;
                                        break;
                                    case LZX_1_04:
                                        ah.entrypoint = (packer_sign[14 + 0] << 24) |
                                                        (packer_sign[14 + 1] << 16) |
                                                        (packer_sign[14 + 2] << 8) |
                                                        (packer_sign[14 + 3])
                                                        ;
                                        break;
                                    case LZX_UNK:
                                        ah.entrypoint = (packer_sign[46 + 0] << 24) |
                                                        (packer_sign[46 + 1] << 16) |
                                                        (packer_sign[46 + 2] << 8) |
                                                        (packer_sign[46 + 3])
                                                        ;
                                        break;
                                }
                                 
                                if(ah.entrypoint)
                                {
                                    printf("Depacked entry point: " HEX_NUM "\n", ah.entrypoint);
                                }
                                src = (unsigned char *) malloc(code_size);
                                if(!src)
                                {
                                    printf("\nCannot allocate " DEC_NUM " bytes of memory.\n", (int) code_size);
                                    exit(1);
                                }
                                memset(src, 0, code_size);

                                // read the packed data
                                r = fread(src, 1, code_size, inf);
                                if(r != code_size)
                                {
                                    goto err_file_reading;
                                }
                               
                                if(pack_method == LZX_UNK)
                                {
                                    size_depacked = *((unsigned long *) &src[0x72 + pack_dat_offset + real_entrypoint]);
                                }
                                else
                                {
                                    size_depacked = *((unsigned long *) &src[0x4e + pack_dat_offset + real_entrypoint]);
                                }
                                size_depacked_padded = ((size_depacked + 3) >> 2) << 2;
                                mem_code = (unsigned char *) malloc(size_depacked_padded);
                                if(!mem_code)
                                {
                                    printf("\nCannot allocate " DEC_NUM " bytes of memory.\n", (int) size_depacked_padded);
                                    exit(1);
                                }
                                memset(mem_code, 0, size_depacked_padded);
                                depacked_relocs = (unsigned long *) malloc(size_depacked_padded << 2);
                                if(!depacked_relocs)
                                {
                                    printf("\nCannot allocate " DEC_NUM " bytes of memory.\n", (int) size_depacked_padded << 2);
                                    exit(1);
                                }
                                memset(depacked_relocs, 0, size_depacked_padded);
                                printf("\nDepacking (depacked size: " DEC_NUM " bytes)...", (int) size_depacked);
                                switch(pack_method)
                                {
                                    case LZX_0_31:
                                        depack_lzx_0_31(pack_dat_offset, src, mem_code, depacked_relocs);
                                        break;
                                    case LZX_0_42:
                                        depack_lzx_0_42(pack_dat_offset, src, mem_code, depacked_relocs);
                                        break;
                                    case LZX_1_04:
                                        depack_lzx_1_04(pack_dat_offset, src, mem_code, depacked_relocs);
                                        break;
                                    case LZX_UNK:
                                        depack_lzx_unk(pack_dat_offset, src, mem_code, depacked_relocs);
                                        break;
                                }
                            }
                            printf(" done.\n");
                            code_size = size_depacked_padded >> 2;
                        }
                        else
                        {
                            code_size = (ah.size[CODE_SIZE] + 3) >> 2;
                            data_size = (ah.size[DATA_SIZE] + 3) >> 2;
                            bss_size = (ah.size[BSS_SIZE] + 3) >> 2;
                            fseek(inf, sizeof(ah), SEEK_SET);
                            printf("\n");
                        }

                        // amiga hunks header
                        num_hunks = 0;
                        if(code_size)
                        {
                            ++num_hunks;
                        }
                        if(data_size)
                        {
                            ++num_hunks;
                        }
                        if(bss_size)
                        {
                            ++num_hunks;
                        }
                        
                        if(packed && code_size)
                        {
                            // reconstruct an X68000 executable
                            recf = fopen(reconstr_name, "wb");
                            if(!recf)
                            {
                                printf("Error opening file '" STRING "'.\n", reconstr_name);
                                exit(1);
                            }
                            printf("\nRecreating depacked " STRING " executable...\n\n", "X68000");
                            rec_head.magic = MAGIC_X;
                            rec_head.size[CODE_SIZE] = code_size << 2;
                            rec_head.entrypoint = ah.entrypoint;
                            count = 0;
                            while(depacked_relocs[count] != -1)
                            {
                                count++;
                            }
                            // calc real size of relocations section
                            real_x_reloc_size = 0;
                            for(i = 0; i < count; i++)
                            {
                                if(i == 0)
                                {
                                    x_reloc = depacked_relocs[i];
                                }
                                else
                                {
                                    x_reloc = depacked_relocs[i] - depacked_relocs[i - 1];
                                }
                                if(x_reloc >= 32767)
                                {
                                    // add 1 long word
                                    real_x_reloc_size += 4;
                                }
                                real_x_reloc_size += 2;
                            }
                            rec_head.size[RELOC_SIZE] = real_x_reloc_size;
                            printf("Writing header...");
                            fwrite(&rec_head, sizeof(struct x68_x_header), 1, recf);
                            printf(" done.\n");
                            printf("Writing data...");
                            fwrite(mem_code, 1, code_size << 2, recf);
                            printf(" done.\n");
                            printf("Writing relocations...");
                            for(i = 0; i < count; i++)
                            {
                                if(i == 0)
                                {
                                    x_reloc = depacked_relocs[i];
                                }
                                else
                                {
                                    x_reloc = depacked_relocs[i] - depacked_relocs[i - 1];
                                }
                                // it maybe 65535,
                                // i really don't know but it shouldn't be a source of problem anyway
                                if(x_reloc >= 32767)
                                {
                                    // long relocation
                                    fwrite((char *) &word_1, sizeof(unsigned short), 1, recf);
                                    fwrite((char *) &x_reloc, sizeof(unsigned long), 1, recf);
                                }
                                else
                                {
                                    word_x_reloc = (unsigned short) x_reloc;
                                    fwrite((char *) &word_x_reloc, sizeof(unsigned short), 1, recf);
                                }
                            }
                            printf(" done.\n");
                        }

                        printf("\nCreating " STRING " executable (for reversing purposes)...\n\n", "Amiga");
                        fflush(outf);
                        printf("Writing HEADER hunk...");
                        l = 0x000003f3;
                        fwrite((char *) &l, sizeof(long), 1, outf);                 // hunk_header
                        l = 0x00000000;
                        fwrite((char *) &l, sizeof(long), 1, outf);                 // end of name list

                        fwrite((char *) &num_hunks, sizeof(long), 1, outf);         // table size
                        l = 0x00000000;
                        fwrite((char *) &l, sizeof(long), 1, outf);                 // first hunk
                        --num_hunks;
                        fwrite((char *) &num_hunks, sizeof(long), 1, outf);         // last hunk
                        ++num_hunks;
                        // size of each hunk
                        if(code_size)
                        {
                            fwrite((char *) &code_size, sizeof(long), 1, outf);
                        }
                        if(data_size)
                        {
                            fwrite((char *) &data_size, sizeof(long), 1, outf);
                        }
                        if(bss_size)
                        {
                            fwrite((char *) &bss_size, sizeof(long), 1, outf);
                        }
                        printf(" done.\n");
                        
                        // store the code hunk
                        if(code_size)
                        {
                            printf("Writing CODE hunk...");
                            // hunk code
                            l = 0x000003e9;
                            fwrite((char *) &l, sizeof(long), 1, outf);
                            fwrite((char *) &code_size, sizeof(long), 1, outf);
                            flush_cache();
                            if(!packed)
                            {
                                total_code_data_bss_size = ah.size[CODE_SIZE] + ah.size[DATA_SIZE] + ah.size[BSS_SIZE];
                                mem_code = (unsigned char *) malloc(total_code_data_bss_size);
                                if(!mem_code)
                                {
                                    printf("\nCannot allocate " DEC_NUM " bytes of memory.\n", (int) (total_code_data_bss_size));
                                    exit(1);
                                }
                                memset(mem_code, 0, total_code_data_bss_size);
                                // read the data
                                fseek(inf, sizeof(struct x68_x_header), SEEK_SET);
                                r = fread(mem_code, 1, ah.size[CODE_SIZE] + ah.size[DATA_SIZE], inf);
                                if(r != (ah.size[CODE_SIZE] + ah.size[DATA_SIZE]))
                                {
                                    goto err_file_reading;
                                }
                                if(!write_hunk(mem_code, 0, code_size << 2, ah.size[CODE_SIZE], &ah, 0))
                                {
                                    goto err_file_reading;
                                }
                            }
                            else
                            {
                                fwrite(mem_code, 1, code_size << 2, outf);
                                printf(" done.\n");
                                count = 0;
                                // relocs hunk for packed files (always 1 segment)
                                // last relocation mark = -1
                                while(depacked_relocs[count] != -1)
                                {
                                    count++;
                                }
                                if(count)
                                {
                                    printf("Writing RELOC32 hunk...");
                                    l = 0x000003ec;
                                    fwrite((char *) &l, sizeof(long), 1, outf);             // hunk_reloc32
                                    fwrite((char *) &count, 1, sizeof(long), outf);         // amount of entries
                                    l = 0;
                                    fwrite((char *) &l, sizeof(long), 1, outf);             // reloc on hunk 0 only
                                    for(i = 0; i < count; i++)
                                    {
                                        fwrite((char *) &depacked_relocs[i], sizeof(long), 1, outf);
                                    }
                                    // complete the hunk
                                    l = 0x00000000;
                                    fwrite((char *) &l, sizeof(long), 1, outf);             // end of reloc
                                    printf(" done.\n");
                                }
                                free(mem_code);
                                mem_code = NULL;
                                l = 0x000003f2;
                                fwrite((char *) &l, sizeof(long), 1, outf);                 /* hunk_end */
                                goto archive_bail_out;
                            }
                        }

                        // store the data hunk
                        if(data_size)
                        {
                            printf("Writing DATA hunk...");
                            // hunk data
                            l = 0x000003ea;
                            fwrite((char *) &l, sizeof(long), 1, outf);
                            fwrite((char *) &data_size, sizeof(long), 1, outf);
                            if(!write_hunk(mem_code, ah.size[CODE_SIZE], data_size << 2, ah.size[DATA_SIZE], &ah, 1))
                            {
                                goto err_file_reading;
                            }
                        }

                        // store the bss hunk
                        if(bss_size)
                        {
                            printf("Writing BSS hunk...");
                            // hunk bss
                            l = 0x000003eb;
                            fwrite((char *) &l, sizeof(long), 1, outf);
                            fwrite((char *) &bss_size, sizeof(long), 1, outf);
                            printf(" done.\n");
                            if(!write_debug_symbols(&ah, ah.size[CODE_SIZE] + ah.size[DATA_SIZE], 2))
                            {
                                goto err_file_reading;
                            }
                            l = 0x000003f2;
                            fwrite((char *) &l, sizeof(long), 1, outf);
                        }
                        break;
                }
            }
archive_bail_out:;
        } while(MatchNext(&AP) != ERROR_NO_MORE_ENTRIES);
    }
    else
    {
        printf("Can't open specified file.\n\n");
        print_usage();
    }
    exit(0);
}
