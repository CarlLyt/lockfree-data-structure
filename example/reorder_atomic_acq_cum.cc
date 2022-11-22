#include <assert.h>
#include <atomic>
#include <cstdlib>
#include <iostream>
#include <semaphore.h>
#include <thread>
sem_t sem1, sem2;
sem_t end1, end2;
int x, y;
int r1, r2;
std::atomic<int> flag(0);
int threshold = 0;

void thread_worker1() {
  for (;;) {
    if (threshold >= 20)
      return;
    sem_wait(&sem1);
    while (rand() % 101 != 0)
      ;
    y = 1;
    r1 = x;

    flag.store(y, std::memory_order_release);

    sem_post(&end1);
  }
}

void thread_worker2() {
  for (;;) {
    if (threshold >= 20)
      return;
    sem_wait(&sem2);
    while (rand() % 101 != 0)
      ;
    // upgrade to std::memory_order_acquire
    while (flag.load(std::memory_order_consume) == 0) {
      std::this_thread::yield();
    }

    x = 1;
    r2 = y;
    sem_post(&end2);
  }
}

int main() {
  std::kill_dependency(x);
  std::kill_dependency(y);
  std::kill_dependency(r1);
  std::kill_dependency(r2);
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

    if (r1 == 0 && r2 == 0) {
      threshold++;
      std::cout << iterations << " iterations, found reordered happend "
                << threshold << " times" << std::endl;
      if (threshold >= 20)
        break;
    }
    iterations++;
    r1 = 1, r2 = 1;
    flag = 0;
    if (iterations % 1000000 == 0)
      std::cout << "alive" << std::endl;
  }
  sem_post(&sem1);
  sem_post(&sem2);
  work1.join();
  work2.join();
  return 0;
}