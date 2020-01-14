#pragma once

#include <driver_interface.h>

int sata_init();
MODULE_INIT(sata_init);

int sata_exit();
MODULE_EXIT(sata_exit);