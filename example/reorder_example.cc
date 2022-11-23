#include <assert.h>
#include <conf.h>
#include <cstdlib>
#include <iostream>
#include <semaphore.h>
#include <thread>
sem_t sem1, sem2;
sem_t end1, end2;
int x, y;
int r1, r2;

void thread_worker1() {
  for (;;) {
    sem_wait(&sem1);
    while (rand() % 6 != 0)
      ;
    r1 = x;
    y = 1;
    // asm volatile("mfence" ::: "memory");
    // asm volatile("" ::: "memory");

    sem_post(&end1);
  }
}

void thread_worker2() {
  for (;;) {
    sem_wait(&sem2);
    while (rand() % 6 != 0)
      ;
    r2 = y;
    x = 1;
    // asm volatile("mfence" ::: "memory");
    // asm volatile("" ::: "memory");
    sem_post(&end2);
  }
}

int main() {
  sem_init(&sem1, 0, 0);
  sem_init(&sem2, 0, 0);
  sem_init(&end1, 0, 0);
  sem_init(&end2, 0, 0);
  ulong iterations = 0;
  std::thread work1(thread_worker1);
  std::thread work2(thread_worker2);
  for (;;) {
    x = 0, y = 0;
    sem_post(&sem1);
    sem_post(&sem2);
    sem_wait(&end1);
    sem_wait(&end2);

    if (r1 == 1) {
      if (r2 == 1)
        std::cout << "Iteration " << iterations << std::endl;
      assert(r2 != 1);
    }
    iterations++;
    r1 = 0, r2 = 0;
    if (iterations % HEARBEAT == 0)
      std::cout << "alive" << std::endl;
  }
  sem_post(&sem1);
  sem_post(&sem2);
  work1.join();
  work2.join();
  return 0;
}