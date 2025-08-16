
/*****************************************************************************
 *   Ledger App Boilerplate.
 *   (c) 2020 Ledger SAS.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *****************************************************************************/

#include "os.h"
#include "glyphs.h"
#include "nbgl_use_case.h"

#include "globals.h"
#include "menu.h"
#include "display.h"

//  -----------------------------------------------------------
//  ----------------------- HOME PAGE -------------------------
//  -----------------------------------------------------------

void app_quit(void) {
    // exit app here
    os_sched_exit(-1);
}

//  -----------------------------------------------------------
//  --------------------- SETTINGS MENU -----------------------
//  -----------------------------------------------------------
#define SETTING_INFO_NB 3
static const char* const INFO_TYPES[SETTING_INFO_NB] = {"Version", "Owner", "Declaration"};
static const char* const INFO_CONTENTS[SETTING_INFO_NB] = {APPVERSION, "@tanteikg", "I am writing this of free and sound mind. My 2 beneficiaries are Alice and Bob"};

// settings switches definitions
enum { DUMMY_SWITCH_1_TOKEN = FIRST_USER_TOKEN, DUMMY_SWITCH_2_TOKEN };
enum { DUMMY_SWITCH_1_ID = 0, DUMMY_SWITCH_2_ID, SETTINGS_SWITCHES_NB };

static nbgl_contentSwitch_t switches[SETTINGS_SWITCHES_NB] = {0};

static const nbgl_contentInfoList_t infoList = {
    .nbInfos = SETTING_INFO_NB,
    .infoTypes = INFO_TYPES,
    .infoContents = INFO_CONTENTS,
};

static uint8_t initSettingPage;
static void review_warning_choice(bool confirm);
static void controls_callback(int token, uint8_t index, int page);

// settings menu definition
#define SETTING_CONTENTS_NB 1
static const nbgl_content_t contents[SETTING_CONTENTS_NB] = {
    {.type = SWITCHES_LIST,
     .content.switchesList.nbSwitches = SETTINGS_SWITCHES_NB,
     .content.switchesList.switches = switches,
     .contentActionCallback = controls_callback}};

static const nbgl_genericContents_t settingContents = {.callbackCallNeeded = false,
                                                       .contentsList = contents,
                                                       .nbContents = SETTING_CONTENTS_NB};

// callback for setting warning choice
static void review_warning_choice(bool confirm) {
    uint8_t switch_value;
    if (confirm) {
        // toggle the switch value
        switch_value = !N_storage.dummy2_allowed;
        switches[DUMMY_SWITCH_2_ID].initState = (nbgl_state_t) switch_value;
        // store the new setting value in NVM
        nvm_write((void*) &N_storage.dummy2_allowed, &switch_value, 1);
    }

    // Reset setting menu to the right page
    nbgl_useCaseHomeAndSettings(APPNAME,
                                &ICON_APP_HOME,
                                "For the future",
                                initSettingPage,
                                &settingContents,
                                &infoList,
                                NULL,
                                app_quit);
}

static void controls_callback(int token, uint8_t index, int page) {
    UNUSED(index);

    initSettingPage = page;

    uint8_t switch_value;
    if (token == DUMMY_SWITCH_1_TOKEN) {
        // Dummy 1 switch touched
        // toggle the switch value
        switch_value = !N_storage.dummy1_allowed;
        switches[DUMMY_SWITCH_1_ID].initState = (nbgl_state_t) switch_value;
        // store the new setting value in NVM
        nvm_write((void*) &N_storage.dummy1_allowed, &switch_value, 1);
    } else if (token == DUMMY_SWITCH_2_TOKEN) {
        // Dummy 2 switch touched

        // in this example we display a warning when the user wants
        // to activate the dummy 2 setting
        if (!N_storage.dummy2_allowed) {
            // Display the warning message and ask the user to confirm
            nbgl_useCaseChoice(&ICON_APP_WARNING,
                               "Bob",
                               "Are you sure to\ninclude Bob\nin beneficiary?",
                               "I understand, confirm",
                               "Cancel",
                               review_warning_choice);
        } else {
            // toggle the switch value
            switch_value = !N_storage.dummy2_allowed;
            switches[DUMMY_SWITCH_2_ID].initState = (nbgl_state_t) switch_value;
            // store the new setting value in NVM
            nvm_write((void*) &N_storage.dummy2_allowed, &switch_value, 1);
        }
    }
}

// home page definition
void ui_menu_main(void) {
    // Initialize switches data
    switches[DUMMY_SWITCH_1_ID].initState = (nbgl_state_t) N_storage.dummy1_allowed;
    switches[DUMMY_SWITCH_1_ID].text = "Beneficiary 1";
    switches[DUMMY_SWITCH_1_ID].subText = "Include Alice\nin beneficiary";
    switches[DUMMY_SWITCH_1_ID].token = DUMMY_SWITCH_1_TOKEN;
#ifdef HAVE_PIEZO_SOUND
    switches[DUMMY_SWITCH_1_ID].tuneId = TUNE_TAP_CASUAL;
#endif

    switches[DUMMY_SWITCH_2_ID].initState = (nbgl_state_t) N_storage.dummy2_allowed;
    switches[DUMMY_SWITCH_2_ID].text = "Beneficiary 2";
    switches[DUMMY_SWITCH_2_ID].subText = "Include Bob\nin beneficiary";
    switches[DUMMY_SWITCH_2_ID].token = DUMMY_SWITCH_2_TOKEN;
#ifdef HAVE_PIEZO_SOUND
    switches[DUMMY_SWITCH_2_ID].tuneId = TUNE_TAP_CASUAL;
#endif

    nbgl_useCaseHomeAndSettings(APPNAME,
                                &ICON_APP_HOME,
                                "For the future",
                                INIT_HOME_PAGE,
                                &settingContents,
                                &infoList,
                                NULL,
                                app_quit);
}
