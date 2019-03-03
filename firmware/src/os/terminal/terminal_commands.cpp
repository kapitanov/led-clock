#include "terminal_impl.h"
#include "../scheduler/scheduler_impl.h"

using namespace os;

void os::_cmd_restart()
{
    println(F("Restarting..."));
    println();

    ESP.restart();
}

void os::_cmd_stats()
{
    println(F("ESP stats"));

    uint32_t free_mem = ESP.getFreeHeap();
    if (free_mem < 1024)
    {
        printf(F("FreeHeap:              %d b\r\n"), free_mem);
    }
    else
    {
        printf(F("FreeHeap:              %d Kb\r\n"), free_mem / 1024);
    }

    printf(F("ChipId:                0x%08X\r\n"), ESP.getChipId());
    printf(F("SdkVersion:            %s\r\n"), ESP.getSdkVersion());
    printf(F("CoreVersion:           %s\r\n"), ESP.getCoreVersion().c_str());

    printf(F("BootVersion:           %d\r\n"), ESP.getBootVersion());

    uint8_t boot_mode = ESP.getBootMode();
    switch (boot_mode)
    {
    case 1:
        print(F("BootMode:              SYS_BOOT_NORMAL_MODE"));
        break;
    case 0:
        print(F("BootMode:              SYS_BOOT_ENHANCE_MODE"));
        break;
    default:
        printf(F("BootMode:              %d"), boot_mode);
        break;
    }
    println();

    printf(F("CpuFreq:               %d MHz"), ESP.getCpuFreqMHz());
    println();
    printf(F("FlashChipId:           0x%08X"), ESP.getFlashChipId());
    println();

    uint32_t chip_real_size = ESP.getFlashChipRealSize();
    if (chip_real_size < 1024)
    {
        printf(F("FlashChipRealSize:     %d b"), chip_real_size);
    }
    else if (chip_real_size < 1024 * 1024)
    {
        printf(F("FlashChipRealSize:     %d Kb"), chip_real_size / 1024);
    }
    else
    {
        printf(F("FlashChipRealSize:     %d Mb"), chip_real_size / 1024 / 1024);
    }
    println();

    uint32_t chip_size = ESP.getFlashChipSize();
    if (chip_size < 1024)
    {
        printf(F("FlashChipSize:         %d b"), chip_size);
    }
    else if (chip_size < 1024 * 1024)
    {
        printf(F("FlashChipSize:         %d Kb"), chip_size / 1024);
    }
    else
    {
        printf(F("FlashChipSize:         %d Mb"), chip_size / 1024 / 1024);
    }
    println();

    printf(F("FlashChipSpeed:        %d MHz"), ESP.getFlashChipSpeed() / 1000 / 1000);
    println();

    FlashMode_t flash_mode = ESP.getFlashChipMode();
    switch (flash_mode)
    {
    case FM_QIO:
        print(F("FlashChipMode:         FM_QIO"));
        break;
    case FM_QOUT:
        print(F("FlashChipMode:         FM_QOUT"));
        break;
    case FM_DIO:
        print(F("FlashChipMode:         FM_DIO"));
        break;
    case FM_DOUT:
        print(F("FlashChipMode:         FM_DOUT"));
        break;
    case FM_UNKNOWN:
        print(F("FlashChipMode:         FM_UNKNOWN"));
        break;
    default:
        printf(F("FlashChipMode:         0x%02X"), flash_mode);
        break;
    }
    println();

    uint32_t sketch_size = ESP.getSketchSize();
    if (sketch_size < 1024)
    {
        printf(F("SketchSize:            %d b"), sketch_size);
    }
    else if (sketch_size < 1024 * 1024)
    {
        printf(F("SketchSize:            %d Kb"), sketch_size / 1024);
    }
    else
    {
        printf(F("SketchSize:            %d Mb"), sketch_size / 1024 / 1024);
    }
    println();

    printf(F("SketchMD5:             %s"), ESP.getSketchMD5().c_str());
    println();

    uint32_t Free_sketch_size = ESP.getFreeSketchSpace();
    if (Free_sketch_size < 1024)
    {
        printf(F("FreeSketchSpace:       %d b"), Free_sketch_size);
    }
    else if (Free_sketch_size < 1024 * 1024)
    {
        printf(F("FreeSketchSpace:       %d Kb"), Free_sketch_size / 1024);
    }
    else
    {
        printf(F("FreeSketchSpace:       %d Mb"), Free_sketch_size / 1024 / 1024);
    }
    println();

    printf(F("ResetReason:           %s\r\n"), ESP.getResetReason().c_str());
    printf(F("ResetInfo:             %s\r\n"), ESP.getResetInfo().c_str());
    printf(F("CycleCount:            %d\r\n"), ESP.getCycleCount());
}

void os::_cmd_ps()
{
    println(F("Threads"));
    _dump_threads();
}