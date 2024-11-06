#pragma once

#include "wled.h"

/*
 * UsermodBlaqVoidARPalette provides controls for audio-reactive color palettes
 * It allows runtime adjustment of:
 * - Bass threshold for triggering
 * - Red color component ranges (min, mid, max)
 * - Accent color (RGB) and intensity
 * 
 * Installation:
 * 1. Copy this file to the /wled00/usermods/ directory
 * 2. Register the usermod in usermods_list.cpp:
 *    #include "usermod_blaqvoid_ar_palette.h"
 *    void registerUsermods() {
 *      usermods.add(new UsermodBlaqVoidARPalette("AR Palette", true));
 *    }
 */
class UsermodBlaqVoidARPalette : public Usermod {
  private:
    // Audio threshold
    uint8_t bass_threshold = 128;  // Default middle value for bass detection

    // Red component ranges
    uint8_t red_min = 0;    // Minimum red value in palette
    uint8_t red_mid = 127;  // Middle red value
    uint8_t red_max = 255;  // Maximum red value

    // Accent color components
    uint8_t accent_r = 0;   // Red component of accent color
    uint8_t accent_g = 0;   // Green component
    uint8_t accent_b = 255; // Blue component (default blue accent)
    uint8_t accent_amount = 128; // Intensity of accent color

    // moar tweakz
    float smoothingFactor = 0.5f;  // Default smoothing factor
    float fractionControl = 1.0f;  // Default value for standard interpolation speed

    // Getter methods for external access
    public:
      uint8_t getBassThreshold() { return bass_threshold; }
      uint8_t getRedMin() { return red_min; }
      uint8_t getRedMid() { return red_mid; }
      uint8_t getRedMax() { return red_max; }
      uint8_t getAccentR() { return accent_r; }
      uint8_t getAccentG() { return accent_g; }
      uint8_t getAccentB() { return accent_b; }
      uint8_t getAccentAmount() { return accent_amount; }
      float getSmoothingFactor() { return smoothingFactor; }
      float getFractionControl() { return fractionControl; }

      // Static instance getter for accessing from other code
      static UsermodBlaqVoidARPalette* getInstance() {
        return (UsermodBlaqVoidARPalette*) usermods.lookup(USERMOD_ID_BLAQVOID_AR);
      }

    // Constructor
    UsermodBlaqVoidARPalette(const char *name, bool enabled):Usermod(name, enabled) {}

    /*
     * setup() is called once at boot. WiFi is not yet connected at this point.
     * You can use it to initialize variables, sensors or similar.
     */
    void setup() {
      // Nothing to initialize for this usermod
      initDone = true;
    }

    /*
     * loop() is called continuously. Here you can check for events, read sensors, etc.
     */
    void loop() {
      if (!enabled || strip.isUpdating()) return;
      
      // This usermod doesn't need continuous processing
      // It just provides parameter storage and UI
    }

    /*
     * addToConfig() can be used to add custom persistent settings to the cfg.json file in the "um" (usermod) object.
     * It will be called by WLED when settings are actually saved (for example, LED settings are saved)
     */
    void addToConfig(JsonObject& root) {
      Usermod::addToConfig(root); 
      JsonObject top = root[FPSTR(_name)];

      // Save all parameters
      top["bass_threshold"] = bass_threshold;
      top["red_min"] = red_min;
      top["red_mid"] = red_mid;
      top["red_max"] = red_max;
      top["accent_r"] = accent_r;
      top["accent_g"] = accent_g;
      top["accent_b"] = accent_b;
      top["accent_amount"] = accent_amount;
      top["smoothingFactor"] = smoothingFactor;
      top["fractionControl"] = fractionControl; 
    }

    /*
     * readFromConfig() can be used to read back the custom settings you added with addToConfig().
     * This is called by WLED when settings are loaded (currently this only happens once immediately after boot)
     */
    bool readFromConfig(JsonObject& root) {
      bool configComplete = Usermod::readFromConfig(root);
      JsonObject top = root[FPSTR(_name)];

      // Read all parameters, with defaults if not found
      configComplete &= getJsonValue(top["bass_threshold"], bass_threshold, 128);
      configComplete &= getJsonValue(top["red_min"], red_min, 0);
      configComplete &= getJsonValue(top["red_mid"], red_mid, 127);
      configComplete &= getJsonValue(top["red_max"], red_max, 255);
      configComplete &= getJsonValue(top["accent_r"], accent_r, 0);
      configComplete &= getJsonValue(top["accent_g"], accent_g, 0);
      configComplete &= getJsonValue(top["accent_b"], accent_b, 255);
      configComplete &= getJsonValue(top["accent_amount"], accent_amount, 128);

      configComplete &= getJsonValue(top["smoothingFactor"], smoothingFactor, .5 );
      configComplete &= getJsonValue(top["fractionControl"], fractionControl, 1.0);
      return configComplete;
    }

    /*
     * addToJsonInfo() can be used to add custom entries to the /json/info part of the JSON API.
     * Creating an "u" object allows you to add custom key/value pairs to the Info section of the WLED web UI.
     */
    void addToJsonInfo(JsonObject& root) {
      JsonObject user = root["u"];
      if (user.isNull()) user = root.createNestedObject("u");

      // Add bass threshold info
      JsonArray thresh = user.createNestedArray("Bass Threshold");
      thresh.add(bass_threshold);
      
      // Add red component ranges
      JsonArray redRange = user.createNestedArray("Red Range");
      redRange.add("Min:");
      redRange.add(red_min);
      redRange.add("Mid:");
      redRange.add(red_mid);
      redRange.add("Max:");
      redRange.add(red_max);
      
      // Add accent color info
      JsonArray accent = user.createNestedArray("Accent Color");
      accent.add("R:");
      accent.add(accent_r);
      accent.add("G:");
      accent.add(accent_g);
      accent.add("B:");
      accent.add(accent_b);
      accent.add("Intensity:");
      accent.add(accent_amount);

      JsonArray arraysmoothingfactor = user.createNestedArray("Smoothing Factor:");
      arraysmoothingfactor.add(smoothingFactor);

      JsonArray arrayfractioncontrol = user.createNestedArray("Interpolation Speed:");
      arrayfractioncontrol.add(fractionControl);

    }

    /*
     * addToJsonState() can be used to add custom entries to the /json/state part of the JSON API (state object).
     * Values in the state object may be modified by connected clients
     */
    void addToJsonState(JsonObject& root) {
      if (!initDone || !enabled) return;

      JsonObject usermod = root[FPSTR(_name)];
      if (usermod.isNull()) usermod = root.createNestedObject(FPSTR(_name));

      usermod["bass_threshold"] = bass_threshold;
      usermod["red_min"] = red_min;
      usermod["red_mid"] = red_mid;
      usermod["red_max"] = red_max;
      usermod["accent_r"] = accent_r;
      usermod["accent_g"] = accent_g;
      usermod["accent_b"] = accent_b;
      usermod["accent_amount"] = accent_amount;
      usermod["smoothingFactor"] = smoothingFactor;
      usermod["fractionControl"] = fractionControl;
    }

    /*
     * readFromJsonState() can be used to receive data clients send to the /json/state part of the JSON API (state object).
     * Values in the state object may be modified by connected clients
     */
    void readFromJsonState(JsonObject& root) {
      if (!initDone) return;

      JsonObject usermod = root[FPSTR(_name)];
      if (!usermod.isNull()) {
        bass_threshold = usermod["bass_threshold"] | bass_threshold;
        red_min = usermod["red_min"] | red_min;
        red_mid = usermod["red_mid"] | red_mid;
        red_max = usermod["red_max"] | red_max;
        accent_r = usermod["accent_r"] | accent_r;
        accent_g = usermod["accent_g"] | accent_g;
        accent_b = usermod["accent_b"] | accent_b;
        accent_amount = usermod["accent_amount"] | accent_amount;
        smoothingFactor = usermod["smoothingFactor"] | smoothingFactor;
        fractionControl = usermod["fractionControl"] | fractionControl;
      }
    }

    /*
     * appendConfigData() can be used to add custom data to the config page
     */
    void appendConfigData() {
      // Add info text for all parameters
      
      oappend(SET_F("addInfo('")); oappend(String(FPSTR(_name)).c_str()); 
      oappend(SET_F(":bass_threshold")); oappend(SET_F("',1,'Threshold for bass detection (0-255)');"));

      oappend(SET_F("addInfo('")); oappend(String(FPSTR(_name)).c_str()); 
      oappend(SET_F(":red_min")); oappend(SET_F("',1,'Minimum red value in palette (0-255)');"));

      oappend(SET_F("addInfo('")); oappend(String(FPSTR(_name)).c_str()); 
      oappend(SET_F(":red_mid")); oappend(SET_F("',1,'Middle red value in palette (0-255)');"));

      oappend(SET_F("addInfo('")); oappend(String(FPSTR(_name)).c_str()); 
      oappend(SET_F(":red_max")); oappend(SET_F("',1,'Maximum red value in palette (0-255)');"));

      oappend(SET_F("addInfo('")); oappend(String(FPSTR(_name)).c_str()); 
      oappend(SET_F(":accent_r")); oappend(SET_F("',1,'Red component of accent color (0-255)');"));

      oappend(SET_F("addInfo('")); oappend(String(FPSTR(_name)).c_str()); 
      oappend(SET_F(":accent_g")); oappend(SET_F("',1,'Green component of accent color (0-255)');"));

      oappend(SET_F("addInfo('")); oappend(String(FPSTR(_name)).c_str()); 
      oappend(SET_F(":accent_b")); oappend(SET_F("',1,'Blue component of accent color (0-255)');"));

      oappend(SET_F("addInfo('")); oappend(String(FPSTR(_name)).c_str()); 
      oappend(SET_F(":accent_amount")); oappend(SET_F("',1,'Intensity of accent color (0-255)');"));

      oappend(SET_F("addInfo('")); oappend(String(FPSTR(_name)).c_str()); 
      oappend(SET_F(":smoothingFactor")); oappend(SET_F("',1,'Smoothing Factor .2-.8');"));

      oappend(SET_F("addInfo('")); oappend(String(FPSTR(_name)).c_str()); 
      oappend(SET_F(":fractionControl")); oappend(SET_F("',1,'Interpolation speed .2 - 1.8');"));
      

    }

    /*
     * getId() allows you to optionally give your V2 usermod an unique ID (please define it in const.h!).
     */
    uint16_t getId() {
      return USERMOD_ID_BLAQVOID_AR;
    }
};