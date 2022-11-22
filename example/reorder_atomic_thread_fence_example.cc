#include <atomic>
#include <cstdlib>
#include <iostream>
#include <semaphore.h>
#include <thread>

sem_t sem1, sem2;
sem_t end1, end2;
int x, y;
int r1, r2;
// std::atomic<int> r1(0), r2(0);
int threshold = 0;

void thread_worker1() {
  for (;;) {
    if (threshold >= 20)
      return;

    sem_wait(&sem1);

    y = 1;
    std::atomic_thread_fence(std::memory_order_seq_cst);
    r1 = x;

    sem_post(&end1);
  }
}

void thread_worker2() {
  for (;;) {
    if (threshold >= 20)
      return;

    sem_wait(&sem2);

    x = 1;
    std::atomic_thread_fence(std::memory_order_seq_cst);
    r2 = y;

    sem_post(&end2);
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
    x = 100, y = 100;
    r1 = 1;
    r2 = 1;
    sem_post(&sem1);
    sem_post(&sem2);
    sem_wait(&end1);
    sem_wait(&end2);

    // int tmpr1 = r1.load(std::memory_order_seq_cst);
    // int tmpr2 = r2.load(std::memory_order_seq_cst);
    if (r1 == 100 && r2 == 100) {
      threshold++;
      std::cout << iterations << " found reordered happend " << threshold
                << ", r1=" << r1 << ", r2=" << r2 << std::endl;

      if (threshold >= 20)
        break;
    }
    iterations++;
  }
  sem_post(&sem1);
  sem_post(&sem2);
  work1.join();
  work2.join();
  return 0;
}