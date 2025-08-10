#include "peripheralmanager.h"
#include "storagemanager.h"

#if 0
#define UART_ID uart1
#define BAUD_RATE 115200
extern char string_a[100];
#endif

void PeripheralManager::initUSB(){
    const PeripheralOptions& peripheralOptions = Storage::getInstance().getPeripheralOptions();
#if 0
    sprintf(string_a, "en: %02d dp: %02d 5v: %02d order: %02d ", peripheralOptions.blockUSB0.enabled, peripheralOptions.blockUSB0.dp, peripheralOptions.blockUSB0.enable5v, peripheralOptions.blockUSB0.order);
    uart_puts(UART_ID, string_a);
    uart_puts(UART_ID, "\n");
#endif
    //en: 01 dp: 28 5v: -1 order: 00
    //if (peripheralOptions.blockUSB0.enabled) blockUSB0.setConfig(0, peripheralOptions.blockUSB0.dp, peripheralOptions.blockUSB0.enable5v, peripheralOptions.blockUSB0.order);
    blockUSB0.setConfig(0, 28, -1, 0);
}


void PeripheralManager::initI2C(){
    const PeripheralOptions& peripheralOptions = Storage::getInstance().getPeripheralOptions();
#if 0
    sprintf(string_a, "en: %02d sda: %02d scl: %02d speed: %02d ", peripheralOptions.blockI2C0.enabled, peripheralOptions.blockI2C0.sda, peripheralOptions.blockI2C0.scl, peripheralOptions.blockI2C0.speed);
    uart_puts(UART_ID, string_a);
    uart_puts(UART_ID, "\n");
#endif    
    //en: 01 sda: 00 scl: 01 speed: 400000
    //if (peripheralOptions.blockI2C0.enabled) blockI2C0.setConfig(0, peripheralOptions.blockI2C0.sda, peripheralOptions.blockI2C0.scl, peripheralOptions.blockI2C0.speed);
    blockI2C0.setConfig(0, 0, 1, 400000);
    if (peripheralOptions.blockI2C1.enabled) blockI2C1.setConfig(1, peripheralOptions.blockI2C1.sda, peripheralOptions.blockI2C1.scl, peripheralOptions.blockI2C1.speed); 
}

void PeripheralManager::initSPI(){
    const PeripheralOptions& peripheralOptions = Storage::getInstance().getPeripheralOptions();
    if (peripheralOptions.blockSPI0.enabled) blockSPI0.setConfig(0, peripheralOptions.blockSPI0.tx, peripheralOptions.blockSPI0.rx, peripheralOptions.blockSPI0.sck, peripheralOptions.blockSPI0.cs);
    if (peripheralOptions.blockSPI1.enabled) blockSPI1.setConfig(1, peripheralOptions.blockSPI1.tx, peripheralOptions.blockSPI1.rx, peripheralOptions.blockSPI1.sck, peripheralOptions.blockSPI1.cs);
}

PeripheralI2C* PeripheralManager::getI2C(uint8_t block) {
    if (block < NUM_I2CS) {
        return ((block == 0) ? &blockI2C0 : &blockI2C1);
    }
    return nullptr;
}

PeripheralSPI* PeripheralManager::getSPI(uint8_t block) {
    if (block < NUM_SPIS) {
        return ((block == 0) ? &blockSPI0 : &blockSPI1);
    }
    return nullptr;
}

PeripheralUSB* PeripheralManager::getUSB(uint8_t block) {
    if (block < NUM_USBS) {
        return ((block == 0) ? &blockUSB0 : &blockUSB0);
    }
    return nullptr;
}

bool PeripheralManager::isI2CEnabled(uint8_t block) {
    if (block < NUM_I2CS) {
        return (((block == 0) ? blockI2C0.configured : blockI2C1.configured));
    }
    return false;
}

bool PeripheralManager::isSPIEnabled(uint8_t block) {
    if (block < NUM_SPIS) {
        return (((block == 0) ? blockSPI0.configured : blockSPI1.configured));
    }
    return false;
}

bool PeripheralManager::isUSBEnabled(uint8_t block) {
    if (block < NUM_USBS) {
//        return (((block == 0) ? blockUSB0.configured : false));
        return 1; //USB enabled
    }
    return false;
}

PeripheralI2CScanResult PeripheralManager::scanForI2CDevice(std::vector<uint8_t> addressList) {
    PeripheralI2CScanResult scanResult = {
        .address = -1,
        .block = 0
    };
    
    for (uint8_t block = 0; block < NUM_I2CS; block++) {
        if (isI2CEnabled(block)) {
            PeripheralI2C* i2c = getI2C(block);

            for (uint8_t i = 0; i < addressList.size(); i++) {
                if (!((addressList[i] & 0x78) == 0 || (addressList[i] & 0x78) == 0x78)) {
                    uint8_t result = i2c->test(addressList[i]);

                    if (result) {
                        scanResult.address = addressList[i];
                        scanResult.block = block;
                        return scanResult;
                    }
                }
            }
        }
    }
    return scanResult;
}