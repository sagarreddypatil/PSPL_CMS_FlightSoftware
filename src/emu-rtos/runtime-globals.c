#include "emu.h"
#include <hardware/flash.h>

// Under Mutex, needs to be saved to flash

presistent_globals_t persistent_globals = {
    .magic = FLASH_MAGIC,

    .fuel_upper_setpoint = 0,
    .fuel_lower_setpoint = 0,

    .ox_upper_setpoint = 0,
    .ox_lower_setpoint = 0,

    .fuel_state = BB_ISOLATE,
    .ox_state   = BB_ISOLATE,
};

// Under Critical Section, don't need to save
int32_t fuel_pressure = 0;
int32_t ox_pressure   = 0;
int32_t aux_pressure   = 0;

void save_presistent_globals() {
    flash_range_erase(FLASH_OFFSET, FLASH_SECTOR_SIZE);
    flash_range_program(FLASH_OFFSET, persistent_globals.raw,
                        sizeof(presistent_globals_t));
}

void load_persistent_globals() {
    presistent_globals_t *stored_presistent_casted =
        (presistent_globals_t *)(stored_presistent_contents);

    safeprintf("Stored Magic: %08x\n", stored_presistent_casted->magic);

    if (stored_presistent_casted->magic == FLASH_MAGIC) {
        memcpy(&persistent_globals, stored_presistent_contents,
               sizeof(presistent_globals_t));
    }
}