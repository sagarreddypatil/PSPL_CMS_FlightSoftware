#include "emu.h"
#include "config.h"

int32_t fuel_pressure;
int32_t ox_pressure;

int32_t fuel_upper_setpoint = 0;
int32_t fuel_lower_setpoint = 0;

int32_t ox_upper_setpoint = 0;
int32_t ox_lower_setpoint = 0;

bb_state_t fuel_state = BB_ISOLATE;
bb_state_t ox_state   = BB_ISOLATE;

void bang_bang_loop_main() {
    // NOTE: GPIO Put is atomic

    TickType_t prev_wake = xTaskGetTickCount();

    uint64_t fuel_solenoid_last_set = time_us_64();
    uint64_t ox_solenoid_last_set   = time_us_64();

    bool fuel_solenoid_intended_state = SOLENOID_CLOSE;
    bool ox_solenoid_intended_state   = SOLENOID_CLOSE;

    bool fuel_solenoid_state = SOLENOID_CLOSE;
    bool ox_solenoid_state   = SOLENOID_CLOSE;

    while (true) {
        // Thread safe get values

        global_lock();
        const int32_t my_fuel_upper_setpoint = fuel_upper_setpoint;
        const int32_t my_fuel_lower_setpoint = fuel_lower_setpoint;

        const int32_t my_ox_upper_setpoint = ox_upper_setpoint;
        const int32_t my_ox_lower_setpoint = ox_lower_setpoint;

        const bb_state_t my_fuel_state = fuel_state;
        const bb_state_t my_ox_state   = ox_state;
        global_unlock();

        taskENTER_CRITICAL();
        const int32_t my_fuel_pressure = fuel_pressure;
        const int32_t my_ox_pressure   = ox_pressure;
        taskEXIT_CRITICAL();

        // Decide intended state

        switch (my_fuel_state) {
            case BB_ISOLATE:
                fuel_solenoid_intended_state = SOLENOID_CLOSE;
                break;
            case BB_OPEN:
                fuel_solenoid_intended_state = SOLENOID_OPEN;
                break;
            case BB_REGULATE:
                if (my_fuel_pressure >= my_fuel_upper_setpoint) {
                    fuel_solenoid_intended_state = SOLENOID_CLOSE;
                } else if (my_fuel_pressure <= my_fuel_lower_setpoint) {
                    fuel_solenoid_intended_state = SOLENOID_OPEN;
                }
                break;
        }

        switch (my_ox_state) {
            case BB_ISOLATE:
                ox_solenoid_intended_state = SOLENOID_CLOSE;
                break;
            case BB_OPEN:
                ox_solenoid_intended_state = SOLENOID_OPEN;
                break;
            case BB_REGULATE:
                if (my_ox_pressure >= my_ox_upper_setpoint) {
                    ox_solenoid_intended_state = SOLENOID_CLOSE;
                } else if (my_ox_pressure <= my_ox_lower_setpoint) {
                    ox_solenoid_intended_state = SOLENOID_OPEN;
                }
                break;
        }

        // Apply state according to minimum period

        uint64_t now = time_us_64();

        if (now >= fuel_solenoid_last_set + FUEL_SOLENOID_MIN_PERIOD) {
            fuel_solenoid_state    = fuel_solenoid_intended_state;
            fuel_solenoid_last_set = now;
        }

        if (now >= ox_solenoid_last_set + OX_SOLENOID_MIN_PERIOD) {
            ox_solenoid_state    = ox_solenoid_intended_state;
            ox_solenoid_last_set = now;
        }

        gpio_put(FUEL_SOLENOID, fuel_solenoid_state);
        gpio_put(OX_SOLENOID, ox_solenoid_state);

        vTaskDelayUntil(&prev_wake, pdMS_TO_TICKS(BB_LOOP_PERIOD));
    }
}