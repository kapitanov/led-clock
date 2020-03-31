#include "rt.h"

#include <EEPROM.h>
#include <CRC32.h>

uint32_t rt_crc(const void *ptr, size_t length)
{
    const char *buffer = (char *)ptr;

    CRC32 crc;
    for (size_t i = 0; i < length; i++)
    {
        crc.update(buffer[i]);
    }

    return crc.finalize();
}

bool rt_config_read(size_t address, void *value, size_t size)
{
    rt_logf(F("config: reading from 0x%08X"), address);

    memset(value, 0, size);
    EEPROM.begin(address + size + sizeof(uint32_t));

    for (size_t i = 0; i < size; i++)
    {
        *((uint8_t *)value + i) = EEPROM.read(address + i);
    }

    uint32_t crc = 0;
    for (size_t i = 0; i < sizeof(uint32_t); i++)
    {
        *((uint8_t *)&crc + i) = EEPROM.read(address + size + i);
    }

    EEPROM.end();

    uint32_t actual_crc = rt_crc(value, size);
    if (crc != actual_crc)
    {
        rt_logf(F("config: config is damaged (crc 0x%08X != 0x%08X)"), crc, actual_crc);
        memset(value, 0, size);
        return false;
    }

    rt_logf(F("config: crc is ok (0x%08X)"), crc);

    return true;
}

void rt_config_write(size_t address, void *value, size_t size)
{
    rt_logf(F("config: writing to 0x%08X"), address);

    EEPROM.begin(address + size + sizeof(uint32_t));

    const uint8_t *buffer = (uint8_t *)value;
    for (size_t i = 0; i < size; i++)
    {
        EEPROM.write(address + i, buffer[i]);
    }

    uint32_t crc = rt_crc(buffer, size);
    for (size_t i = 0; i < sizeof(uint32_t); i++)
    {
        EEPROM.write(address + size + i, *((uint8_t *)&crc + i));
    }

    EEPROM.commit();
    EEPROM.end();

    rt_logf(F("config: written to 0x%08X (crc 0x%08X)"), address, crc);
}