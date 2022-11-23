#include <assert.h>
#include <conf.h>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <semaphore.h>
#include <thread>
sem_t sem, end, finish;
int x, y;
int r1, r2;
int flag = 0;
void thread_worker1() {
  for (;;) {
    sem_wait(&sem);

    y = 1;
    r1 = x;
    flag = 1;
    sem_post(&end);
  }
}

void thread_worker2() {
  for (;;) {
    sem_wait(&end);
    x = 1;
    r2 = y;
    sem_post(&finish);
  }
}

int main() {
  sem_init(&sem, 0, 0);
  sem_init(&end, 0, 0);
  sem_init(&finish, 0, 0);
  ulong iterations = 0;
  std::thread work1(thread_worker1);
  std::thread work2(thread_worker2);
  for (;;) {
    x = 0, y = 0;
    sem_post(&sem);
    sem_wait(&finish);
    if (r1 == 0) {
      assert(r2 != 0);
    }
    iterations++;
    r1 = 1, r2 = 1;
    flag = 0;
    if (iterations % HEARBEAT == 0)
      std::cout << "alive" << std::endl;
  }
  work1.join();
  work2.join();
  return 0;
}