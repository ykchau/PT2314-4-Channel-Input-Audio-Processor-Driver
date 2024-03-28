#ifndef PT2314_H_
#define PT2314_H_

#include <Arduino.h>
#include <Wire.h>

#define SPT(str,value) Serial.print(String(str) + value)
#define SPL(str,value) Serial.println(String(str) + value)
#define SPH(str,value) { Serial.print(String("[HEX]") + str); Serial.println(value,HEX); }

#define PT2314_I2C_ADDRESS          0x44

// Channel
#define PT2314_CHANNEL_1   0x00
#define PT2314_CHANNEL_2   0x01
#define PT2314_CHANNEL_3   0x02
#define PT2314_CHANNEL_4   0x03

// Speaker
#define PT2314_SPEAKER_LEFT     0
#define PT2314_SPEAKER_RIGHT    1
#define PT2314_SPEAKER_ALL      2


#define PT2314_MUTE         0x1F

// Mask
#define PT2314_MASK_VOLUME  0x00
#define PT2314_MASK_ATT_L   0xC0
#define PT2314_MASK_ATT_R   0xE0
#define PT2314_MASK_SWITCH  0x40
#define PT2314_MASK_BASS    0x60
#define PT2314_MASK_TREBLE  0x70

// Loudness
#define PT2314_LOUDNESS_ON  0x00
#define PT2314_LOUDNESS_OFF 0x04

// Mute
#define PT2314_MUTE_OFF     0
#define PT2314_MUTE_ON      1


// Gain
#define PT2314_GAIN_11_25   0x00
#define PT2314_GAIN_7_5     0x08
#define PT2314_GAIN_3_75    0x10
#define PT2314_GAIN_0       0x18

// Sound Effects
#define PT2314_SE_NORMAL    0
#define PT2314_SE_CLASSIC   1
#define PT2314_SE_POP       2
#define PT2314_SE_ROCK      3
#define PT2314_SE_JAZZ      4

// Scale Type
#define PT2314_TYPE_VOLUME      0
#define PT2314_TYPE_BASS        1
#define PT2314_TYPE_TREBLE      2
#define PT2314_TYPE_ATT         3
#define PT2314_TYPE_ATT_LEFT    4
#define PT2314_TYPE_ATT_RIGHT   5


typedef struct Settings {
    // Raw Data
    byte volume = 0;            // 64 level, range 0 ~ -78.75 dB
    byte attenuation_L = 0;     // 32 level, 0 ~ -38.75 & all '1' is mute
    byte attenuation_R = 0;     // 32 level, 0 ~ -38.75 & all '1' is mute
    byte bass = 0;              // 16 level, -14 ~ 14
    byte treble = 0;            // 16 level, -14 ~ 14
    // audio switch
    byte channel = 0;           // 4 channels
    byte loudness = 0;          // on/off
    byte gain = 0;              // 4 gains type
    // mute
    byte mute_L = 0;
    byte mute_R = 0;
    // scale
    byte scale = 15;            // Max number for scaling
};


class PT2314 {
private:
    void I2CWrite(byte data);
    
public:
    Settings settings;
    void setMute(byte speaker, byte mute);
    void setVolume(byte volume);
    void setAttenuation(byte speaker, byte attenuation);
    void setBass(byte bass);
    void setTreble(byte treble);
    void switchChannel(byte channel, byte loudness, byte gain);

    void setSoundEffect(byte soundEffect);
    void printSettings();
    byte scaleToRaw(byte type, byte inputLevel);
    byte getScaledValue(byte type);
};


#endif // PT2314_H