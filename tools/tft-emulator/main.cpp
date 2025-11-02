// Includes libraries necessary for the code.
#include "emulator/src/lib_draw.cpp"
#include "emulator/src/lib_pwmsnd.cpp"
#include "snd/father.h"
#include "snd/notTrue.h"
#include "img/darthVader.h"

void picopad() {
        DrawClear();
        DrawImgRle(darthVaderRLE, darthVaderRLE_Pal, 0, 0, 320, 240);
        DispUpdate();
}

int main() {
    // místo create(): 
    if (!FrameTex.resize({DispWidth, DispHeight})) {
    std::cerr << "Failed to resize texture\n";
    return EXIT_FAILURE;
    }
    FrameSprite.setTexture(FrameTex,true);
    PWMSndInit();
    PlaySoundChan(0, fatherSnd, 132300, False, 1, 1, SNDFORM_PCM, 0);
    PlaySoundChan(1, notTrueSnd, 154350, False, 1, 1, SNDFORM_PCM, 0);
    while (window.isOpen()) {
        // nové pollEvent API
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                PWMSndTerm();
                window.close();
            }
            else if (auto* key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->scancode == sf::Keyboard::Scancode::Y) {
                    PWMSndTerm();
                    window.close();
                    return 0;
                }
            }
        }

        picopad();

        window.clear();
        window.draw(FrameSprite);
        window.display();
    }

    return 0;
}
