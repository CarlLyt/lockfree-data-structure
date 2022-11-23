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
int flag = 0;

void thread_worker1() {
  for (;;) {
    sem_wait(&sem1);
    while (rand() % 101 != 0)
      ;
    y = 1;
    // asm volatile("mfence" ::: "memory");
    // asm volatile("" ::: "memory");
    r1 = x;
    flag = 1;
    sem_post(&end1);
  }
}

void thread_worker2() {
  for (;;) {
    sem_wait(&sem2);
    while (flag == 0) {
      std::this_thread::yield();
    }
    x = 1;
    // asm volatile("mfence" ::: "memory");
    // asm volatile("" ::: "memory");
    r2 = y;
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

    if (r1 == 0) {
      assert(r2 != 0);
    }
    iterations++;
    r1 = 1, r2 = 1;
    flag = 0;
    if (iterations % HEARBEAT == 0)
      std::cout << "alive" << std::endl;
  }
  sem_post(&sem1);
  sem_post(&sem2);
  work1.join();
  work2.join();
  return 0;
}