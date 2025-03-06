
#include "bridge_gpsim.h"

#include <gpsim/pic-processor.h>
#include <gpsim/sim_context.h>

#include "../lib/util.h"

void simulation_cleanup(void);

static pic_processor* gpic = NULL;
static IOPIN* iopins[100];

int bridge_gpsim_init(const char* processor, const char* fileName, float freq) {
    Processor* tempProc = 0l;
    CSimulationContext::GetContext()->LoadProgram(fileName, processor, &tempProc, NULL);
    gpic = dynamic_cast<pic_processor*>(tempProc);

    bridge_gpsim_set_frequency(freq);

    if (gpic) {
        for (int pin = 1; pin <= bridge_gpsim_get_pin_count(); pin++) {
            iopins[pin] = gpic->get_pin(pin);
        }
    }

    return (gpic != NULL) - 1;
}

void bridge_gpsim_set_frequency(double freq) {
    if (gpic) {
        gpic->set_frequency(freq);
    }
}

void bridge_gpsim_reset(void) {
    if (gpic->is_sleeping())
        gpic->exit_sleep();
    gpic->reset(POR_RESET);
}

unsigned char bridge_gpsim_get_pin_count(void) {
    if (gpic) {
        return gpic->get_pin_count();
    }
    return 8;
}

const char* bridge_gpsim_get_pin_name(int pin) {
    if (gpic) {
        return gpic->get_pin_name(pin).c_str();
    }
    return "error";
}

unsigned char bridge_gpsim_get_pin_value(int pin) {
    if (iopins[pin]) {
        return iopins[pin]->getDrivingState();
    }
    return 0;
}

unsigned char bridge_gpsim_get_pin_dir(int pin) {
    if (iopins[pin]) {
        return (iopins[pin]->get_direction() == IOPIN::DIR_INPUT);
    }
    return 0;
}

void bridge_gpsim_set_pin_value(int pin, unsigned char value) {
    char cPinState = iopins[pin]->getForcedDrivenState();

    switch (cPinState) {
        case '0':
        case 'Z':
        case 'X':
        case '1':
            if (value) {
                iopins[pin]->forceDrivenState('1');
                iopins[pin]->putState(true);
            } else {
                iopins[pin]->forceDrivenState('0');
                iopins[pin]->putState(false);
            }
            break;
        case 'W':
            if (!value)
                iopins[pin]->forceDrivenState('w');
            break;
        case 'w':
            if (value)
                iopins[pin]->forceDrivenState('W');
            break;
    }
}

void bridge_gpsim_set_apin_value(int pin, float value) {
    iopins[pin]->set_is_analog(true);
    iopins[pin]->set_Cth(5e-12);
    iopins[pin]->set_nodeVoltage(value);
}

void bridge_gpsim_step(void) {
    gpic->step_cycle();
    bp.clear_halt();
}

void bridge_gpsim_end(void) {
    CSimulationContext::GetContext()->Clear();
    // simulation_cleanup ();
    // Ugly hack to permit gpsim restart
    // gpsim don't support restart yet
    // https://sourceforge.net/p/gpsim/bugs/163/
    for (int i = 0; i < 1000; i++) {
        get_interface().remove_interface(i);
    }
}

char* bridge_gpsim_get_processor_list(char* buff, unsigned int size) {
    std::list<ProcessorConstructor*>::iterator processor_iterator;
    ProcessorConstructorList* pl = ProcessorConstructor::GetList();
    ProcessorConstructor* p;

    buff[0] = 0;
    // Print the name of each processor.

    for (processor_iterator = pl->begin(); processor_iterator != pl->end(); processor_iterator++) {
        p = *processor_iterator;

        strncat(buff, p->names[1], size);
        strncat(buff, ",", size);
    }

    return buff;
}

static int write_hex(unsigned char* mem, unsigned int size, unsigned short* config, unsigned int configsize,
                     unsigned int configaddr, const char* fname);

int bridge_gpsim_dump_memory(const char* fname) {
    unsigned char* mem;
    unsigned short* config;
    unsigned short val;
    unsigned int memsize = gpic->program_memory_size() * 2;
    unsigned int configsize = gpic->getConfigMemory()->getnConfigWords();
    unsigned int configaddr = 0x2007;

    mem = (unsigned char*)malloc(memsize);

    for (unsigned int i = 0; i < memsize; i += 2) {
        val = gpic->program_memory[i / 2]->get_value();
        mem[i + 1] = (val & 0xFF00) >> 8;
        mem[i] = (val & 0x00FF);
    }

    config = (unsigned short*)malloc(configsize * 2);

    for (unsigned int i = 0; i < configsize; i++) {
        if (gpic->getConfigMemory()->getConfigWord(i)) {
            config[i] = gpic->getConfigMemory()->getConfigWord(i)->getVal();
            // gpic->config_word_address() don´t work for PIC18F!
            configaddr = gpic->getConfigMemory()->getConfigWord(i)->ConfigWordAdd() - i;
        } else {
            config[i] = 0xFFFF;
        }
    }

    int ret = write_hex(mem, memsize, config, configsize, configaddr, fname);

    free(mem);
    free(config);
    return ret;
}

static int write_hex(unsigned char* mem, unsigned int size, unsigned short* config, unsigned int configsize,
                     unsigned int configaddr, const char* fname) {
    FILE* fout;
    unsigned char sum;
    unsigned char nb;
    unsigned int iaddr = 0;
    unsigned int i;
    char values[100];
    char tmp[200];

    fout = fopen_UTF8(fname, "w");

    if (fout) {
        // program memory
        nb = 0;
        sum = 0;
        for (i = 0; i < size; i++) {
            if (i == 0x00000)
                fprintf(fout, ":020000040000FA\n");
            if (i == 0x10000)
                fprintf(fout, ":020000040001F9\n");
            if (i == 0x20000)
                fprintf(fout, ":020000040002F8\n");
            if (i == 0x30000)
                fprintf(fout, ":020000040003F7\n");

            if (nb == 0) {
                iaddr = i & 0xFFFF;
                snprintf(values, 99, "%02X", mem[i]);
            } else {
                snprintf(tmp, 199, "%s%02X", values, mem[i]);
                strcpy(values, tmp);
            }

            nb++;
            sum += mem[i];

            if (nb == 16) {
                sum += nb;
                sum += (iaddr & 0x00FF);
                sum += ((iaddr & 0xFF00) >> 8);
                // printf("sum=%02X %02X %02X\n",sum,~sum,(~sum)+1);
                sum = (~sum) + 1;
                fprintf(fout, ":%02X%04X00%s%02X\n", nb, iaddr, values, sum);
                nb = 0;
                sum = 0;
            }
        }
        if (nb) {
            sum += nb;
            sum += (iaddr & 0x00FF);
            sum += ((iaddr & 0xFF00) >> 8);
            sum = (~sum) + 1;
            fprintf(fout, ":%02X%04X00%s%02X\n", nb, iaddr, values, sum);
        }

        // config
        nb = 0;
        sum = 0;

        if (configaddr > 0xFFFF) {
            fprintf(fout, ":020000040030CA\n");  // FOR PIC18F
            configaddr = 0;

            for (unsigned int b = 0; b < configsize; b += 2) {
                config[b / 2] = ((config[b + 1] & 0xFF) << 8) | (config[b] & 0xFF);
            }
            configsize = configsize / 2;
        }

        for (i = 0; i < configsize; i++) {
            if (nb == 0) {
                iaddr = (configaddr * 2) + (i * 2);
                sprintf(values, "%02X%02X", config[i] & 0x00FF, (config[i] & 0xFF00) >> 8);
            } else {
                sprintf(tmp, "%s%02X%02X", values, config[i] & 0x00FF, (config[i] & 0xFF00) >> 8);
                strcpy(values, tmp);
            }

            nb += 2;
            sum += config[i] & 0x00FF;
            sum += (config[i] & 0xFF00) >> 8;
            if (nb == 16) {
                sum += nb;
                sum += (iaddr & 0x00FF);
                sum += ((iaddr & 0xFF00) >> 8);
                // printf("sum=%02X %02X %02X\n",sum,~sum,(~sum)+1);
                sum = (~sum) + 1;
                fprintf(fout, ":%02X%04X00%s%02X\n", nb, iaddr, values, sum);
                nb = 0;
                sum = 0;
            }
        }
        if (nb) {
            sum += nb;
            sum += (iaddr & 0x00FF);
            sum += ((iaddr & 0xFF00) >> 8);
            sum = (~sum) + 1;
            fprintf(fout, ":%02X%04X00%s%02X\n", nb, iaddr, values, sum);
        }

        // end
        fprintf(fout, ":00000001FF\n");
        fclose(fout);
        return 0;  // no error
    } else {
        printf("ERROR: File not found!(%s)\n", fname);
        return -1;
    }
    return 0;  // no error
}