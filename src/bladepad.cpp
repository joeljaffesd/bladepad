#include "al/app/al_App.hpp"
#include "al/math/al_Random.hpp"
using namespace al;

#include "../Gimmel/include/gimmel.hpp"
#include "bitCrusher.hpp"

#define SAMPLE_RATE 48000

template <typename T = float>
struct MyApp: public App {
    float color = 0.0;
    giml::SinOsc<T> mOsc{SAMPLE_RATE}; // synth sine

    std::unique_ptr<giml::Chorus<T>> mChorus; // chorus effect
    std::unique_ptr<giml::BitCrusher<T>> mBitCrusher; // bit crusher effect
    std::unique_ptr<giml::Delay<T>> mDelay; // delay line
    std::unique_ptr<giml::Reverb<T>> mReverb; // reverb effect
    giml::EffectsLine<T> mEffectsLine; // effects line

    void onInit() override { // Called on app start
        // Initialize the chorus effect
        mChorus = std::make_unique<giml::Chorus<T>>(SAMPLE_RATE);
        mChorus->setParams(0.2, 6.0, 0.5);
        mChorus->toggle(true);
        mEffectsLine.pushBack(mChorus.get());

        // Initialize the bit crusher effect
        mBitCrusher = std::make_unique<giml::BitCrusher<T>>(SAMPLE_RATE);
        // mBitCrusher->toggle(true);
        mEffectsLine.pushBack(mBitCrusher.get());

        // Initialize the delay effect
        mDelay = std::make_unique<giml::Delay<T>>(SAMPLE_RATE);
        mDelay->setParams();
        mDelay->toggle(true);
        mEffectsLine.pushBack(mDelay.get());

        // Initialize the reverb effect
        mReverb = std::make_unique<giml::Reverb<T>>(SAMPLE_RATE);
        mReverb->setParams(0.02, 0.5, 0.5, 0.5f, 1.f, 0.75f, giml::Reverb<T>::RoomType::SPHERE);
        mReverb->toggle(true);
        mEffectsLine.pushBack(mReverb.get());

        std::cout << "onInit()" << std::endl;
    }

    void onCreate() override { // Called when graphics context is available
        std::cout << "onCreate()" << std::endl;
    }

    void onAnimate(double dt) override { // Called once before drawing
        color += 0.01;
        if (color > 1.0) {
            mOsc.setFrequency(al::rnd::uniform(1000.0, 41.0));
            color -= 1.0;
        }
    } 

    void onDraw(Graphics &g) override { // Draw function
        g.clear(color);
    }

    void onSound(AudioIOData &io) override { // Audio callback
        while (io()) {
            io.out(0) = mEffectsLine.processSample(mOsc.processSample()) * 0.1;
        }
    }

    void onMessage(osc::Message &m) override { // OSC message callback
        m.print();
    }
};


int main()
{
    MyApp<> app;
    app.configureAudio(SAMPLE_RATE, 256, 2, 1);
    app.start();
    return 0;
}
