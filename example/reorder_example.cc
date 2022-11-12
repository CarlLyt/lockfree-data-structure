#include <cstdlib>
#include <iostream>
#include <semaphore.h>
#include <thread>

// Set either of these to 1 to prevent CPU reordering
#define USE_CPU_FENCE 0
#define USE_SINGLE_HW_THREAD 0 // Supported on Linux, but not Cygwin or PS3

#if USE_SINGLE_HW_THREAD
#include <sched.h>
#endif

//-------------------------------------
//  MersenneTwister
//  A thread-safe random number generator with good randomness
//  in a small number of instructions. We'll use it to introduce
//  random timing delays.
//-------------------------------------
#define MT_IA 397
#define MT_LEN 624

class MersenneTwister {
  unsigned int m_buffer[MT_LEN];
  int m_index;

public:
  MersenneTwister(unsigned int seed);
  // Declare noinline so that the function call acts as a compiler barrier:
  unsigned int integer() __attribute__((noinline));
};

MersenneTwister::MersenneTwister(unsigned int seed) {
  // Initialize by filling with the seed, then iterating
  // the algorithm a bunch of times to shuffle things up.
  for (int i = 0; i < MT_LEN; i++)
    m_buffer[i] = seed;
  m_index = 0;
  for (int i = 0; i < MT_LEN * 100; i++)
    integer();
}

unsigned int MersenneTwister::integer() {
  // Indices
  int i = m_index;
  int i2 = m_index + 1;
  if (i2 >= MT_LEN)
    i2 = 0; // wrap-around
  int j = m_index + MT_IA;
  if (j >= MT_LEN)
    j -= MT_LEN; // wrap-around

  // Twist
  unsigned int s = (m_buffer[i] & 0x80000000) | (m_buffer[i2] & 0x7fffffff);
  unsigned int r = m_buffer[j] ^ (s >> 1) ^ ((s & 1) * 0x9908B0DF);
  m_buffer[m_index] = r;
  m_index = i2;

  // Swizzle
  r ^= (r >> 11);
  r ^= (r << 7) & 0x9d2c5680UL;
  r ^= (r << 15) & 0xefc60000UL;
  r ^= (r >> 18);
  return r;
}

sem_t sem1, sem2;
sem_t end1, end2;
int x, y;
int r1, r2;
int threshold = 0;

void thread_worker1() {
  MersenneTwister random(1);
  for (;;) {
    sem_wait(&sem1);
    while (random.integer() % 8 != 0)
      ;
    y = 1;
    asm volatile("" ::: "memory");
    r1 = x;
    sem_post(&end1);
    if (threshold >= 20)
      return;
  }
}

void thread_worker2() {
  MersenneTwister random(1);
  for (;;) {

    sem_wait(&sem2);
    while (random.integer() % 8 != 0)
      ;
    x = 1;
    asm volatile("mfence" ::: "memory");
    r2 = y;
    sem_post(&end2);
    if (threshold >= 20)
      return;
  }
}

int main() {
  sem_init(&sem1, 0, 0);
  sem_init(&sem2, 0, 0);
  sem_init(&end1, 0, 0);
  sem_init(&end2, 0, 0);
  int iterations = 0;
  std::thread work1(thread_worker1);
  std::thread work2(thread_worker2);

  for (;;) {
    x = 0, y = 0;
    sem_post(&sem1);
    sem_post(&sem2);
    sem_wait(&end1);
    sem_wait(&end2);

    if (x == 0 && y == 0) {
      threshold++;
      std::cout << iterations << " found reordered happend " << threshold
                << " times" << std::endl;
      if (threshold >= 20)
        break;
    }
    iterations++;
  }
  work1.join();
  work2.join();
  return 0;
}