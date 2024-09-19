// #ifndef BCOMFY_SETTINGS_H
// #define BCOMFY_SETTINGS_H

// #include <Arduino.h>
// #include <LinkedList.h>

// LinkedList<SettingProperty> settingProperties;

// enum SettingPropertyType {
//     STRING,
//     BOOL,
//     INT,
//     FLOAT
// };

// class SettingProperty {
//     public:
//         String key;
//         String name;
//         SettingPropertyType type;

//         SettingProperty(String key, String name, SettingPropertyType type) {
//             this->key = key;
//             this->name = name;
//             this->type = type;
//             settingProperties.add(this);
//         }
// };

// SettingProperty setting_emergencyNumber("emergencyNumber", "Telefono Emergencia", SettingPropertyType::STRING);
// SettingProperty setting_ledColor("ledColor", "Color LEDs", SettingPropertyType::STRING);
// SettingProperty setting_beep("beep", "Buzzer", SettingPropertyType::BOOL);
// SettingProperty setting_vibrate("vibrate", "Vibrar", SettingPropertyType::BOOL);
// SettingProperty setting_volume("volume", "Volumen", SettingPropertyType::INT);

// #endif