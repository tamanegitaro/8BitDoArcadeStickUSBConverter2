#include "addons/gamepad_usb_host_listener.h"
#include "drivermanager.h"
#include "storagemanager.h"
#include "class/hid/hid_host.h"
#include "drivers/ps4/PS4Driver.h"

#include "system.h"
#include "gamepad.h"

void GamepadUSBHostListener::setup() {
    _controller_host_enabled = false;
}

void GamepadUSBHostListener::process() {
    Gamepad *gamepad = Storage::getInstance().GetGamepad();
    gamepad->hasAnalogTriggers = true;
    gamepad->hasLeftAnalogStick = true;
    gamepad->hasRightAnalogStick = true;
    gamepad->state.dpad     |= _controller_host_state.dpad;
    gamepad->state.buttons  |= _controller_host_state.buttons;
    gamepad->state.lx       = _controller_host_state.lx;
    gamepad->state.ly       = _controller_host_state.ly;
    gamepad->state.rx       = _controller_host_state.rx;
    gamepad->state.ry       = _controller_host_state.ry;
    gamepad->state.rt       = _controller_host_state.rt;
    gamepad->state.lt       = _controller_host_state.lt;
}

void GamepadUSBHostListener::mount(uint8_t dev_addr, uint8_t instance, uint8_t const* desc_report, uint16_t desc_len) {
    _controller_host_enabled = true;
    tuh_vid_pid_get(dev_addr, &controller_vid, &controller_pid);
}

void GamepadUSBHostListener::unmount(uint8_t dev_addr) {
    _controller_host_enabled = false;
    controller_pid = 0x00;
    controller_vid = 0x00;
}

void GamepadUSBHostListener::report_received(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len) {
    // if a hid device hasn't been mounted
    if ( _controller_host_enabled == false ) return;

    // Interface protocol (hid_interface_protocol_enum_t)
    uint8_t const itf_protocol = tuh_hid_interface_protocol(dev_addr, instance);

    // stop execution if a keyboard or mouse is mounted
    if ( itf_protocol == HID_ITF_PROTOCOL_KEYBOARD ) return;

    process_ctrlr_report(dev_addr, report, len);
}

void GamepadUSBHostListener::process_ctrlr_report(uint8_t dev_addr, uint8_t const* report, uint16_t len) {
    uint16_t joystick_mid = GAMEPAD_JOYSTICK_MID;
    _controller_host_state.buttons = 0;
    _controller_host_state.dpad = 0;
    _controller_host_state.lx = joystick_mid;
    _controller_host_state.ly = joystick_mid;
    _controller_host_state.rx = joystick_mid;
    _controller_host_state.ry = joystick_mid;

    switch(controller_pid)
    {
        case 0x2009:             // 8Bitdo Arcade Stick Switch mode
            //PID 2009, VID 057E
            process_8bitdoarcade(report);
            break;
        case DS4_ORG_PRODUCT_ID: // Sony Dualshock 4 controller
        case DS4_PRODUCT_ID:     // Sony Dualshock 4 controller
        case PS4_PRODUCT_ID:     // Razer Panthera
            process_ds4(report);
            break;
        case 0x9400:             // Google Stadia controller
            process_stadia(report);
            break;

        case 0x0510:             // pre-2015 Ultrakstik 360
        case 0x0511:             // Ultrakstik 360
            process_ultrastik360(report);
            break;
        default:
            break;
    }
}

uint16_t GamepadUSBHostListener::map(uint8_t x, uint8_t in_min, uint8_t in_max, uint16_t out_min, uint16_t out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


//check if different than 2
bool GamepadUSBHostListener::diff_than_2(uint8_t x, uint8_t y) {
    return (x - y > 2) || (y - x > 2);
}

// check if 2 reports are different enough
bool GamepadUSBHostListener::diff_report(dualshock4_t const* rpt1, dualshock4_t const* rpt2) {
    bool result;

    // x, y, z, rz must different than 2 to be counted
    result = diff_than_2(rpt1->GD_GamePadPointerX, rpt2->GD_GamePadPointerX) || 
            diff_than_2(rpt1->GD_GamePadPointerY , rpt2->GD_GamePadPointerY ) ||
            diff_than_2(rpt1->GD_GamePadPointerZ, rpt2->GD_GamePadPointerZ) || 
            diff_than_2(rpt1->GD_GamePadPointerRz, rpt2->GD_GamePadPointerRz);

    // check the rest with mem compare
    result |= memcmp(&rpt1->GD_GamePadPointerRz + 1, &rpt2->GD_GamePadPointerRz + 1, sizeof(dualshock4_t)-6);

    return result;
}

void GamepadUSBHostListener::process_8bitdoarcade_hotkey(void)
{
  /* If A1, S1, S2 are pressed at same time, activate hotkey */
  if((_controller_host_state.buttons & GAMEPAD_MASK_A1) && (_controller_host_state.buttons & GAMEPAD_MASK_S2) && (_controller_host_state.buttons & GAMEPAD_MASK_S1))
  {

    Gamepad * gamepad = Storage::getInstance().GetGamepad();
    InputMode inputMode = gamepad->getOptions().inputMode;
    InputMode previnputMode = inputMode;

    const GamepadOptions& gamepadOptions = Storage::getInstance().getGamepadOptions();

    if(_controller_host_state.buttons & GAMEPAD_MASK_B1) inputMode = (InputMode)gamepadOptions.inputModeB1; /* B1 XBOX */
    if(_controller_host_state.buttons & GAMEPAD_MASK_B2) inputMode = (InputMode)gamepadOptions.inputModeB2; /* B2 PCEMINI */
    if(_controller_host_state.buttons & GAMEPAD_MASK_R2) inputMode = (InputMode)gamepadOptions.inputModeR2; /* R2 NEOGEO */
    if(_controller_host_state.buttons & GAMEPAD_MASK_L2) inputMode = (InputMode)gamepadOptions.inputModeL2; /* L2 EGRETII */
    if(_controller_host_state.buttons & GAMEPAD_MASK_B3) inputMode = (InputMode)gamepadOptions.inputModeB3; /* B3 PS3 */
    if(_controller_host_state.buttons & GAMEPAD_MASK_B4) inputMode = (InputMode)gamepadOptions.inputModeB4; /* B4 PS4 */
    if(_controller_host_state.buttons & GAMEPAD_MASK_R1) inputMode = (InputMode)gamepadOptions.inputModeR1; /* R1 PSCLASSIC */
    if(_controller_host_state.buttons & GAMEPAD_MASK_L1) inputMode = (InputMode)gamepadOptions.inputModeL1; /* L1 MDMINI/ASTRO */

    if(inputMode == 0xFF) //If Input mode is "No mode selected", do nothing
    {
        inputMode = previnputMode;
    }
    else
    {
        if (inputMode != gamepad->getOptions().inputMode) {
        // Save the changed input mode
        gamepad->setInputMode(inputMode);
        //EventManager::getInstance().triggerEvent(new GPStorageSaveEvent(true));
        Storage::getInstance().save(true);

        sleep_ms(100);

        System::reboot(System::BootMode::DEFAULT); /* Restart GP2040-CE */
        while(1);
        /* Wait for while */
        }
    }
  }
}

void GamepadUSBHostListener::process_8bitdoarcade(uint8_t const* report)
{
    ebitdoarcade_report_t controller_report;

    uint8_t const report_id = report[0];

    if (report_id == 0x30) {

        memcpy(&controller_report, report, sizeof(controller_report));
        _controller_host_state.lx = map(controller_report.GD_GamePadPointerXl + (controller_report.GD_GamePadPointerXh * 16),0 ,255,GAMEPAD_JOYSTICK_MIN,GAMEPAD_JOYSTICK_MAX);
        _controller_host_state.ly = map(0xff - controller_report.GD_GamePadPointerY,0 ,255,GAMEPAD_JOYSTICK_MIN,GAMEPAD_JOYSTICK_MAX);
        _controller_host_state.rx = map(controller_report.GD_GamePadPointerZl + (controller_report.GD_GamePadPointerZh * 16),0 ,255,GAMEPAD_JOYSTICK_MIN,GAMEPAD_JOYSTICK_MAX);
        _controller_host_state.ry = map(0xff - controller_report.GD_GamePadPointerRz,0 ,255,GAMEPAD_JOYSTICK_MIN,GAMEPAD_JOYSTICK_MAX);

        if (controller_report.BTN_GamePadButton1 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_B3;
        if (controller_report.BTN_GamePadButton2 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_B4;
        if (controller_report.BTN_GamePadButton3 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_B1;
        if (controller_report.BTN_GamePadButton4 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_B2;
        if (controller_report.BTN_GamePadButton5 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_R1;
        if (controller_report.BTN_GamePadButton6 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_R2;
        if (controller_report.BTN_GamePadButton7 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_S1;
        if (controller_report.BTN_GamePadButton8 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_S2;
        if (controller_report.BTN_GamePadButton9 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_R3;
        if (controller_report.BTN_GamePadButton10 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_L3;
        if (controller_report.BTN_GamePadButton11 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_A1;
        if (controller_report.BTN_GamePadButton12 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_L1;
        if (controller_report.BTN_GamePadButton13 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_L2;

        if (controller_report.GD_GamePadDpadUp == 1) _controller_host_state.dpad |= GAMEPAD_MASK_UP;
        if (controller_report.GD_GamePadDpadRight == 1) _controller_host_state.dpad |= GAMEPAD_MASK_RIGHT;
        if (controller_report.GD_GamePadDpadDown == 1) _controller_host_state.dpad |= GAMEPAD_MASK_DOWN;
        if (controller_report.GD_GamePadDpadLeft == 1) _controller_host_state.dpad |= GAMEPAD_MASK_LEFT;
    }

    process_8bitdoarcade_hotkey();
}

void GamepadUSBHostListener::process_ds4(uint8_t const* report)
{
    dualshock4_t controller_report;

    // previous report used to compare for changes
    static dualshock4_t prev_report = { 0 };

    uint8_t const report_id = report[0];

    if (report_id == 1) {

        memcpy(&controller_report, report, sizeof(controller_report));

        if ( diff_report(&prev_report, &controller_report) ) {
            _controller_host_state.lx = map(controller_report.GD_GamePadPointerX,0 ,255,GAMEPAD_JOYSTICK_MIN,GAMEPAD_JOYSTICK_MAX);
            _controller_host_state.ly = map(controller_report.GD_GamePadPointerY,0 ,255,GAMEPAD_JOYSTICK_MIN,GAMEPAD_JOYSTICK_MAX);
            _controller_host_state.rx = map(controller_report.GD_GamePadPointerZ,0 ,255,GAMEPAD_JOYSTICK_MIN,GAMEPAD_JOYSTICK_MAX);
            _controller_host_state.ry = map(controller_report.GD_GamePadPointerRz,0 ,255,GAMEPAD_JOYSTICK_MIN,GAMEPAD_JOYSTICK_MAX);
            _controller_host_state.lt = controller_report.SIM_GamePadBrake;
            _controller_host_state.rt = controller_report.SIM_GamePadAccelerator;

            if (controller_report.BTN_GamePadButton14 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_A2;
            if (controller_report.BTN_GamePadButton9 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_S1;
            if (controller_report.BTN_GamePadButton12 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_R3;
            if (controller_report.BTN_GamePadButton11 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_L3;
            if (controller_report.BTN_GamePadButton13 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_A1;
            if (controller_report.BTN_GamePadButton10 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_S2;
            if (controller_report.BTN_GamePadButton6 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_R1;
            if (controller_report.BTN_GamePadButton5 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_L1;
            if (controller_report.BTN_GamePadButton4 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_B4;
            if (controller_report.BTN_GamePadButton3 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_B2;
            if (controller_report.BTN_GamePadButton2 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_B1;
            if (controller_report.BTN_GamePadButton1 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_B3;
            if (controller_report.BTN_GamePadButton8 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_R2;
            if (controller_report.BTN_GamePadButton7 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_L2;

            if (controller_report.GD_GamePadHatSwitch == 0) _controller_host_state.dpad |= GAMEPAD_MASK_UP;
            if (controller_report.GD_GamePadHatSwitch == 1) _controller_host_state.dpad |= GAMEPAD_MASK_UP | GAMEPAD_MASK_RIGHT;
            if (controller_report.GD_GamePadHatSwitch == 2) _controller_host_state.dpad |= GAMEPAD_MASK_RIGHT;
            if (controller_report.GD_GamePadHatSwitch == 3) _controller_host_state.dpad |= GAMEPAD_MASK_RIGHT | GAMEPAD_MASK_DOWN;
            if (controller_report.GD_GamePadHatSwitch == 4) _controller_host_state.dpad |= GAMEPAD_MASK_DOWN;
            if (controller_report.GD_GamePadHatSwitch == 5) _controller_host_state.dpad |= GAMEPAD_MASK_DOWN | GAMEPAD_MASK_LEFT;
            if (controller_report.GD_GamePadHatSwitch == 6) _controller_host_state.dpad |= GAMEPAD_MASK_LEFT;
            if (controller_report.GD_GamePadHatSwitch == 7) _controller_host_state.dpad |= GAMEPAD_MASK_LEFT | GAMEPAD_MASK_UP;
        }
    }

    prev_report = controller_report;
}

void GamepadUSBHostListener::process_stadia(uint8_t const* report) {
    google_stadia_report_t controller_report;

    memcpy(&controller_report, report, sizeof(controller_report));

    _controller_host_state.lx = map(controller_report.GD_GamePadPointerX ,1,255,GAMEPAD_JOYSTICK_MIN,GAMEPAD_JOYSTICK_MAX);
    _controller_host_state.ly = map(controller_report.GD_GamePadPointerY,1 ,255,GAMEPAD_JOYSTICK_MIN,GAMEPAD_JOYSTICK_MAX);
    _controller_host_state.rx = map(controller_report.GD_GamePadPointerZ,1 ,255,GAMEPAD_JOYSTICK_MIN,GAMEPAD_JOYSTICK_MAX);
    _controller_host_state.ry = map(controller_report.GD_GamePadPointerRz,1 ,255,GAMEPAD_JOYSTICK_MIN,GAMEPAD_JOYSTICK_MAX);
    _controller_host_state.lt = controller_report.SIM_GamePadBrake;
    _controller_host_state.rt = controller_report.SIM_GamePadAccelerator;

    if (controller_report.BTN_GamePadButton18 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_A2;
    if (controller_report.BTN_GamePadButton11 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_S1;
    if (controller_report.BTN_GamePadButton15 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_R3;
    if (controller_report.BTN_GamePadButton14 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_L3;
    if (controller_report.BTN_GamePadButton13 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_A1;
    if (controller_report.BTN_GamePadButton12 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_S2;
    if (controller_report.BTN_GamePadButton8 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_R1;
    if (controller_report.BTN_GamePadButton7 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_L1;
    if (controller_report.BTN_GamePadButton5 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_B4;
    if (controller_report.BTN_GamePadButton4 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_B3;
    if (controller_report.BTN_GamePadButton2 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_B2;
    if (controller_report.BTN_GamePadButton1 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_B1;
    if (controller_report.BTN_GamePadButton19 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_R2;
    if (controller_report.BTN_GamePadButton20 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_L2;

    if (controller_report.GD_GamePadHatSwitch == 0) _controller_host_state.dpad |= GAMEPAD_MASK_UP;
    if (controller_report.GD_GamePadHatSwitch == 1) _controller_host_state.dpad |= GAMEPAD_MASK_UP | GAMEPAD_MASK_RIGHT;
    if (controller_report.GD_GamePadHatSwitch == 2) _controller_host_state.dpad |= GAMEPAD_MASK_RIGHT;
    if (controller_report.GD_GamePadHatSwitch == 3) _controller_host_state.dpad |= GAMEPAD_MASK_RIGHT | GAMEPAD_MASK_DOWN;
    if (controller_report.GD_GamePadHatSwitch == 4) _controller_host_state.dpad |= GAMEPAD_MASK_DOWN;
    if (controller_report.GD_GamePadHatSwitch == 5) _controller_host_state.dpad |= GAMEPAD_MASK_DOWN | GAMEPAD_MASK_LEFT;
    if (controller_report.GD_GamePadHatSwitch == 6) _controller_host_state.dpad |= GAMEPAD_MASK_LEFT;
    if (controller_report.GD_GamePadHatSwitch == 7) _controller_host_state.dpad |= GAMEPAD_MASK_LEFT | GAMEPAD_MASK_UP;
}

void GamepadUSBHostListener::process_ultrastik360(uint8_t const* report) {

    ultrastik360_t controller_report;

    memcpy(&controller_report, report, sizeof(controller_report));

    _controller_host_state.lx = map(controller_report.GD_GamePadPointerX, 0, 255, GAMEPAD_JOYSTICK_MIN,GAMEPAD_JOYSTICK_MAX);
    _controller_host_state.ly = map(controller_report.GD_GamePadPointerY, 0, 255, GAMEPAD_JOYSTICK_MIN,GAMEPAD_JOYSTICK_MAX);

    if (controller_report.BTN_GamePadButton1 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_B1;
    if (controller_report.BTN_GamePadButton2 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_B2;
    if (controller_report.BTN_GamePadButton3 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_B3;
    if (controller_report.BTN_GamePadButton4 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_B4;
    if (controller_report.BTN_GamePadButton5 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_L1;
    if (controller_report.BTN_GamePadButton6 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_L2;
    if (controller_report.BTN_GamePadButton7 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_R1;
    if (controller_report.BTN_GamePadButton8 == 1) _controller_host_state.buttons |= GAMEPAD_MASK_R2;
}