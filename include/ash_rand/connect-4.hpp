#pragma once

#include <immintrin.h>
#include <algorithm>
#include <array>
#include <cstdint>
#include <functional>
#include <ostream>

#include "pcg32.hpp"


#define USE_AVX


typedef pcg32 Random;


// board
//   012345678
// 0 .........
// 1 .........
// 2 .........
// 3 .........
// 4 .........
// 5 .........
// 6 .........

constexpr int WIDTH = 9;
constexpr int HEIGHT = 7;
constexpr uint64_t COLUMN = 0x40201008040201ULL;
constexpr uint64_t ROW = 0x1FFULL;
constexpr int MAX_NUMBER_OF_ACTIONS = WIDTH + 1;

constexpr size_t HORIZONTAL_MASK = 0x7e3f1f8fc7e3f1f8ULL;
constexpr size_t DOWN_RIGHT_MASK = 0x7e3f1f8fc7e3f1f8ULL;
constexpr size_t DOWN_LEFT_MASK = 0xfc7e3f1f8fc7e3fULL;
constexpr size_t VERTICAL_MASK = (1ULL<<63) - 1;

constexpr size_t FIRST_ROW = 0x7fc0000000000000;

constexpr int STEAL = 63;
constexpr int STEAL_CG = -2;

#ifndef CODINGAME
const char* BOARD_SYMBOLS[] = {".", "\033[31m*\033[0m", "\033[33m*\033[0m"};
#else
const char* BOARD_SYMBOLS[] = {".", "0", "1"};
#endif

enum Status {ONGOING, WIN, TIE};

//std::array<int,WIDTH> COLUMNS = {0,1,2,3,4,5,6,7,8};
std::array<int,WIDTH> COLUMNS = {4,3,5,2,6,1,7,0,8};


typedef std::array<uint64_t,WIDTH*HEIGHT*2> ZobristArray;

ZobristArray ZOBRIST_HASHES;

void init_zobrist() {
  Random rng(42);
  for (auto& z_hash : ZOBRIST_HASHES) {
    z_hash = rng.generate_uint64();
  }
}

#ifdef PROFILE
    __attribute__((noinline))
#endif
bool move_connects_4(uint64_t bitboard) {
  // this function gets called *VERY* frequently!! let's try to optimize it as
  // much as possible
#ifdef USE_AVX
  // parallel code: it's VERY fast!!
  __m256i mm_bitboard = _mm256_set1_epi64x(bitboard);
  __m256i mm_masks = _mm256_set_epi64x(HORIZONTAL_MASK, DOWN_RIGHT_MASK, DOWN_LEFT_MASK, VERTICAL_MASK);
  __m256i mm_shifts = _mm256_set_epi64x(1, WIDTH+1, WIDTH-1, WIDTH);
  __m256i mm_shifts2 = _mm256_set_epi64x(2, 2*(WIDTH+1), 2*(WIDTH-1), 2*WIDTH);

  mm_bitboard = _mm256_and_si256(mm_bitboard, _mm256_sllv_epi64(mm_bitboard, mm_shifts2));
  mm_bitboard = _mm256_and_si256(mm_bitboard, _mm256_sllv_epi64(mm_bitboard, mm_shifts));
  //mm_bitboard = _mm256_and_si256(mm_bitboard, _mm256_sllv_epi64(mm_bitboard, mm_shifts));

  int z = _mm256_testz_si256(mm_bitboard, mm_masks);
  return !z;

#else
  // serial code

  uint64_t bb = bitboard;

  bb = bb&(bb<<1);
  bb = bb&(bb<<1);
  bb = bb&(bb<<1) & HORIZONTAL_MASK;

  if (bb)
    return true;

  bb = bitboard;
  bb = bb&(bb<<(WIDTH+1));
  bb = bb&(bb<<(WIDTH+1));
  bb = bb&(bb<<(WIDTH+1)) & DOWN_RIGHT_MASK;

  if (bb)
    return true;

  bb = bitboard;
  bb = bb&(bb<<(WIDTH-1));
  bb = bb&(bb<<(WIDTH-1));
  bb = bb&(bb<<(WIDTH-1)) & DOWN_LEFT_MASK;

  if (bb)
    return true;

  bb = bitboard;
  bb = bb&(bb<<(WIDTH));
  bb = bb&(bb<<(WIDTH));
  bb = bb&(bb<<(WIDTH)) & VERTICAL_MASK;

  return bb != 0;
#endif
}

struct State {
  std::array<uint64_t,2> bitboard;
  size_t z_hash;
  int8_t next_player;
  int8_t number_of_tokens;

  State() : bitboard{0}, z_hash(0), next_player(0), number_of_tokens(0) {
  }

  void step(int action) {
    if (action == STEAL) {
      std::swap(bitboard[0], bitboard[1]);
      recompute_hash();
    }
    else {
      bitboard[next_player] |= (1ULL<<action);
      z_hash ^= ZOBRIST_HASHES[2*action+next_player];
      ++number_of_tokens;
    }
    next_player ^= 1;
  }

  State get_successor(int action) const {
    State successor(*this);
    successor.step(action);
    return successor;
  }

  int free_position(int col) const {
    int row = HEIGHT;
    while (get_cell(--row,col) != -1 && row >= 0);
    if (row >= 0) {
      return row*WIDTH + col;
    }
    return -1;
  }
  
  bool operator==(const State& other) const {
    return bitboard == other.bitboard; // the rest of the members are derived
  }

  bool operator!=(const State& other) const {
    return !(*this == other);
  }

#ifdef PROFILE
    __attribute__((noinline))
#endif
  void recompute_hash() {
    z_hash = 0;
    for (int j = 0; j < 2; ++j) {
      auto bb = bitboard[j];
      while (bb) {
        int i = __builtin_ctzll(bb);
        bb &= ~(1ULL<<i);
        z_hash ^= ZOBRIST_HASHES[2*i + j];
      }
    }
  }

#ifdef PROFILE
    __attribute__((noinline))
#endif
  Status get_status() const {
    Status status = ONGOING;
    if (number_of_tokens == WIDTH*HEIGHT) {
      status = TIE;
    }
    else if (number_of_tokens >= 4  && move_connects_4(bitboard[next_player^1])) {
      status = WIN;
    }
    return status;
  }

  int get_cell(int index) const {
    int content = -1;
    uint64_t mask = 1ULL << index;
    if (bitboard[0]&mask) {
      content = 0;
    }
    else if (bitboard[1]&mask) {
      content = 1;
    }
    return content;
  }

  int get_cell(int i, int j) const {
    int index = i*WIDTH + j;
    return get_cell(index);
  }
};

namespace std {
  
  template<> struct hash<State> {
    size_t operator()(const State& state) const {
      return state.z_hash;
    }
  };
  
}

std::ostream& operator<<(std::ostream& out, const State& state) {
  for (int i = 0; i < HEIGHT; ++i) {
    for (int j = 0; j < WIDTH; ++j) {
      out << BOARD_SYMBOLS[1+state.get_cell(i,j)];
    }
    out << '\n';
  }
  out << "Z-Hash: " << state.z_hash << '\n';
  out << "Number of tokens: " << (int)state.number_of_tokens << '\n';
  Status status = state.get_status();
  if (status == WIN) {
    out << "Player " << BOARD_SYMBOLS[2-state.next_player] << " wins";
  }
  else if (status == TIE) {
    out << "Tie";
  }
  else {
    out << "Next to move: " << BOARD_SYMBOLS[1+state.next_player];
  }
  return out;
}

class AvailableMoves {
  public:

    class const_iterator {
      public:
        const_iterator(uint64_t moves = 0) : m_remaining_positions(moves) {
        }

        int operator*() {
          return __builtin_ctzll(m_remaining_positions);
        }

        bool operator!=(const const_iterator& other) const {
          return m_remaining_positions != other.m_remaining_positions;
        }

        const_iterator& operator++() {
          m_remaining_positions &= m_remaining_positions - 1;
          return *this;
        }

      private:

        uint64_t m_remaining_positions;
    };

    AvailableMoves(const State& state) {
      uint64_t filled_cells = state.bitboard[0]|state.bitboard[1];
      m_moves = ((filled_cells>>WIDTH)|FIRST_ROW) & ~filled_cells;

      if (state.number_of_tokens == 1 && state.next_player == 1) {
        m_moves |= 1ULL<<STEAL; // steal move for second player
      }
    }

    AvailableMoves(uint64_t moves = 0) : m_moves(moves) {
    }

    AvailableMoves operator&(const AvailableMoves& other) const {
      return m_moves&other.m_moves;
    }

    AvailableMoves move_below() const {
      return m_moves<<WIDTH;
    }

    AvailableMoves filter_out(const AvailableMoves& other) const {
      return m_moves & ~other.m_moves;
    }

    int operator[](int index) const {
      return __builtin_ctzll(_pdep_u64(1ULL<<index, m_moves));
    }

    int size() const {
      return __builtin_popcountll(m_moves);
    }

    bool contains(int move) const {
      return (m_moves>>move)&1;
    }

    int first() const {
      return __builtin_ctzll(m_moves);
    }

    operator bool() const {
      return m_moves;
    }

    const_iterator begin() const {
      return const_iterator(m_moves);
    }

    const_iterator end() const {
      return const_iterator();
    }


  private:

    uint64_t m_moves;

};

#ifdef USE_AVX

AvailableMoves potentially_winning_moves(uint64_t my_bb, uint64_t opp_bb) { 
  __m256i mm_bitboard = _mm256_set1_epi64x(my_bb);
  __m256i mm_holes = _mm256_set1_epi64x(~(my_bb|opp_bb));
  __m256i mm_masks = _mm256_set_epi64x(HORIZONTAL_MASK, DOWN_RIGHT_MASK, DOWN_LEFT_MASK, VERTICAL_MASK);
  __m256i mm_shifts = _mm256_set_epi64x(1, WIDTH+1, WIDTH-1, WIDTH);
  __m256i mm_shifts2 = _mm256_set_epi64x(2, 2*(WIDTH+1), 2*(WIDTH-1), 2*WIDTH);

  __m256i mm_aux0 = _mm256_sllv_epi64(mm_bitboard, mm_shifts);
  __m256i mm_aux1 = _mm256_xor_si256(mm_bitboard, mm_aux0);
  __m256i mm_aux2 = _mm256_and_si256(mm_bitboard, mm_aux0);
  __m256i mm_aux3 = _mm256_and_si256(mm_aux1, _mm256_sllv_epi64(mm_aux2, mm_shifts2));
  __m256i mm_aux4 = _mm256_and_si256(mm_aux2, _mm256_sllv_epi64(mm_aux1, mm_shifts2));
  __m256i mm_aux5 = _mm256_and_si256(_mm256_or_si256(mm_aux3, mm_aux4), mm_masks);

  __m256i acc = _mm256_and_si256(mm_aux5, mm_holes);
  mm_aux5 = _mm256_srlv_epi64(mm_aux5, mm_shifts);
  acc = _mm256_or_si256(acc, _mm256_and_si256(mm_aux5, mm_holes));
  mm_aux5 = _mm256_srlv_epi64(mm_aux5, mm_shifts);
  acc = _mm256_or_si256(acc, _mm256_and_si256(mm_aux5, mm_holes));
  mm_aux5 = _mm256_srlv_epi64(mm_aux5, mm_shifts);
  acc = _mm256_or_si256(acc, _mm256_and_si256(mm_aux5, mm_holes));

  uint64_t winning0 = _mm256_extract_epi64(acc, 0);
  uint64_t winning1 = _mm256_extract_epi64(acc, 1);
  uint64_t winning2 = _mm256_extract_epi64(acc, 2);
  uint64_t winning3 = _mm256_extract_epi64(acc, 3);

  return winning0|winning1|winning2|winning3;
}

#endif


class Environment {
  public:
#ifdef PROFILE
    __attribute__((noinline))
#endif
    AvailableMoves get_available_moves() const {
      return AvailableMoves(m_state);
    }
    
    void reset(const State& state) {
      m_state = state;
    }

    void reset() {
      m_state = State();
    }
    
    Status get_status() const {
      return m_state.get_status();
    }
    
    int next_player() const {
      return m_state.next_player;
    }
    
    int last_player() const {
      return m_state.next_player^1;
    }

#ifdef PROFILE
    __attribute__((noinline))
#endif
    void step(int action) {
      m_state.step(action);
    }

    const State& get_state() const {
      return m_state;
    }

  private:
    State m_state;
};

