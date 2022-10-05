#include <array>
#include <cstdint>
#include <random>


namespace ash_rand {

inline uint64_t produce_random_seed() {
    std::random_device rdev;
    return ((uint64_t)rdev())<<32 | rdev();
}

using randu = std::linear_congruential_engine<uint64_t,65539,0,2147483648>;

using seq_nonrng = std::linear_congruential_engine<uint32_t,1,1,65536>;

class ran {
    // From numerical recipes. 192 bit state, 64 bit generator, 3.138 * 10^57 period

    public:
        typedef uint64_t result_type;
        typedef std::array<result_type,3> state_type;

        static constexpr result_type min() { return 0; }
        static constexpr result_type max() { return ~result_type(0ULL); }

        ran(result_type s = 0) {
            seed(s);
        }

        const state_type& state() const {
            return m_state;
        }

        void state(const state_type& new_state) {
            m_state = new_state;
        }

        void seed(result_type s) {
            m_state[1] = 4101842887655102017ULL;
            m_state[2] = 1;
            m_state[0] = s^m_state[1];
            (*this)();
            m_state[1] = m_state[0];
            (*this)();
            m_state[2] = m_state[1];
            (*this)();
        }

        result_type operator()() {
            m_state[0] = m_state[0]*2862933555777941757LL + 7046029254386353087LL;
            m_state[1] ^= m_state[1] >> 17;
            m_state[1] ^= m_state[1] << 31;
            m_state[1] ^= m_state[1] >> 8;
            m_state[2] = 4294957665U*(m_state[2]&0xffffffff) + (m_state[2] >> 32);
            uint64_t x = m_state[0] ^ (m_state[0] << 21);
            x ^= x >> 35;
            x ^= x << 4;
            return (x + m_state[1]) ^ m_state[2];
        }

    private:
        state_type m_state;
};

class ranq1 {
    // From numerical recipes. 64 bit state, 64 bit generator, 2**64 period
    
    public:
        typedef uint64_t result_type;
        typedef uint64_t state_type;

        static constexpr result_type min() { return 0; }
        static constexpr result_type max() { return ~result_type(0ULL); }

        ranq1(result_type s = 0) {
            seed(s);
        }

        state_type state() const {
            return m_state;
        }

        void state(state_type new_state) {
            m_state = new_state;
        }

        void seed(result_type s) {
            m_state = 4101842887655102017ULL;
            m_state ^= s;
            m_state = (*this)();
        }

        result_type operator()() {
            m_state ^= m_state >> 21;
            m_state ^= m_state << 35;
            m_state ^= m_state >> 4;
            return m_state * 2685821657736338717LL;
        }

    private:
        state_type m_state;
};

class pcg32 {
    /* By Melissa O'Neil. 64bit state, 32bit output, period of 2^64 (~1.8e19)*/

    public:
        typedef uint32_t result_type;
        typedef uint64_t state_type;

        static constexpr state_type mul = 6364136223846793005ULL;
        static constexpr state_type inc = 1442695040888963407ULL;

        static constexpr result_type min() { return 0; }
        static constexpr result_type max() { return ~result_type(0ULL); }

        pcg32(state_type s = 0) { seed(s); }

        state_type state() const {
            return m_state;
        }

        void state(state_type new_state) {
            m_state = new_state;
        }

        void seed(state_type s) {
            m_state = s;
            (*this)();
        }

        result_type operator()() {
            state_type old_state = m_state;
            m_state = m_state*mul + inc;
            result_type xorshifted = ((old_state>>18)^old_state) >> 27;
            result_type rot = old_state >> 59;
            return (xorshifted>>rot) | (xorshifted<<((-rot)&31));
        }

    private:
        state_type m_state;
};

class splitmix64 {
    // Java default RNG: 64 bit state and output, 2^64 period
    public:
        typedef uint64_t result_type;
        typedef uint64_t state_type;

        static constexpr result_type min() { return 0; }
        static constexpr result_type max() { return ~result_type(0ULL); }

        splitmix64(result_type s = 0) : m_state(s) { }

        state_type state() const {
            return m_state;
        }

        void state(state_type state) {
            m_state = state;
        }

        void seed(state_type s) {
            m_state = s;
        }


        result_type operator()() {
            result_type z = (m_state += 0x9e3779b97f4a7c15ULL);
        	z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
        	z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
        	return z ^ (z >> 31);
        }
    private:

        state_type m_state;
};

class xoshiro256ss {
    // 256bit state, 64bit output, period of 2^256-1 (~1.2e77)
    public:
        typedef uint64_t result_type;
        typedef std::array<result_type,4> state_type;

        static constexpr result_type min() { return 0; }
        static constexpr result_type max() { return ~result_type(0ULL); }

        xoshiro256ss(result_type s = 0) { seed(s); }

        const state_type& state() const {
            return m_state;
        }

        void state(state_type new_state) {
            m_state = new_state;
        }

        void seed(result_type s = 0) {
            splitmix64 srandom(s);
            for (result_type& s_i : m_state)
                s_i = srandom();
        }

        result_type operator()() {
            result_type result = rotl(m_state[1] * 5, 7) * 9;
        	result_type t = m_state[1] << 17;
        	m_state[2] ^= m_state[0];
            m_state[3] ^= m_state[1];
            m_state[1] ^= m_state[2];
            m_state[0] ^= m_state[3];
        	m_state[2] ^= t;
        	m_state[3] = rotl(m_state[3], 45);
        	return result;
        }

    private:

        static result_type rotl(result_type x, unsigned k) {
            return (x << k) | (x >> (64 - k));
        }

        state_type m_state;
};

}
