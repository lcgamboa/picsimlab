/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   bridge_gpsim.h
 * Author: gamboa
 *
 * Created on 20 de Agosto de 2020, 16:56
 */

#ifndef BRIGE_GPSIM_H
#define BRIGE_GPSIM_H

#ifdef __cplusplus
extern "C" {
#endif

int bridge_gpsim_init(const char* processor, const char* fileName, float freq);
void bridge_gpsim_reset(void);
unsigned char bridge_gpsim_get_pin_count(void);
const char* bridge_gpsim_get_pin_name(int pin);
unsigned char bridge_gpsim_get_pin_value(int pin);
unsigned char bridge_gpsim_get_pin_dir(int pin);
void bridge_gpsim_set_pin_value(int pin, unsigned char value);
void bridge_gpsim_step(void);
void bridge_gpsim_end(void);
int bridge_gpsim_dump_memory(const char* fname);
char* bridge_gpsim_get_processor_list(char* buff, unsigned int size);

#ifdef __cplusplus
}
#endif

#endif /* BRIGE_GPSIM_H */
