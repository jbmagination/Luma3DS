/*
*   This file is part of Luma3DS
*   Copyright (C) 2017-2018 Sono (https://github.com/MarcuzD), panicbit
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*   Additional Terms 7.b and 7.c of GPLv3 apply to this file:
*       * Requiring preservation of specified reasonable legal notices or
*         author attributions in that material or in the Appropriate Legal
*         Notices displayed by works containing it.
*       * Prohibiting misrepresentation of the origin of that material,
*         or requiring that modified versions of such material be marked in
*         reasonable ways as different from the original version.
*/

#include <3ds.h>
#include "ifile.h"
#include "memory.h"
#include "menu.h"
#include "menus/screen_filters.h"
#include "draw.h"
#include "redshift/redshift.h"
#include "redshift/colorramp.h"

#define TEMP_DEFAULT NEUTRAL_TEMP

bool screenFilterPersistence;
u16 screenFilterTemperature;

#define SCREEN_FILTER_CONFIG_PATH ("/luma/screen_filter_config.bin")

void writeLut(u32* lut)
{
    GPU_FB_TOP_COL_LUT_INDEX = 0;
    GPU_FB_BOTTOM_COL_LUT_INDEX = 0;

    for (int i = 0; i <= 255; i++) {
        GPU_FB_TOP_COL_LUT_ELEM = *lut;
        GPU_FB_BOTTOM_COL_LUT_ELEM = *lut;
        lut++;
    }
}

typedef struct {
    u8 r;
    u8 g;
    u8 b;
    u8 z;
} Pixel;

void applyColorSettings(color_setting_t* cs)
{
    u16 c[0x600];
    Pixel px[0x400];
    u8 i = 0;

    memset(c, 0, sizeof(c));
    memset(px, 0, sizeof(px));

    do {
        px[i].r = i;
        px[i].g = i;
        px[i].b = i;
        px[i].z = 0;
    } while(++i);

    do {
        *(c + i + 0x000) = px[i].r | (px[i].r << 8);
        *(c + i + 0x100) = px[i].g | (px[i].g << 8);
        *(c + i + 0x200) = px[i].b | (px[i].b << 8);
    } while(++i);

    colorramp_fill(c + 0x000, c + 0x100, c + 0x200, 0x100, cs);

    do {
        px[i].r = *(c + i + 0x000) >> 8;
        px[i].g = *(c + i + 0x100) >> 8;
        px[i].b = *(c + i + 0x200) >> 8;
    } while(++i);

    writeLut((u32*)px);
}

Menu screenFiltersMenu = {
    "Screen filters menu",
    .nbItems = 3,
    {
        { "Set persistence", METHOD, .method = &screenFiltersMenu_SetPersistence },
        { "Set color temperature", METHOD, .method = &screenFiltersMenu_SetTemperature},
        { "Save settings", METHOD, .method = &screenFiltersMenu_SaveSettings}
    }
};

void screenFiltersMenu_SetPersistence()
{
    Draw_Lock();
    Draw_ClearFramebuffer();
    Draw_FlushFramebuffer();
    Draw_Unlock();

    do
    {
        Draw_Lock();

        Draw_DrawString(10, 10, COLOR_TITLE, "Screen filters menu");
        Draw_DrawString(10, 30, COLOR_WHITE, "Press A to toggle persistence");
        Draw_DrawString(10, 40, COLOR_WHITE, "Press B to go back");

        Draw_DrawString(10, 60, COLOR_RED, "WARNING:");
        Draw_DrawString(10, 70, COLOR_WHITE, "  * Persisting color filter across sleep can cause");
        Draw_DrawString(10, 80, COLOR_WHITE, "    a failure to wake!");
        Draw_DrawString(10, 90, COLOR_WHITE, "  * If you experience problems waking up,");
        Draw_DrawString(10, 100, COLOR_WHITE, "    perform a hard reset with the power button.");

        Draw_DrawString(10, 120, COLOR_WHITE, "Current status:");
        Draw_DrawString(100, 120, (screenFilterPersistence ? COLOR_GREEN : COLOR_RED), (screenFilterPersistence ? " ON " : " OFF"));

        Draw_FlushFramebuffer();
        Draw_Unlock();

        u32 pressed = waitInputWithTimeout(1000);

        if(pressed & BUTTON_A)
        {
            screenFilterPersistence = !screenFilterPersistence;
        }
        else if(pressed & BUTTON_B)
        {
            return;
        }
    }
    while (!terminationRequest);
}

void screenFiltersMenu_SetTemperature()
{
    Draw_Lock();
    Draw_ClearFramebuffer();
    Draw_FlushFramebuffer();
    Draw_Unlock();

    const unsigned int MAX_COLOR_TEMPERATURE = 6500;
    const unsigned int MIN_COLOR_TEMPERATURE = 1200;
    const unsigned int INCREMENT_AMOUNT = 500;

    do
    {
        Draw_Lock();

        Draw_DrawString(10, 10, COLOR_TITLE, "Screen filters menu");
        Draw_DrawString(10, 30, COLOR_WHITE, "Press UP to increase color temperature");
        Draw_DrawString(10, 40, COLOR_WHITE, "Press DOWN to decrease color temperature");
        Draw_DrawString(10, 50, COLOR_WHITE, "Press B to go back");
        Draw_DrawFormattedString(10, 60, COLOR_WHITE, "Current temperature: %dK", screenFilterTemperature);

        Draw_FlushFramebuffer();
        Draw_Unlock();

        u32 pressed = waitInputWithTimeout(500);

        if(pressed & BUTTON_UP)
        {
            screenFilterTemperature += INCREMENT_AMOUNT;

            if (screenFilterTemperature > MAX_COLOR_TEMPERATURE)
            {
                screenFilterTemperature = MAX_COLOR_TEMPERATURE;
            }
        }
        else if(pressed & BUTTON_DOWN)
        {
            screenFilterTemperature -= INCREMENT_AMOUNT;

            if (screenFilterTemperature < MIN_COLOR_TEMPERATURE)
            {
                screenFilterTemperature = MIN_COLOR_TEMPERATURE;
            }
        }
        else if(pressed & BUTTON_B)
        {
            return;
        }

        screenFiltersSetTemperature(screenFilterTemperature);
    }
    while (!terminationRequest);
}

void screenFiltersMenu_SaveSettings(void)
{
    ScreenFilterConfig config;

    memcpy(config.magic, "CONF", 4);
    config.temperature = screenFilterTemperature;
    config.persistence = screenFilterPersistence;

    Result res = screenFiltersWriteConfigFile(&config);

    Draw_Lock();
    Draw_ClearFramebuffer();
    Draw_FlushFramebuffer();
    Draw_Unlock();

    do
    {
        Draw_Lock();
        Draw_DrawString(10, 10, COLOR_TITLE, "Screen filters menu");
        if(R_SUCCEEDED(res))
        {
            Draw_DrawString(10, 30, COLOR_GREEN, "Operation succeeded.");
            Draw_DrawString(10, 50, COLOR_WHITE, "Please reboot to make settings take effect.");
        }
        else
        {
            Draw_DrawFormattedString(10, 30, COLOR_RED, "Operation failed (0x%08lx).", res);
        }
        Draw_FlushFramebuffer();
        Draw_Unlock();
    }
    while(!(waitInput() & BUTTON_B) && !terminationRequest);
}

Result screenFiltersReadConfigFile(ScreenFilterConfig* config)
{
    s64 out;
    if(R_FAILED(svcGetSystemInfo(&out, 0x10000, 0x203))) svcBreak(USERBREAK_ASSERT);
    bool isSdMode = (bool)out;

    FS_ArchiveID archiveId = isSdMode ? ARCHIVE_SDMC : ARCHIVE_NAND_RW;
    IFile file;
    Result res = IFile_Open(&file, archiveId, fsMakePath(PATH_EMPTY, ""), fsMakePath(PATH_ASCII, SCREEN_FILTER_CONFIG_PATH), FS_OPEN_CREATE | FS_OPEN_READ);

    // Ensure sane defaults
    ScreenFilterConfig configData;
    configData.persistence = false;
    configData.temperature = TEMP_DEFAULT;

    if(R_SUCCEEDED(res))
    {
        u64 size;

        res = IFile_Read(&file, &size, &configData, sizeof(configData));
        IFile_Close(&file);
    }

    config->temperature = configData.temperature;
    config->persistence = configData.persistence;

    return res;
}

Result screenFiltersWriteConfigFile(ScreenFilterConfig* config)
{
    Result res;

    IFile file;
    u64 total;

    s64 out;
    bool isSdMode;
    if(R_FAILED(svcGetSystemInfo(&out, 0x10000, 0x203))) svcBreak(USERBREAK_ASSERT);
    isSdMode = (bool)out;

    FS_ArchiveID archiveId = isSdMode ? ARCHIVE_SDMC : ARCHIVE_NAND_RW;
    res = IFile_Open(&file, archiveId, fsMakePath(PATH_EMPTY, ""), fsMakePath(PATH_ASCII, SCREEN_FILTER_CONFIG_PATH), FS_OPEN_CREATE | FS_OPEN_WRITE);

    if(R_SUCCEEDED(res))
    {
        res = IFile_Write(&file, &total, config, sizeof(*config), 0);
        IFile_Close(&file);
    }

    return res;
}

void screenFiltersSetTemperature(int temperature)
{
    color_setting_t cs;
    memset(&cs, 0, sizeof(cs));

    cs.temperature = temperature;
    cs.gamma[0] = 1.0F;
    cs.gamma[1] = 1.0F;
    cs.gamma[2] = 1.0F;
    cs.brightness = 1.0F;

    applyColorSettings(&cs);
}
