// ****************************************************************************
//
//                          PWM sound output
//
// ****************************************************************************
// PicoLibSDK - Alternative SDK library for Raspberry Pico and RP2040
// Copyright (c) 2023 Miroslav Nemecek, Panda38@seznam.cz, hardyplotter2@gmail.com
//	https://github.com/Panda381/PicoLibSDK
//	https://www.breatharian.eu/hw/picolibsdk/index_en.html
//	https://github.com/pajenicko/picopad
//	https://picopad.eu/en/
// License:
//	This source code is freely available for any purpose, including commercial.
//	It is possible to take and modify the code or parts of it, without restriction.

#include "global.h"    // globals
#include "includes.h"
#include "lib_pwmsnd.h"
#include <algorithm>
#include <vector>
#include <SFML/Audio.hpp>

// Config definitions (only one place)
#if USE_CONFIG
sConfig Config = { CONFIG_VOLUME_FULLSTEP };

void ConfigSetVolume(int level) {
    Config.volume = std::clamp(level, 0, CONFIG_VOLUME_MAX * CONFIG_VOLUME_STEP);
    GlobalVolumeUpdate();
}

void ConfigIncVolume() {
    ConfigSetVolume(Config.volume + CONFIG_VOLUME_STEP);
}

void ConfigDecVolume() {
    ConfigSetVolume(Config.volume - CONFIG_VOLUME_STEP);
}
#endif

// global sound OFF
volatile Bool GlobalSoundOff = False;

#if USE_PWMSND  // use PWM sound output; set 1.. = number of channels

// PWM sound channels
sPwmSnd PwmSound[USE_PWMSND];

// IMA ADPCM tables
#define ADPCM_MINVAL    -32768      // IMA ADPCM minimal value
#define ADPCM_MAXVAL    +32767      // IMA ADPCM maximal value
#define ADPCM_STEPS     89          // IMA ADPCM number of steps

const s8 ADPCM_TabInx[16] = {
    -1,-1,-1,-1, 2,4,6,8, -1,-1,-1,-1, 2,4,6,8
};

const s16 ADPCM_StepSize[ADPCM_STEPS] = {
    7,8,9,10,11,12,13,14,16,17,19,21,23,25,28,31,
    34,37,41,45,50,55,60,66,73,80,88,97,107,118,130,143,
    157,173,190,209,230,253,279,307,337,371,408,449,494,544,598,658,
    724,796,876,963,1060,1166,1282,1411,1552,1707,1878,2066,2272,2499,2749,3024,
    3327,3660,4026,4428,4871,5358,5894,6484,7132,7845,8630,9493,10442,11487,12635,13899,
    15289,16818,18500,20350,22385,24623,27086,29794,32767
};

// HW macros override for emulator
#undef PWM_Top
#undef PWM_Comp
#undef PWM_Enable
#define PWM_Top(slice, top)      /* no-op in emulator */
#define PWM_Comp(slice, chan, v) /* no-op in emulator */
#define PWM_Enable(slice)

// SFML SoundStream pro výstup mixovaných kanálů
class PwmSoundStream : public sf::SoundStream {
public:
    PwmSoundStream() {
        // Mono výstup, SFML 3.0 vyžaduje channelMap
        initialize(1, SOUNDRATE, { sf::SoundChannel::Mono });
    }

protected:
    bool onGetData(sf::SoundStream::Chunk& data) override {
        const int chunkSize = 512;
        buffer.resize(chunkSize);

        for (int i = 0; i < chunkSize; ++i) {
            int mix = 128 * SNDINT;  // výchozí vzorek

            // mix všech kanálů
            for (int ch = 0; ch < USE_PWMSND; ++ch) {
                auto& s = PwmSound[ch];
                if (s.cnt > 0) {
                    if (s.form == SNDFORM_PCM) {
                        mix += (int(s.snd[0]) - 128) * s.vol;
                        s.acc += s.inc;
                        int adv = s.acc >> SNDFRAC;
                        s.snd += adv;
                        s.cnt -= adv;
                        s.acc &= (SNDINT - 1);
                    } else {
                        // TODO: ADPCM dekódování
                    }
                    if (s.cnt <= 0) {
                        s.cnt   = s.nextcnt;
                        s.snd   = s.next;
                        s.acc   = 0;
                    }
                }
            }

            // ořez a normalizace na signed 16-bit
            int out8 = std::clamp(mix >> SNDFRAC, 0, 255);
            buffer[i] = static_cast<std::int16_t>((out8 - 128) * 256);
        }

        data.samples     = buffer.data();
        data.sampleCount = buffer.size();
        return true;
    }

    void onSeek(sf::Time /*offset*/) override {
        // Emulace nemá seek; prázdné
    }

private:
    std::vector<std::int16_t> buffer;
};  // zde končí třída PwmSoundStream


// single static instance
static PwmSoundStream soundStream;

// initialize PWM sound output
void PWMSndInit() {
    if (GlobalSoundOff) return;
    for (int i = 0; i < USE_PWMSND; ++i) {
        PwmSound[i].cnt = 0;
    }
    PWM_Top(PWMSND_SLICE, PWMSND_TOP);
    PWM_Comp(PWMSND_SLICE, PWMSND_CHAN, 128);
    soundStream.play();
}

// terminate PWM sound output
void PWMSndTerm() {
    soundStream.stop();
}

// stop playing sound
void StopSoundChan(u8 chan) {
    //dmb();
    PwmSound[chan].cnt = 0;
    //dmb();
}

void StopSound() {
    StopSoundChan(0);
}

void StopAllSound() {
    for (int i = 0; i < USE_PWMSND; ++i) StopSoundChan(i);
}

// play sound on channel
void PlaySoundChan(u8 chan, const u8* snd, int len, Bool rep,
                   float speed, float volume, u8 form, int ext) {
    if (GlobalSoundOff) return;
    // prepare volume
#if USE_CONFIG
    int v = (int)(SNDINT * volume * Config.volume / CONFIG_VOLUME_FULLSTEP + 0.5f);
#else
    int v = (int)(SNDINT * volume + 0.5f);
#endif
    // prepare speed
    if (form == SNDFORM_ADPCM) speed = 1.0f;
    int sinc = (int)(SNDINT * speed + 0.5f);
    // stop previous
    StopSoundChan(chan);
    auto* s = &PwmSound[chan];
    s->nextcnt = 0;
    if (rep) {
        s->next = snd;
        s->nextcnt = len;
    }
    s->inc = sinc;
    s->acc = 0;
    s->vol0 = volume;
    s->vol = v;
    s->sampblock = (s16)ext;
    s->sampcnt = 0;
    s->form = form;
    s->stepinx = 0;
    s->prevval = 0;
    s->odd = False;
    s->snd = snd;
    //dmb();
    s->cnt = len;
    //dmb();
    PWM_Enable(PWMSND_SLICE);
}

void PlaySound(const u8* snd, int len) {
    PlaySoundChan(0, snd, len, False, 1, 1, SNDFORM_PCM, 0);
}

void PlaySoundRep(const u8* snd, int len) {
    PlaySoundChan(0, snd, len, True, 1, 1, SNDFORM_PCM, 0);
}

// ADPCM
void PlayADPCMChan(u8 chan, const u8* snd, int len, int sampblock) {
    PlaySoundChan(chan, snd, len, False, 1, 1, SNDFORM_ADPCM, sampblock);
}

void PlayADPCMRepChan(u8 chan, const u8* snd, int len, int sampblock) {
    PlaySoundChan(chan, snd, len, True, 1, 1, SNDFORM_ADPCM, sampblock);
}

// speed
void SpeedSoundChan(u8 chan, float speed) {
    PwmSound[chan].inc = (int)(SNDINT * speed + 0.5f);
}

void SpeedSound(float speed) {
    SpeedSoundChan(0, speed);
}

// volume
void VolumeSoundChan(u8 chan, float volume) {
    auto* s = &PwmSound[chan];
    s->vol0 = volume;
#if USE_CONFIG
    s->vol = (int)(SNDINT * volume * Config.volume / CONFIG_VOLUME_FULLSTEP + 0.5f);
#else
    s->vol = (int)(SNDINT * volume + 0.5f);
#endif
}

void VolumeSound(float volume) {
    VolumeSoundChan(0, volume);
}

// playing check
Bool PlayingSoundChan(u8 chan) {
    if (GlobalSoundOff) return False;
    return PwmSound[chan].cnt > 0;
}

Bool PlayingSound() {
    if (GlobalSoundOff) return False;
    return PlayingSoundChan(0);
}

// next repeat
void SetNextSoundChan(u8 chan, const u8* snd, int len) {
    auto* s = &PwmSound[chan];
    if (PlayingSoundChan(chan) && s->next == snd && s->nextcnt == len) return;
    s->nextcnt = 0;
    //dmb();
    if (s->cnt == 0) {
        s->sampcnt = 0;
        s->stepinx = 0;
        s->prevval = 0;
        s->odd = False;
        s->snd = snd;
        //dmb();
        s->cnt = len;
        //dmb();
    }
    s->next = snd;
    //dmb();
    s->nextcnt = len;
}

void SetNextSound(const u8* snd, int len) {
    SetNextSoundChan(0, snd, len);
}

// global sound toggles
void GlobalSoundSetOff() {
    GlobalSoundOff = True;
    PWMSndTerm();
}

void GlobalSoundSetOn() {
    PWMSndTerm();
    GlobalSoundOff = False;
    PWMSndInit();
}

// update global volume on channels
void GlobalVolumeUpdate() {
#if USE_CONFIG
    for (int i = 0; i < USE_PWMSND; ++i) {
        auto* s = &PwmSound[i];
        s->vol = (int)(SNDINT * s->vol0 * Config.volume / CONFIG_VOLUME_FULLSTEP + 0.5f);
    }
#endif
}

#endif  // USE_PWMSND
