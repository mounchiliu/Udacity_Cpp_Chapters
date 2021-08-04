## Concurrency Support in C++11

The concurrency support in C++ makes it possible for a program to execute multiple threads in parallel. Concurrency was first introduced into the standard with C++11. Since then, new concurrency features have been added with each new standard update, such as in C++14 and C++17. Before C++11, concurrent behavior had to be implemented using native concurrency support from the OS, using POSIX Threads, or third-party libraries such as BOOST. The standardization of concurrency in C++ now makes it possible to develop cross-platform concurrent programs, which is as significant improvement that saves time and reduces error proneness. Concurrency in C++ is provided by the thread support library, which can be accessed by including the <thread> header.

A running program consists of at least one thread. When the main function is executed, we refer to it as the "main thread". Threads are uniquely identified by their thread ID, which can be particularly useful for debugging a program. The code on the right prints the thread identifier of the main thread and outputs it to the console:
    
<img src="./C2-2-A3.png"/>

These are the results when run:
    
<img src="./img_0.png"/>
    
You can compile this code from the terminal in the lower right using `g++` as follows:
```
g++ example_1.cpp
```
and run it with
```
./a.out
```
    
Note: The actual thread id and process exit message will vary from machine to machine.

<!--
%%ulab_page_divider
--><hr/>

Also, it is possible to retrieve the number of available CPU cores of a system. The example on the right prints the number of CPU cores to the console.

<img src="./C2-2-A2b.png"/>

These are the results from a local machine at the time of writing:

<img src="./img_1.png"/>

Try running this code to see what results you get!

<!--
%%ulab_page_divider
--><hr/>

## Starting a second thread

In this section, we will start a second thread in addition to the main thread of our program. To do this, we need to construct a thread object and pass it the function we want to be executed by the thread. Once the thread enters the runnable state, the execution of the associated thread function may start at any point in time.

<figure><img src="./C2-2-A3a.png"/></figure>

After the thread object has been constructed, the main thread will continue and execute the remaining instructions until it reaches the end and returns. It is possible that by this point in time, the thread will also have finished. But if this is not the case, the main program will terminate and the resources of the associated process will be freed by the OS. As the thread exists within the process, it can no longer access those resources and thus not finish its execution as intended.

To prevent this from happening and have the main program wait for the thread to finish the execution of the thread function, we need to call `join()` on the thread object. This call will only return when the thread reaches the end of the thread function and block the main thread until then.

The code on the right shows how to use `join()` to ensure that `main()` waits for the thread `t` to finish its operations before returning. It uses the function `sleep_for()`, which pauses the execution of the respective threads for a specified amount of time. The idea is to simulate some work to be done in the respective threads of execution.

<img src="./C2-2-A3b.png"/>

To compile this code with `g++`, you will need to use the `-pthread` flag. `pthread` adds support for multithreading with the pthreads library, and the option sets flags for both the preprocessor and linker:
```
g++ example_3.cpp -pthread
```

**Note:** If you compile without the `-pthread` flag, you will see an error of the form:
`undefined reference to pthread_create`.
You will need to use the `-pthread` flag for all other multithreaded examples in this course going forward.

The code produces the following output:

<img src="./img_2.png"/>

Not surprisingly, the main function finishes before the thread because the delay inserted into the thread function is much larger than in the main path of execution. The call to `join()` at the end of the main function ensures that it will not prematurely return. As an experiment, comment out `t.join()` and execute the program. What do you expect will happen?


<!--
%%ulab_page_divider
--><hr/>

## Randomness of events

One very important trait of concurrent programs is their non-deterministic behavior. It can not be predicted which thread the scheduler will execute at which point in time. In the code on the right, the amount of work to be performed both in the thread function and in main has been split into two separate jobs.

The console output shows that the work packages in both threads have been interleaved with the first package being performed before the second package.

<img src="./img_3.png"/>

Interestingly, when executed on my local machine, the order of execution has changed. Now, instead of finishing the second work package in the thread first, main gets there first.

<img src="./img_4.png"/>

Executing the code several times more shows that the two versions of program output interchange in a seemingly random manner. This element of randomness is an important characteristic of concurrent programs and we have to take measures to deal with it in a controlled way that prevent unwanted behavior or even program crashes.

**Reminder:** You will need to use the `-pthread` flag when compiling this code, just as you did with the previous example. This flag will be needed for all future multithreaded programs in this course as well.

<!--
%%ulab_page_divider
--><hr/>

## Using join() as a barrier

In the previous example, the order of execution is determined by the scheduler. If we wanted to ensure that the thread function completed its work before the main function started its own work (because it might be waiting for a result to be available), we could achieve this by repositioning the call to join.

In the file on the right, the `.join()` has been moved to before the work in `main()`. The order of execution now always looks like the following:

<img src="./img_5.png"/>

In later sections of this course, we will make extended use of the join() function to carefully control the flow of execution in our programs and to ensure that results of thread functions are available and complete where we need them to be.

<!--
%%ulab_page_divider
--><hr/>

## Detach

Let us now take a look at what happens if we don’t join a thread before its destructor is called. When we comment out join in the example above and then run the program again, it aborts with an error. The reason why this is done is that the designers of the C++ standard wanted to make debugging a multi-threaded program easier: Having the program crash forces the programer to remember joining the threads that are created in a proper way. Such a hard error is usually much easier to detect than soft errors that do not show themselves so obviously.

There are some situations however, where it might make sense to not wait for a thread to finish its work. This can be achieved by "detaching" the thread, by which the internal state variable "joinable" is set to "false". This works by calling the `detach()` method on the thread. The destructor of a detached thread does nothing: It neither blocks nor does it terminate the thread. In the following example, detach is called on the thread object, which causes the main thread to immediately continue until it reaches the end of the program code and returns. Note that a detached thread can not be joined ever again.

<img src="./C2-3-A6.png"/>

You can run the code above using `example_6.cpp` over on the right side of the screen.

Programmers should be very careful though when using the `detach()`-method. You have to make sure that the thread does not access any data that might get out of scope or be deleted. Also, we do not want our program to terminate with threads still running. Should this happen, such threads will be terminated very harshly without giving them the chance to properly clean up their resources - what would usually happen in the destructor. So a well-designed program usually has a well-designed mechanism for joining all threads before exiting.

<!--
%%ulab_page_divider
--><hr/>

## join vs detch


    Q: 在一个线程中，开了另一个线程去干另一件事，使用join函数后，原始线程会等待新线程执行结束之后，再去销毁线程对象。
    这样有什么好处？
    
    - 因为它要等到新线程执行完，再销毁，线程对象，这样如果新线程使用了共享变量，等到新线程执行完再销毁这个线程对象，不会产生异常。
    - 如果不使用join，使用detch，那么新线程就会与原线程分离，如果原线程先执行完毕，销毁线程对象及局部变量，并且新线程有共享变量或引用之类，这样新线程可能使用的变量，就变成未定义，产生异常或不可预测的错误。
    - 所以，当你确定程序没有使用共享变量或引用之类的话，可以使用detch函数，分离线程。
    - 但是使用join可能会造成性能损失，因为原始线程要等待新线程的完成，所以有些情况（前提是你知道这种情况，如上）使用detch会更好。
    
    
    - join，主线程就会等待子线程运行结束，然后主线程在运行，直到结束，传统编程就是如此，试想如果子线程访问主线程中的资源，如果主线程结束了，其中资源释放，但是子线程还在运行，要访问资源，这样的话肯定会出问题的，
    - detach，即分离的意思，一旦detach，就讲子线程交给系统托管，与进程，主线程无关了，这种情况下，就很可能主线程结束，子线程还在运行，所以detach就引发出问题

    
    
    
## Quiz: Starting your own threads

In the code on the right, you will find a thread function called `threadFunctionEven`, which is passed to a thread `t`. In this example, the thread is immediately detached after creation. To ensure main does not quit before the thread is finished with its work, there is a `sleep_for` call at the end of main.

Please create a new function called `threadFunctionOdd` that outputs the string &quot;Odd threadn&quot;. Then write a for-loop that starts 6 threads and immediately detaches them. Based on wether the increment variable is even or odd, you should pass the respective function to the thread.

```cpp
#include <iostream>
#include <thread>

void threadFunctionEven()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(1)); // simulate work
    std::cout << "Even thread\n";
}

/* Student Task START */
void threadFunctionOdd()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(1)); // simulate work
    std::cout << "Odd thread\n";
}
/* Student Task END */

int main()
{
    /* Student Task START */
    for (int i = 0; i < 6; ++i)
    {
        if (i % 2 == 0)
        {
            std::thread t(threadFunctionEven);
            t.detach();
        }
        else
        {
            std::thread t(threadFunctionOdd);
            t.detach();
        }
    }
    /* Student Task END */

    // ensure that main does not return before the threads are finished
    std::this_thread::sleep_for(std::chrono::milliseconds(1)); // simulate work

    std::cout << "End of main is reached" << std::endl;
    return 0;
}
```

<!--
%%ulab_page_divider
--><hr/>

Run the program several times and look the console output. What do you observe? As a second experiment, comment out the `sleep_for` function in the main thread. What happens to the detached threads in this case?

<img src="./C2-3-A6-1.png"/>

The order in which even and odd threads are executed changes. Also, some threads are executed after the main function reaches its end. When `sleep_for` is removed, threads will not finish before the program terminates.

## detach 注意事项：   
总结：
    1、使用引用和指针是要注意 使用值传递，不要使用引用传递，杜绝使用指针传递；

　　2、对于类对象，建议使用引用来接收，如果使用引用会只会构造两次 （构造+拷贝构造），而使用传值会构造三次 (见 https://www.cnblogs.com/chen-cs/p/13056703.html)；

　　3、在detach下要避免隐式转换，因为此时子线程可能还来不及转换主线程就结束了，应该在构造线程时，用参数构造一个临时对象传入。
    
### example

**Q.1**
    
![image](https://user-images.githubusercontent.com/47606318/128193980-c5afb191-b9ca-4820-96c0-29a9b67d83d1.png)

第一个构造是在主程序内，对应第一个析构
第二个是拷贝构造，对应thread内，对应第二个析构， 因为如果线程函数中形参的是引用类型，那主线程会将传入的值做一次赋值. thread_func内部的对应对象地址变了，而传回来的没有变
    
可以通过以下方式查看
    
···cpp
    {
    cout << "主线程开始" << endl;
    int i = 4;
    A a(i);
    std::cout << " 1 " << &a << std::endl;
    thread t(test, a);
    std::cout << "2" << &a << std::endl;
    t.join();
    cout << "主线程结束！" << endl;
    return 0;
}
···
    
    
Note: 形参：函数的arg
      实参：调用函数的arg
 
**Q2**    
![image](https://user-images.githubusercontent.com/47606318/128196627-0f4ab0f0-9b07-4451-a412-080b788a2835.png)
![image](https://user-images.githubusercontent.com/47606318/128196672-1a438bb1-2a16-4237-bd79-4fa99cfa1a85.png)

如果函数的形参是类的对象，调用函数参数是值传递， 会调用拷贝函数， 即函数调用时候（参数为类）值传递会产生临时变量。
thread内部获取这个对象时， 又会调用拷贝函数
所以是三次
    
因此， 接收时使用引用更高效
    
总结：
![image](https://user-images.githubusercontent.com/47606318/128201480-cef08439-9423-4695-86fe-be15c181232f.png)

**Q3**
    
如果使用隐式传递
    
```cpp
#include <iostream>
#include <thread>
    
using namespace std;
    
class A {
public:
    int ai;
    A(int i) : ai(i)
    {
        cout << "构造" << this << endl;
    }

    A(const A& a) :ai(a.ai) {
        cout << "拷贝构造" << this << endl;
    }

    ~A()
    {
        cout << "析构" << this << endl;
    }
};
    
//void test(const A a)
void test(const A b)
{
    cout << &b << std::endl;
    cout << "子线程开始" << endl;
    cout << "子线程结束" << endl;
    return;
}
    
int main()
{
    cout << "主线程开始" << endl;
    int i = 4;
    A a(i);
    std::cout << "1 " << &a << std::endl;
    //test(a);
    thread t(test, i); //隐式传递，i为int，隐式传递一个object
    t.join();
    cout << "主线程结束！" << endl;
    return 0;
}
```

- 如果把join 替换成 detach，发现若使用隐式传递，会在主线程创造thread是创造一个对象待以后传入thread，传入的操作如果此可能发生在主线程结束之后， 会发现无法创建object，因为与object相关的资源已经销毁  

- 而如果传递一个临时对象 ![image](https://user-images.githubusercontent.com/47606318/128204440-91ac62cf-a6d5-45e5-b261-78efa844267b.png)
  就算detach， 也ok，创建thread时会对创建的对象A(i)进行拷贝，传入时就传拷贝好了的。
    
### Reference:
https://www.cnblogs.com/chen-cs/p/13056703.html
https://study-life.blog.csdn.net/article/details/108099819
    
    
