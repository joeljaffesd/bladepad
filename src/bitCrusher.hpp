#include "../Gimmel/include/gimmel.hpp"

namespace giml {

/**
 * @brief only does 0.5 for now, the default in Charlie's implementation in 
 * [Gibberish](https://github.com/charlieroberts/gibber.audio.lib)
 */
template <typename T = float>
class BitCrusher : public Effect<T> {
private:
    size_t mBitDepth = sizeof(T) * 8; // Bit depth of the effect
    T mSampleRate; // Sample rate of the effect
    T lastOut;
    bool everyOther = false;

public:
    BitCrusher() = delete; // Delete default constructor
    BitCrusher(T sampleRate) : mSampleRate(sampleRate) {}

    // Copy constructor
    BitCrusher(const BitCrusher& other) 
      : mBitDepth(other.mBitDepth), 
        mSampleRate(other.mSampleRate), 
        lastOut(other.lastOut), 
        everyOther(other.everyOther) {}

    // Copy assignment operator
    BitCrusher& operator=(const BitCrusher& other) {
      if (this != &other) {
        mBitDepth = other.mBitDepth;
        mSampleRate = other.mSampleRate;
        lastOut = other.lastOut;
        everyOther = other.everyOther;
      }
      return *this;
    }

    // Destructor
    ~BitCrusher() = default;

    inline T processSample(const T& input) override {

      if (!this->enabled) {
        return input;
      }

      if (!everyOther) {
        everyOther = !everyOther;
        return lastOut;
      } 

      int reducedBits = static_cast<int>(mBitDepth * 0.5);
      reducedBits = clip<size_t>(reducedBits, 1, mBitDepth);
  
      T step = static_cast<T>(1.0) / static_cast<T>(1 << reducedBits);

      lastOut = std::round(input / step) * step;
      everyOther = !everyOther;
      return lastOut;
    }
};
} // namespace giml