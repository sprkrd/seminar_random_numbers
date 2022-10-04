#include <array>
#include <cstdint>
#include <random>


namespace ash::random {

inline uint64_t produce_random_seed() {
  std::random_device rdev;
  return ((uint64_t)rdev())<<32 | rdev();
}

class Ran {
  public:
    typedef uint64_t result_t;
    typedef std::array<result_t,3> state_t;

    static constexpr result_t min() { return 0; }
    static constexpr result_t max() { return ~result_t(0ULL); }

    Ran(result_t s = 0) {
      seed(s);
    }

    const state_t& state() const {
      return m_state;
    }

    void state(const state_t& state) {
      m_state = state;
    }

    void seed(result_t s) {
      m_state[1] = 4101842887655102017ULL;
      m_state[2] = 1;
      m_state[0] = s^m_state[1]; (*this)();
      m_state[1] = m_state[0]; (*this)();
      m_state[2] = m_state[1]; (*this)();
    }

    void random_seed() {
      seed(produce_random_seed());
    }

    result_t operator()() {
      m_state[0] = m_state[0]*2862933555777941757ULL +
                   7046029254386353087ULL;
      m_state[1] ^= m_state[1] >> 17;
      m_state[1] ^= m_state[1] << 31;
      m_state[1] ^= m_state[1] >> 8;
      m_state[2] = 4294957665ULL*(m_state[2]&0xffffffffULL) +
                   (m_state[2] >> 32);
      uint64_t x = m_state[0] ^ (m_state[0] << 21);
      x ^= x >> 35;
      x ^= x << 4;
      return (x + m_state[1]) ^ m_state[2];
    }

  private:
    state_t m_state;
};

class RanQ1 {
  public:
    typedef uint64_t result_t;

    static constexpr result_t min() { return 0; }
    static constexpr result_t max() { return ~result_t(0ULL); }

    RanQ1(result_t s = 0) {
      seed(s);
    }

    void seed(result_t s) {
      m_v = 4101842887655102017ULL;
      m_v ^= s;
      m_v = (*this)();
    }

    void random_seed() {
      seed(produce_random_seed());
    }

    result_t operator()() {
      m_v ^= m_v >> 21; m_v ^= m_v << 35; m_v ^= m_v >> 4;
      return m_v*2685821657736338717ULL;
    }

  private:
    result_t m_v;
};

class pcg32 {
  /* 64bit state, 32bit output, period of 2^64 (~1.8e19)*/
  public:
    typedef uint32_t result_t;
    typedef uint64_t state_t;

    static constexpr state_t mul = 6364136223846793005ULL;
    static constexpr state_t inc = 1442695040888963407ULL;

    static constexpr result_t min() { return 0; }
    static constexpr result_t max() { return ~result_t(0ULL); }

    pcg32(state_t s = 0) { seed(s); }

    void seed(state_t s) {
      m_state = s;
      (*this)();
    }

    void random_seed() {
      seed(produce_random_seed());
    }

    result_t operator()() {
      state_t old_state = m_state;
      m_state = m_state*mul + inc;
      result_t xorshifted = ((old_state>>18)^old_state) >> 27;
      result_t rot = old_state >> 59;
      return (xorshifted>>rot) | (xorshifted<<((-rot)&31));
    }
  private:
    state_t m_state;
};

class xoshiro256ss {
  /* 256bit state, 64bit output, period of 2^256-1 (~1.2e77)*/
  public:
    typedef uint64_t result_t;

    static result_t splitmix64(result_t& s) {
      result_t z = (s += 0x9e3779b97f4a7c15ULL);
    	z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    	z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    	return z ^ (z >> 31);
    }

    static result_t rotl(result_t x, unsigned k) {
      return (x << k) | (x >> (64 - k));
    }

    static constexpr result_t min() { return 0; }
    static constexpr result_t max() { return ~result_t(0ULL); }

    xoshiro256ss(result_t s = 0) {
      seed(s);
    }

    void seed(result_t s = 0) {
      for (result_t& s_i : m_s)
        s_i = splitmix64(s);
    }

    void random_seed() {
      seed(produce_random_seed());
    }

    result_t operator()() {
      result_t result = rotl(m_s[1] * 5, 7) * 9;
    	result_t t = m_s[1] << 17;
    	m_s[2] ^= m_s[0];
      m_s[3] ^= m_s[1];
      m_s[1] ^= m_s[2];
      m_s[0] ^= m_s[3];
    	m_s[2] ^= t;
    	m_s[3] = rotl(m_s[3], 45);
    	return result;
    }

  private:
    result_t m_s[4];
};

}
