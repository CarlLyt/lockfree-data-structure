#include <assert.h>
#include <atomic>
#include <conf.h>
#include <cstdlib>
#include <iostream>
#include <semaphore.h>
#include <thread>
sem_t sem1, sem2;
sem_t end1, end2;
std::atomic<int> x(1), y(1);
std::atomic<int> r1(1), r2(1);

void thread_worker1() {
  for (;;) {
    sem_wait(&sem1);
    y = 100;
    r1.store(1, std::memory_order_release);
    sem_post(&end1);
  }
}

void thread_worker2() {
  for (;;) {
    sem_wait(&sem2);
    while (r1.load(std::memory_order_acquire) != 1)
      ;
    assert(y == 100);
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
    x = 0, y = 0;
    sem_post(&sem1);
    sem_post(&sem2);
    sem_wait(&end1);
    sem_wait(&end2);

    if (r1.load(std::memory_order_acquire) == 0 &&
        r2.load(std::memory_order_acquire) == 0) {
      assert_action(iterations);
      std::abort();
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