#ifdef BUILD_ADAM

#include "mediaTypeROM.h"

#include <cstring>

#include "../../include/debug.h"
#include "fnSystem.h"

#define ROM_BLOCK_SIZE 512

MediaTypeROM::MediaTypeROM()
{
    rom=(char *)malloc(32768);
}

MediaTypeROM::~MediaTypeROM()
{
    free(rom);
}

// Returns TRUE if an error condition occurred
bool MediaTypeROM::read(uint32_t blockNum, uint16_t *readcount)
{
    if (blockNum == 0)
        memcpy(_media_blockbuff, block0, sizeof(_media_blockbuff));
    else if (blockNum == 1)
        memcpy(_media_blockbuff, block1, sizeof(_media_blockbuff));
    else
    {
        blockNum -= 2;
        if (blockNum < 32)
            memcpy(_media_blockbuff, &rom[blockNum * 1024], sizeof(_media_blockbuff));
        else
        {
            _media_controller_status = 2;
            return true;
        }
    }
    return false;
}

// Returns TRUE if an error condition occurred
bool MediaTypeROM::write(uint32_t blockNum, bool verify)
{
    return true;
}

uint8_t MediaTypeROM::status()
{
    return _media_controller_status;
}

// Returns TRUE if an error condition occurred
bool MediaTypeROM::format(uint16_t *responsesize)
{
    return true;
}

mediatype_t MediaTypeROM::mount(FILE *f, uint32_t disksize)
{
    uint16_t o = 0;

    Debug_print("ROM MOUNT\r\n");

    _media_fileh = f;
    _mediatype = MEDIATYPE_ROM;

    if (disksize > 32768)
        disksize = 32768;

    // Load ROM into memory.
    // Do this and yield in chonks so we don't starve the other threads

    while (disksize)
    {
        uint16_t rsz = (disksize > ROM_BLOCK_SIZE ? ROM_BLOCK_SIZE : disksize);

        Debug_printf("Reading %u bytes, %u bytes remaining\n",rsz,disksize);
        
        if (fread(&rom[o], sizeof(uint8_t), rsz, f) != rsz)
        {
            fclose(f);
            _media_fileh = nullptr;
            return MEDIATYPE_UNKNOWN;
        }
        else
        {
            o += rsz;
            disksize -= rsz;
        }

        fnSystem.yield(); // Let the system breathe.
    }

    return _mediatype;
}

// Returns FALSE on error
bool MediaTypeROM::create(FILE *f, uint32_t numBlocks)
{
    return true;
}
#endif /* BUILD_ADAM */