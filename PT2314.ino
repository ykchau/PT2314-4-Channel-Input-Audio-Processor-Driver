#include "PT2314.h"
#include <Wire.h>

PT2314 AudioControl;
String str;
byte value = 0;
byte loudness[] = { PT2314_LOUDNESS_ON, PT2314_LOUDNESS_OFF };
byte gain[] = { PT2314_GAIN_0, PT2314_GAIN_3_75, PT2314_GAIN_7_5, PT2314_GAIN_11_25 };


void setup() {
    Wire.begin();
    Serial.begin(115200);

    AudioControl.switchChannel(PT2314_CHANNEL_1, PT2314_LOUDNESS_OFF, PT2314_GAIN_0);
    AudioControl.setVolume(15);
    AudioControl.setBass(7);
    AudioControl.setTreble(7);
    AudioControl.setAttenuation(PT2314_SPEAKER_LEFT,15);
    AudioControl.setAttenuation(PT2314_SPEAKER_RIGHT,15);

    // If there have no output at the initial stage, just run this command one more time
    AudioControl.switchChannel(PT2314_CHANNEL_1, PT2314_LOUDNESS_OFF, PT2314_GAIN_0);
}

void loop() {
    if ( Serial.available() ) {
        str = Serial.readStringUntil('\n');
        
        switch (str.charAt(0)) {
        case '1':
            AudioControl.switchChannel(PT2314_CHANNEL_1, AudioControl.settings.loudness, AudioControl.settings.gain);
            printChannel();
            break;
        case '2':
            AudioControl.switchChannel(PT2314_CHANNEL_2, AudioControl.settings.loudness, AudioControl.settings.gain);
            printChannel();
            break;
        case '3':
            AudioControl.switchChannel(PT2314_CHANNEL_3, AudioControl.settings.loudness, AudioControl.settings.gain);
            printChannel();
            break;
        case '4':
            AudioControl.switchChannel(PT2314_CHANNEL_4, AudioControl.settings.loudness, AudioControl.settings.gain);
            printChannel();
            break;
        case 'v':
            Serial.println("Please enter volume : small (0 - 100) big");
            readValue();
            AudioControl.setVolume(value);
            printVolume();
            break;
        case 'b':
            Serial.println("Please enter bass : small (0 - 100) big");
            readValue();
            AudioControl.setBass(value);
            printBass();
            break;
        case 't':
            Serial.println("Please enter Treble : small (0 - 100) big");
            readValue();
            AudioControl.setTreble(value);
            printTreble();
            break;
        case 'l':
            Serial.println("Please enter Att L : small (0 - 100) big");
            readValue();
            AudioControl.setAttenuation(PT2314_SPEAKER_LEFT, value);
            printAttL();
            break;
        case 'r':
            Serial.println("Please enter ATT R : small (0 - 100) big");
            readValue();
            AudioControl.setAttenuation(PT2314_SPEAKER_RIGHT, value);
            printAttR();
            break;
        case 'g':   // Gain
            Serial.println("Please enter gain (0 - 3, [0, 3.75, 7.5, 11.25]): ");
            readValue();
            AudioControl.switchChannel(AudioControl.settings.channel, AudioControl.settings.loudness, gain[value]);
            printChannel();
            break;
        case 'c':   // Channel
            Serial.println("Please enter channel (1 - 4) : ");
            readValue();
            AudioControl.switchChannel(value - 1, AudioControl.settings.loudness, AudioControl.settings.gain);
            printChannel();
            break;
        case 'o':   // Loudness
            Serial.println("Please enter loudness (0 - ON, 1 - OFF): ");
            readValue();
            AudioControl.switchChannel(AudioControl.settings.channel, loudness[value], AudioControl.settings.gain);
            printChannel();
            break;
        case 'e':
            SPL("Please SE", "(0 ~ 4, Normal, Classic, Pop, Rock, Jazz)");
            readValue();
            AudioControl.setSoundEffect(value);
            break;
        case '`':
            AudioControl.printSettings();
            break;
        case ',':
            SPL("Mute Channel Left"," ");
            AudioControl.setMute(PT2314_SPEAKER_LEFT, (AudioControl.settings.mute_L == PT2314_MUTE_ON)? PT2314_MUTE_OFF : PT2314_MUTE_ON);
            break;
        case '.':
            SPL("Mute Channel Right"," ");
            AudioControl.setMute(PT2314_SPEAKER_RIGHT, (AudioControl.settings.mute_R == PT2314_MUTE_ON)? PT2314_MUTE_OFF : PT2314_MUTE_ON);
            break;
        default:
            break;
        }
    }
}

void readValue() {
    while ( !Serial.available() ) {
        continue;
    }

    str = Serial.readStringUntil('\n');
    value = byte(str.toInt());
}

void printVolume() {
    SPL("[RAW] Volume : ", AudioControl.settings.volume);
}

void printTreble() {
    SPL("[RAW] Treble : ", AudioControl.settings.treble);
}

void printBass() {
    SPL("[RAW] Bass : ", AudioControl.settings.bass);
}

void printAttL() {
    SPL("[RAW] Att L : ", AudioControl.settings.attenuation_L);
}

void printAttR() {
    SPL("[RAW] Att R : ", AudioControl.settings.attenuation_R);
}

void printChannel() {
    SPL("[RAW] Channel : ", AudioControl.settings.channel);
}

void printLoudness() {
    SPL("[RAW] Loudness : ", AudioControl.settings.loudness);
}

void printGain() {
    SPL("[RAW] Gain : ", AudioControl.settings.gain);
}