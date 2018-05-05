///*
// * Interval-timer demo program.
// * Hebrew University OS course.
// * Author: OS, os@cs.huji.ac.il
// */
//
//#include <stdio.h>
//#include <signal.h>
//#include <sys/time.h>
//
//
//int gotit = 0;
//
//void timer_handler(int sig)
//{
//    setTimer(1000000);
//    gotit = 1;
//    printf("Timer expired\n");
//}
//
//void setTimer(int interval){
//    struct sigaction sa;
//    struct itimerval timer;
//
//    // Install timer_handler as the signal handler for SIGVTALRM.
//    sa.sa_handler = &timer_handler;
//    if (sigaction(SIGVTALRM, &sa, NULL) < 0) {
//        printf("sigaction error.");
//    }
//    int sec = interval / 1000000;
//    int microsec = interval % 1000000;
//    // Start counting time:
//    timer.it_value.tv_sec = sec;
//    timer.it_value.tv_usec = microsec;
//    timer.it_interval.tv_sec = sec;
//    timer.it_interval.tv_usec = microsec;
//
//    // Start a virtual timer. It counts down whenever this process is executing.
//    if (setitimer (ITIMER_VIRTUAL, &timer, NULL)) {
//        printf("setitimer error.");
//    }
//
//}
//
//int main(void) {
//
//    setTimer(4000000);
//	for(;;) {
//		if (gotit) {
//			printf("Got it!\n");
//			gotit = 0;
//		}
//	}
//}

