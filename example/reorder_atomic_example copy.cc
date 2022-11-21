#include <atomic>
#include <cstdlib>
#include <iostream>
#include <semaphore.h>
#include <thread>

sem_t sem1, sem2;
sem_t end1, end2;
std::atomic<int> x(0), y(0);
std::atomic<int> r1(0), r2(0);
int threshold = 0;

void thread_worker1() {
  for (;;) {
    if (threshold >= 20)
      return;

    sem_wait(&sem1);
    y.store(1, std::memory_order_seq_cst);
    r1.store(x.load(std::memory_order_seq_cst), std::memory_order_seq_cst);
    sem_post(&end1);
  }
}

void thread_worker2() {
  for (;;) {
    if (threshold >= 20)
      return;

    sem_wait(&sem2);
    x.store(1, std::memory_order_seq_cst);
    r2.store(y.load(std::memory_order_seq_cst), std::memory_order_seq_cst);
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
    r1.store(1, std::memory_order_seq_cst);
    r2.store(1, std::memory_order_seq_cst);
    sem_post(&sem1);
    sem_post(&sem2);
    sem_wait(&end1);
    sem_wait(&end2);

    if (r1.load(std::memory_order_seq_cst) == 100 &&
        r2.load(std::memory_order_seq_cst) == 100) {
      threshold++;
      std::cout << iterations << " found reordered happend " << threshold
                << " times" << std::endl;
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