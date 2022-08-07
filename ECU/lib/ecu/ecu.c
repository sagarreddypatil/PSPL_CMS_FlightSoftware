#include "ecu.h"

ecu_t ecu_init() {
    ecu_t ecu;

    ecu.adc = adc_init(&DDRB, &PORTB, ECU_ADC_0_SS);
    ecu.eth = ethernut_init(&DDRB, &PORTB, ECU_ADC_0_SS, ECU_IP);
    ecu.flash = flash_init(&DDRB, &PORTB, ECU_ADC_0_SS);

    return ecu;
}

void ecu_health(ecu_t *ecu) {
    uint16_t mean = (((uint32_t)(ecu->data.pressure_1)) + ((uint32_t)(ecu->data.pressure_2)) + ((uint32_t)(ecu->data.pressure_3))) / 3;

    if(ecu->data.pressure_1 > mean + ECU_BOUND || ecu->data.pressure_1 < mean - ECU_BOUND) {
        ecu->health.pressure_1_count++;
        if(ecu->health.pressure_1_count > ECU_NUM_FAILURES) {
            ecu->health.pressure_1 = true;
        }
    } if(ecu->data.pressure_2 > mean + ECU_BOUND || ecu->data.pressure_2 < mean - ECU_BOUND) {
        ecu->health.pressure_2_count++;
        if(ecu->health.pressure_2_count > ECU_NUM_FAILURES) {
            ecu->health.pressure_2 = true;
        }
    } if(ecu->data.pressure_3 > mean + ECU_BOUND || ecu->data.pressure_3 < mean - ECU_BOUND) {
        ecu->health.pressure_3_count++;
        if(ecu->health.pressure_3_count > ECU_NUM_FAILURES) {
            ecu->health.pressure_3 = true;
        }
    }
}