#include "terminal_impl.h"
#include "../scheduler/scheduler_impl.h"

using namespace os;

void os::_cmd_cls()
{
    clear();
}


void os::_cmd_restart()
{
    attr(BOLD, YELLOW);
    print(F("Restarting..."));
    attr(RESET);
    println();
    println();

    ESP.restart();
}

void os::_cmd_stats()
{
    attr(BLACK, BG_CYAN);
    print(F("ESP stats"));
    attr(RESET);
    println();

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
        print(F("BootMode:              SYS_BOOT_NORMAL_MODE\r\n"));
        break;
    case 0:
        print(F("BootMode:              SYS_BOOT_ENHANCE_MODE\r\n"));
        break;
    default:
        printf(F("BootMode:              %d\r\n"), boot_mode);
        break;
    }

    printf(F("CpuFreq:               %d MHz\r\n"), ESP.getCpuFreqMHz());
    printf(F("FlashChipId:           0x%08X\r\n"), ESP.getFlashChipId());

    uint32_t chip_real_size = ESP.getFlashChipRealSize();
    if (chip_real_size < 1024)
    {
        printf(F("FlashChipRealSize:     %d b\r\n"), chip_real_size);
    }
    else if (chip_real_size < 1024 * 1024)
    {
        printf(F("FlashChipRealSize:     %d Kb\r\n"), chip_real_size / 1024);
    }
    else
    {
        printf(F("FlashChipRealSize:     %d Mb\r\n"), chip_real_size / 1024 / 1024);
    }

    uint32_t chip_size = ESP.getFlashChipSize();
    if (chip_size < 1024)
    {
        printf(F("FlashChipSize:         %d b\r\n"), chip_size);
    }
    else if (chip_size < 1024 * 1024)
    {
        printf(F("FlashChipSize:         %d Kb\r\n"), chip_size / 1024);
    }
    else
    {
        printf(F("FlashChipSize:         %d Mb\r\n"), chip_size / 1024 / 1024);
    }

    printf(F("FlashChipSpeed:        %d MHz\r\n"), ESP.getFlashChipSpeed() / 1000 / 1000);

    FlashMode_t flash_mode = ESP.getFlashChipMode();
    switch (flash_mode)
    {
    case FM_QIO:
        print(F("FlashChipMode:         FM_QIO\r\n"));
        break;
    case FM_QOUT:
        print(F("FlashChipMode:         FM_QOUT\r\n"));
        break;
    case FM_DIO:
        print(F("FlashChipMode:         FM_DIO\r\n"));
        break;
    case FM_DOUT:
        print(F("FlashChipMode:         FM_DOUT\r\n"));
        break;
    case FM_UNKNOWN:
        print(F("FlashChipMode:         FM_UNKNOWN\r\n"));
        break;
    default:
        printf(F("FlashChipMode:         0x%02X\r\n"), flash_mode);
        break;
    }

    uint32_t sketch_size = ESP.getSketchSize();
    if (sketch_size < 1024)
    {
        printf(F("SketchSize:            %d b\r\n"), sketch_size);
    }
    else if (sketch_size < 1024 * 1024)
    {
        printf(F("SketchSize:            %d Kb\r\n"), sketch_size / 1024);
    }
    else
    {
        printf(F("SketchSize:            %d Mb\r\n"), sketch_size / 1024 / 1024);
    }

    printf(F("SketchMD5:             %s\r\n"), ESP.getSketchMD5().c_str());

    uint32_t Free_sketch_size = ESP.getFreeSketchSpace();
    if (Free_sketch_size < 1024)
    {
        printf(F("FreeSketchSpace:       %d b\r\n"), Free_sketch_size);
    }
    else if (Free_sketch_size < 1024 * 1024)
    {
        printf(F("FreeSketchSpace:       %d Kb\r\n"), Free_sketch_size / 1024);
    }
    else
    {
        printf(F("FreeSketchSpace:       %d Mb\r\n"), Free_sketch_size / 1024 / 1024);
    }

    printf(F("ResetReason:           %s\r\n"), ESP.getResetReason().c_str());
    printf(F("ResetInfo:             %s\r\n"), ESP.getResetInfo().c_str());
    printf(F("CycleCount:            %d\r\n"), ESP.getCycleCount());
}

void os::_cmd_ps()
{
    attr(BLACK, BG_CYAN);
    print(F("Threads"));
    attr(RESET);
    println();

    _dump_threads();
}