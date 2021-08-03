#include <iostream>
#include <thread>

void threadFunction() {
  std::this_thread::sleep_for(std::chrono::milliseconds(100));  // simulate work
  std::cout << "Finished work in thread. \n";
}

int main() {
  // create thread
  std::thread t(threadFunction);

  // do something in main()
  std::this_thread::sleep_for(std::chrono::milliseconds(50));  // simulate work
  std::cout << "Finished work in main. \n";

  // join(): wait for thread to finish, and run the rest code in this thread
  t.join();

  return 0;
}
