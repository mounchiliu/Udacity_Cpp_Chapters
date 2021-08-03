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


/*
join vs detch
join函数与detch

    Q: 在一个线程中，开了另一个线程去干另一件事，使用join函数后，原始线程会等待新线程执行结束之后，再去销毁线程对象。
    这样有什么好处？
    
    - 因为它要等到新线程执行完，再销毁，线程对象，这样如果新线程使用了共享变量，等到新线程执行完再销毁这个线程对象，不会产生异常。
    - 如果不使用join，使用detch，那么新线程就会与原线程分离，如果原线程先执行完毕，销毁线程对象及局部变量，并且新线程有共享变量或引用之类，这样新线程可能使用的变量，就变成未定义，产生异常或不可预测的错误。
    - 所以，当你确定程序没有使用共享变量或引用之类的话，可以使用detch函数，分离线程。
    - 但是使用join可能会造成性能损失，因为原始线程要等待新线程的完成，所以有些情况（前提是你知道这种情况，如上）使用detch会更好。
*/
