#include "PT2314.h"

void PT2314::I2CWrite(byte data) {
    Wire.beginTransmission(PT2314_I2C_ADDRESS);
    Wire.write(data);

    byte ret = Wire.endTransmission();
    
    if ( ret != 0 ) {
        SPL("Ret : ", ret );
        ret = Wire.endTransmission();
    }
    
    SPH("Data : ", data);
}

void PT2314::setMute(byte speaker, byte mute) {
    byte att_L, att_R;
    switch (speaker) {
        case PT2314_SPEAKER_LEFT:
            settings.mute_L = mute;
            att_L = ( mute == PT2314_MUTE_ON )? PT2314_MUTE : settings.attenuation_L;
            I2CWrite(att_L | PT2314_MASK_ATT_L);
            break;
        case PT2314_SPEAKER_RIGHT:
            settings.mute_R = mute;
            att_R = ( mute == PT2314_MUTE_ON )? PT2314_MUTE : settings.attenuation_R;
            I2CWrite( att_R | PT2314_MASK_ATT_R);
            break;
        case PT2314_SPEAKER_ALL:
            settings.mute_L = mute;
            settings.mute_R = mute;
            att_L = ( mute == PT2314_MUTE_ON )? PT2314_MUTE : settings.attenuation_L;
            att_R = ( mute == PT2314_MUTE_ON )? PT2314_MUTE : settings.attenuation_R;
            I2CWrite( att_L | PT2314_MASK_ATT_L);
            I2CWrite( att_R | PT2314_MASK_ATT_R);
            break;
        default:
            break;
    }
}

/*
    64 level, 0 ~ 63, 0 - Loudest
    range 0 ~ -78.75 dB
    1.25dB per level
*/
void PT2314::setVolume(byte volume) {
    settings.volume = scaleToRaw(PT2314_TYPE_VOLUME, volume);

    I2CWrite(settings.volume | PT2314_MASK_VOLUME);
}

/*
    32 level, 0 ~ 31, all '1' (31) is mute
    range 0 ~ -38.75 dB
    1.25dB per level
*/
void PT2314::setAttenuation(byte speaker, byte attenuation) {
    byte att = scaleToRaw(PT2314_TYPE_ATT, attenuation);

    if ( speaker == PT2314_SPEAKER_LEFT ) {
        settings.attenuation_L = att;
        I2CWrite(att | PT2314_MASK_ATT_L);
    } else {
        settings.attenuation_R = att;
        I2CWrite(att | PT2314_MASK_ATT_R);
    }
}

/*
    16 level, 0 ~ 15
    range -14 ~ 14
    
    Level to Range mapping:
    0 ~ 7  = -14 ~ 0 dB
    8 ~ 15 = 0 ~ 14 dB
    2 dB per level
*/
void PT2314::setBass(byte bass) {
    settings.bass = scaleToRaw(PT2314_TYPE_BASS, bass);

    switch (settings.bass) {
        case 0 ... 7:
            I2CWrite(settings.bass | PT2314_MASK_BASS);
        break;
        case 8 ... 15:
            I2CWrite( (settings.bass ^ 0x07) | PT2314_MASK_BASS);
        break;
    }
}

/*
    16 level, 0 ~ 15
    range -14 ~ 14
    
    Level to Range mapping:
    0 ~ 7  = -14 ~ 0 dB
    8 ~ 15 = 0 ~ 14 dB
    2 dB per level
*/
void PT2314::setTreble(byte treble) {
    settings.treble = scaleToRaw(PT2314_TYPE_TREBLE, treble);

    switch (settings.treble) {
        case 0 ... 7:
            I2CWrite(settings.treble | PT2314_MASK_TREBLE);
        break;
        case 8 ... 15:
            I2CWrite( (settings.treble ^ 0x07) | PT2314_MASK_TREBLE);
        break;
    }
}


/*
    Channel : PT2314_CHANNEL_[1..4] 
    Loudness : PT2314_LOUDNESS_ON | PT2314_LOUDNESS_OFF
    Gain : PT2314_GAIN_11_25 | PT2314_GAIN_7_5 
         | PT2314_GAIN_3_75 | PT2314_GAIN_0
*/
void PT2314::switchChannel(byte channel, byte loudness, byte gain) {
    settings.channel = channel;
    settings.loudness = loudness;
    settings.gain = gain;
    SPH("Channel : ",channel);
    SPH("Loudness : ", loudness);
    SPH("Gain : ", gain);
    I2CWrite( channel | loudness | gain | PT2314_MASK_SWITCH);
}

/*
                    #   Bass            Treble
PT2314_SE_NORMAL    0   0x07(0 dB)     0x07(0 dB)
PT2314_SE_CLASSIC   1   0x0E(2 dB)     0x0E(2 dB)
PT2314_SE_POP       2   0x0E(2 dB)     0x0D(4 dB)
PT2314_SE_ROCK      3   0x0C(6 dB)     0x07(0 dB)
PT2314_SE_JAZZ      4   0x0A(10 dB)    0x0E(2 dB)
*/
void PT2314::setSoundEffect(byte soundEffect) {
    switch (soundEffect) {
        case PT2314_SE_NORMAL:
        setBass(0x07);
        setTreble(0x07);
        SPL("SE : ", "Normal");
        break;
        case PT2314_SE_CLASSIC:
        setBass(0x0E);
        setTreble(0x0E);
        SPL("SE : ", "Classic");
        break;
        case PT2314_SE_POP:
        setBass(0x0E);
        setTreble(0x0D);
        SPL("SE : ", "Pop");
        break;
        case PT2314_SE_ROCK:
        setBass(0x0C);
        setTreble(0x07);
        SPL("SE : ", "Rock");
        break;
        case PT2314_SE_JAZZ:
        setBass(0x0A);
        setTreble(0x0E);
        SPL("SE : ", "Jazz");
        break;
    }
}

/*
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
*/
void PT2314::printSettings() {
    SPT("Channel : ", settings.channel);
    SPL(" | Loudness(0 - ON, 4 - OFF) : ", settings.loudness);
    SPL("Gain(0 - +11.25dB, 8 - +7.5dB, 16 - +3.75dB, 24 - 0dB) : ", settings.gain);
    SPT("Volume : ", settings.volume);
    SPT(" | Bass : ", settings.bass);
    SPL(" | Treble : ", settings.treble);
    SPT("Speaker Left  - ", " ");
    SPT("Att : ", settings.attenuation_L);
    SPL(" | mute_L(0 - Sound ON, 1 - Mute) : ",settings.mute_L);
    SPT("Speaker Right - ", " ");
    SPT("Att : ", settings.attenuation_R);
    SPL(" | mute_R(0 - Sound ON, 1 - Mute) : ",settings.mute_R);
}

/*
    Because the original value is difference and in reverse order
    It will confuse when using, so, we scale the value and make it more readable
                                Fact Value          RAW             After Scale
    Volume(small to big)        0 ~ -78.75dB        63 ~ 0          0 ~ 100
    Bass(small to big)          -14 ~ 14dB          0 ~ 7, 15 ~ 8   0 ~ 100
    Treble(small to big)        -14 ~ 14dB          0 ~ 7, 15 ~ 8   0 ~ 100
    Attenuation(small to big)   0 ~ -37.5dB,Mute    0 ~ 30,31       0 ~ 100 and Mute

    type : PT2314_TYPE_VOLUME | PT2314_TYPE_BASS | PT2314_TYPE_TREBLE | PT2314_TYPE_ATT
    inputLevel : 0 ~ setting.scale (default 15)
*/
byte PT2314::scaleToRaw(byte type, byte inputLevel) {
    SPL("Input : ", inputLevel);
    int raw = 0;
    switch (type) {
        case PT2314_TYPE_VOLUME:
            raw = ((settings.scale - float(inputLevel)) / settings.scale) * 63;
            raw = constrain(raw, 0, 63);
            break;
        case PT2314_TYPE_BASS:
            raw = (float(inputLevel) / settings.scale) * 15;
            raw = constrain(raw, 0, 15);
            break;
        case PT2314_TYPE_TREBLE:
            raw = (float(inputLevel) / settings.scale) * 15;
            raw = constrain(raw, 0, 15);
            break;
        case PT2314_TYPE_ATT:
            if ( inputLevel == 0 ) {
                raw = PT2314_MUTE;
            } else {
                raw = ((settings.scale - float(inputLevel)) / settings.scale) * 30;
                raw = constrain(raw, 0, 30);
            }
    }
    return raw;
}

byte PT2314::getScaledValue(byte type) {
    int level = 0;
    switch (type) {
        case PT2314_TYPE_VOLUME:
            level = ((63 - float(settings.volume)) / 63) * settings.scale;
            break;
        case PT2314_TYPE_BASS:
            level = (float(settings.bass) / 15) * settings.scale;
            break;
        case PT2314_TYPE_TREBLE:
            level = (float(settings.treble) / 15) * settings.scale;
            break;
        case PT2314_TYPE_ATT_LEFT:
            level = ((31 - float(settings.attenuation_L)) / 31) * settings.scale;
        case PT2314_TYPE_ATT_RIGHT:
            level = ((31 - float(settings.attenuation_R)) / 31) * settings.scale;
    }
    return level;
}