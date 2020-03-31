#include "rt.h"

void rt_log_stats();

void rt_init()
{
    Serial.begin(115200);
    rt_log_stats();
}

void rt_log_stats()
{
    uint32_t free_mem = ESP.getFreeHeap();
    if (free_mem < 1024)
    {
        rt_logf(F("boot: FreeHeap:              %d b"), free_mem);
    }
    else
    {
        rt_logf(F("boot: FreeHeap:              %d Kb"), free_mem / 1024);
    }

    rt_logf(F("boot: ChipId:                0x%08X"), ESP.getChipId());
    rt_logf(F("boot: SdkVersion:            %s"), ESP.getSdkVersion());
    rt_logf(F("boot: CoreVersion:           %s"), ESP.getCoreVersion().c_str());

    rt_logf(F("boot: BootVersion:           %d"), ESP.getBootVersion());

    uint8_t boot_mode = ESP.getBootMode();
    switch (boot_mode)
    {
    case 1:
        rt_log(F("boot: BootMode:              SYS_BOOT_NORMAL_MODE"));
        break;
    case 0:
        rt_log(F("boot: BootMode:              SYS_BOOT_ENHANCE_MODE"));
        break;
    default:
        rt_logf(F("boot: BootMode:              %d"), boot_mode);
        break;
    }

    rt_logf(F("boot: CpuFreq:               %d MHz"), ESP.getCpuFreqMHz());
    rt_logf(F("boot: FlashChipId:           0x%08X"), ESP.getFlashChipId());

    uint32_t chip_real_size = ESP.getFlashChipRealSize();
    if (chip_real_size < 1024)
    {
        rt_logf(F("boot: FlashChipRealSize:     %d b"), chip_real_size);
    }
    else if (chip_real_size < 1024 * 1024)
    {
        rt_logf(F("boot: FlashChipRealSize:     %d Kb"), chip_real_size / 1024);
    }
    else
    {
        rt_logf(F("boot: FlashChipRealSize:     %d Mb"), chip_real_size / 1024 / 1024);
    }

    uint32_t chip_size = ESP.getFlashChipSize();
    if (chip_size < 1024)
    {
        rt_logf(F("boot: FlashChipSize:         %d b"), chip_size);
    }
    else if (chip_size < 1024 * 1024)
    {
        rt_logf(F("boot: FlashChipSize:         %d Kb"), chip_size / 1024);
    }
    else
    {
        rt_logf(F("boot: FlashChipSize:         %d Mb"), chip_size / 1024 / 1024);
    }
    rt_logf(F("boot: FlashChipSpeed:        %d MHz"), ESP.getFlashChipSpeed() / 1000 / 1000);

    FlashMode_t flash_mode = ESP.getFlashChipMode();
    switch (flash_mode)
    {
    case FM_QIO:
        rt_log(F("boot: FlashChipMode:         FM_QIO"));
        break;
    case FM_QOUT:
        rt_log(F("boot: FlashChipMode:         FM_QOUT"));
        break;
    case FM_DIO:
        rt_log(F("boot: FlashChipMode:         FM_DIO"));
        break;
    case FM_DOUT:
        rt_log(F("boot: FlashChipMode:         FM_DOUT"));
        break;
    case FM_UNKNOWN:
        rt_log(F("boot: FlashChipMode:         FM_UNKNOWN"));
        break;
    default:
        rt_logf(F("boot: FlashChipMode:         0x%02X"), flash_mode);
        break;
    }

    uint32_t sketch_size = ESP.getSketchSize();
    if (sketch_size < 1024)
    {
        rt_logf(F("boot: SketchSize:            %d b"), sketch_size);
    }
    else if (sketch_size < 1024 * 1024)
    {
        rt_logf(F("boot: SketchSize:            %d Kb"), sketch_size / 1024);
    }
    else
    {
        rt_logf(F("boot: SketchSize:            %d Mb"), sketch_size / 1024 / 1024);
    }

    rt_logf(F("boot: SketchMD5:             %s"), ESP.getSketchMD5().c_str());

    uint32_t Free_sketch_size = ESP.getFreeSketchSpace();
    if (Free_sketch_size < 1024)
    {
        rt_logf(F("boot: FreeSketchSpace:       %d b"), Free_sketch_size);
    }
    else if (Free_sketch_size < 1024 * 1024)
    {
        rt_logf(F("boot: FreeSketchSpace:       %d Kb"), Free_sketch_size / 1024);
    }
    else
    {
        rt_logf(F("boot: FreeSketchSpace:       %d Mb"), Free_sketch_size / 1024 / 1024);
    }

    rt_logf(F("boot: ResetReason:           %s"), ESP.getResetReason().c_str());
    rt_logf(F("boot: ResetInfo:             %s"), ESP.getResetInfo().c_str());
    rt_logf(F("boot: CycleCount:            %d"), ESP.getCycleCount());
}
