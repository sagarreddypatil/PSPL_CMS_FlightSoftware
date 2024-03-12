#include "emu.h"
#include "config.h"

void bang_bang_loop_main() {
    // NOTE: GPIO Put is atomic

    TickType_t prev_wake = xTaskGetTickCount();

    uint64_t fuel_solenoid_last_set = time_us_64();
    uint64_t ox_solenoid_last_set   = time_us_64();
    uint64_t aux_solenoid_last_set  = time_us_64();

    bool fuel_solenoid_intended_position = SOLENOID_CLOSE;
    bool ox_solenoid_intended_position   = SOLENOID_CLOSE;
    bool aux_solenoid_intended_position  = SOLENOID_CLOSE;

    bool fuel_solenoid_position = SOLENOID_CLOSE;
    bool ox_solenoid_position   = SOLENOID_CLOSE;
    bool aux_solenoid_position  = SOLENOID_CLOSE;

    while (true) {
        // Thread safe get values using critical section
        // This prevents the values from being changed while we are reading them
        taskENTER_CRITICAL();
        // Fuel
        const int32_t my_fuel_upper_setpoint =
            persistent_globals.fuel_upper_setpoint;
        const int32_t my_fuel_lower_setpoint =
            persistent_globals.fuel_lower_setpoint;

        // Oxygen
        const int32_t my_ox_upper_setpoint =
            persistent_globals.ox_upper_setpoint;
        const int32_t my_ox_lower_setpoint =
            persistent_globals.ox_lower_setpoint;

        // Auxiliary
        const int32_t my_aux_upper_setpoint =
            persistent_globals.aux_upper_setpoint;
        const int32_t my_aux_lower_setpoint =
            persistent_globals.aux_lower_setpoint;

        // Global control state for valves (ISOLATE, OPEN, REGULATE)
        const bb_state_t my_fuel_state = persistent_globals.fuel_state;
        const bb_state_t my_ox_state   = persistent_globals.ox_state;
        const bb_state_t my_aux_state  = persistent_globals.aux_state;

        // Pressure from ADC
        const int32_t my_fuel_pressure =
            ox_pressure;  // Same channel for bang bang test
        const int32_t my_ox_pressure  = ox_pressure;
        const int32_t my_aux_pressure = ox_pressure;
        taskEXIT_CRITICAL();

        // Decide intended state

        switch (my_fuel_state) {
            case BB_ISOLATE:  // ISOLATE closes the valve regardless of pressure
                fuel_solenoid_intended_position = SOLENOID_CLOSE;
                break;
            case BB_OPEN:  // OPEN opens the valve regardless of pressure
                fuel_solenoid_intended_position = SOLENOID_OPEN;
                break;
            case BB_REGULATE:  // REGULATE opens or closes the valve depending
                               // on pressure (bang bang)
                if (my_fuel_pressure >= my_fuel_upper_setpoint) {
                    fuel_solenoid_intended_position = SOLENOID_CLOSE;
                } else if (my_fuel_pressure <= my_fuel_lower_setpoint) {
                    fuel_solenoid_intended_position = SOLENOID_OPEN;
                }
                break;
        }

        switch (my_ox_state) {
            case BB_ISOLATE:
                ox_solenoid_intended_position = SOLENOID_CLOSE;
                break;
            case BB_OPEN:
                ox_solenoid_intended_position = SOLENOID_OPEN;
                break;
            case BB_REGULATE:
                if (my_ox_pressure >= my_ox_upper_setpoint) {
                    ox_solenoid_intended_position = SOLENOID_CLOSE;
                } else if (my_ox_pressure <= my_ox_lower_setpoint) {
                    ox_solenoid_intended_position = SOLENOID_OPEN;
                }
                break;
        }

        switch (my_aux_state) {
            case BB_ISOLATE:
                aux_solenoid_intended_position = SOLENOID_CLOSE;
                break;
            case BB_OPEN:
                aux_solenoid_intended_position = SOLENOID_OPEN;
                break;
            case BB_REGULATE:
                if (my_aux_pressure >= my_aux_upper_setpoint) {
                    aux_solenoid_intended_position = SOLENOID_CLOSE;
                } else if (my_aux_pressure <= my_aux_lower_setpoint) {
                    aux_solenoid_intended_position = SOLENOID_OPEN;
                }
                break;
        }

        // Apply state according to FUEL_SOLENOID_MIN_PERIOD (70ms)

        uint64_t now = time_us_64();

        /*
         * Only update the solenoid state if the minimum period has passed, and
         * the intended state is different from the current state This prevents
         * the solenoid from being toggled too quickly.
         *
         * However, if the intended state is the same as the current state, do
         * not update the last set time.
         *
         * This allows us to update the state as quickly as possible while still
         * satisfying the minimum period.
         *
         */

        if (now >= fuel_solenoid_last_set + FUEL_SOLENOID_MIN_PERIOD &&
            fuel_solenoid_position != fuel_solenoid_intended_position) {
            fuel_solenoid_position = fuel_solenoid_intended_position;
            fuel_solenoid_last_set = now;
        }

        if (now >= ox_solenoid_last_set + OX_SOLENOID_MIN_PERIOD &&
            ox_solenoid_position != ox_solenoid_intended_position) {
            ox_solenoid_position = ox_solenoid_intended_position;
            ox_solenoid_last_set = now;
        }

        if (now >= aux_solenoid_last_set + AUX_SOLENOID_MIN_PERIOD &&
            aux_solenoid_position != aux_solenoid_intended_position) {
            aux_solenoid_position = aux_solenoid_intended_position;
            aux_solenoid_last_set = now;
        }

        // Set the solenoid state on the GPIO pins
        gpio_put(FUEL_SOLENOID, fuel_solenoid_position);
        gpio_put(OX_SOLENOID, ox_solenoid_position);
        gpio_put(AUX_SOLENOID, aux_solenoid_position);

        // Wait until the next period (1ms)
        vTaskDelayUntil(&prev_wake, pdMS_TO_TICKS(BB_LOOP_PERIOD));
    }
}