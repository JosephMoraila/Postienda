#include "gui/TicketFrame/PrintMenu/PrintMenu.hpp"
#ifdef __linux__
#include <cups/cups.h>
#include <vector>
#include <libusb-1.0/libusb.h>
#include <iostream>
#include <string>
#include <dirent.h>

void PrintMenu::GetPrinterNamesLinux()
{
    std::vector<std::string> printerNames;

    // ==========================================================
    // 1. Impresoras registradas en CUPS
    // ==========================================================
    int numDests = 0;
    cups_dest_t* dests = nullptr;
    numDests = cupsGetDests2(CUPS_HTTP_DEFAULT, &dests);

    if (numDests > 0 && dests != nullptr)
    {
        for (int i = 0; i < numDests; i++) { printerNames.push_back(dests[i].name);
        }
        cupsFreeDests(numDests, dests);
    }

    // ==========================================================
    // 2. Impresoras del kernel (usblp → /dev/usb/lp0)
    // ==========================================================
    DIR* dir = opendir("/dev/usb");
    if (dir)
    {
        struct dirent* ent;
        while ((ent = readdir(dir)) != nullptr)
        {
            std::string name = ent->d_name;

            // archivo empieza con "lp"
            if (name.rfind("lp", 0) == 0) printerNames.push_back("/dev/usb/" + name);
        }
        closedir(dir);
    }

    // ==========================================================
    // 3. Impresoras USB RAW (solo VID:PID) — SOLO si no hay lp0
    // ==========================================================
    bool hasLp = false;
    for (auto& p : printerNames)
        if (p.find("/dev/usb/lp") != std::string::npos) hasLp = true;

    if (!hasLp)
    {
        libusb_context* ctx = nullptr;
        libusb_init(&ctx);

        libusb_device** devs;
        ssize_t cnt = libusb_get_device_list(ctx, &devs);

        for (ssize_t i = 0; i < cnt; i++) {
            libusb_device_descriptor desc;
            if (libusb_get_device_descriptor(devs[i], &desc) == 0) {
                bool isPrinter = false;

                if (desc.bDeviceClass == 7 || desc.bDeviceSubClass == 1 || desc.bDeviceClass == 0) {
                    libusb_config_descriptor* config;
                    if (libusb_get_active_config_descriptor(devs[i], &config) == 0) {
                        for (uint8_t j = 0; j < config->bNumInterfaces; j++) {
                            if (config->interface[j].altsetting[0].bInterfaceClass == 7) isPrinter = true;
                        }
                        libusb_free_config_descriptor(config);
                    }
                }

                if (isPrinter) {
                    char buf[32];
                    snprintf(buf, sizeof(buf), "USB %04x:%04x", desc.idVendor, desc.idProduct);
                    printerNames.push_back(std::string(buf));
                }
            }
        }

        libusb_free_device_list(devs, 1);
        libusb_exit(ctx);
    }

    for (const std::string& printer : printerNames) this->printerNames.Add(printer);
}
#endif