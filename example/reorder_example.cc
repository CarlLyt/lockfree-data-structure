#include <cstdlib>
#include <iostream>
#include <semaphore.h>
#include <thread>

sem_t sem1, sem2;
sem_t end1, end2;
int x, y;
int r1, r2;
int threshold = 0;

void thread_worker1() {
  for (;;) {
    sem_wait(&sem1);
    while (rand() % 9 != 0)
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
  for (;;) {

    sem_wait(&sem2);
    while (rand() % 9 != 0)
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