﻿#ifndef HGL_THREAD_INCLUDE
#define HGL_THREAD_INCLUDE

#include<hgl/type/DataType.h>
#include<hgl/type/BaseString.h>
#include<hgl/thread/ThreadMutex.h>

#if HGL_OS == HGL_OS_Windows
	using thread_ptr=void *;       //windows版是HANDLE，但HANDLE其实就是void *，这里为了减少#include<windows.h>所以直接写为void *
#else
	#include<pthread.h>
	using thread_ptr=pthread_t;
#endif//HGL_OS == HGL_Windows
namespace hgl
{
	/**
	* 线程类.本类用于建立一个线程并管理它。<br>
	* Sleep和Exit函数是给线程内的程序使用的；而Start,Close,Pause,Rewind是给线程外的程序使用的。<br>
	* 使用本类只需要重载本类，并将代码写在Execute函数内即可。<br>
	* 执行大概为：<br>
	* if(ProcStartThread())		<br>
	* 	while(Execute());	<br>
	* else					<br>
	* 	return(0);
	* 						<br>
	* ProcEndThread();
	*/
	class Thread																					///线程类
	{
		friend void WaitThread(Thread **,int,double);

	protected:

		thread_ptr tp=0;

        ThreadMutex live_lock;

#if HGL_OS != HGL_OS_Windows
		virtual void SetCancelState(bool,bool=true);
        
        friend void *ThreadFunc(Thread *tc);
#else
        friend DWORD WINAPI ThreadFunc(Thread *tc);
#endif//HGL_OS != HGL_OS_Windows

	public:

		virtual ~Thread()
		{
            if(tp)
                Close();
		}

		template<typename T>
		void GetAddress(BaseString<T> &thread_addr_str)const                                       ///<取得线程地址字符串
		{
            T str[(sizeof(void *)+1)<<1];

            hgl::htos(str,(sizeof(void *)+1)<<1,(uint64)tp);

            thread_addr_str=str;
        }

		/**
		* 线程执行函数，会被反复调用
		* @return true 继续执行
		* @return false 退出执行
		*/
		virtual bool Execute()=0;                                                                   ///<线程的运行函数

		virtual bool ProcStartThread(){return(true);}												///<线程启动运行函数,在Execute前被调用
		virtual void ProcEndThread(){}																///<结程结束运行函数,在Execute后被调用

		virtual bool IsExitDelete()const{return true;}												///<返回在退出线程时，是否删除本对象

                bool IsLive()                                                                       ///<当前线程是否还活着
                {
                    if(live_lock.TryLock())
                    {
                        //成功了，证明这个线程已经关闭了
                        live_lock.Unlock();
                        return(false);
                    }

                    return(true);
                }

	public:	//线程运行控制

		virtual bool Start();																		///<开始运行当前线程
		virtual void Close();                                                                       ///<强制关闭当前线程(其它线程调)

		virtual bool IsCurThread();																	///<是否是当前线程

#if HGL_OS != HGL_OS_Windows
		virtual bool Cancel();																		///<放弃这个线程
#endif//HGL_OS != HGL_OS_Windows

        /**
         * 等待当前线程结束<br>
         * 注：此函数在Win平台下任何情况下都起作用，但在linux/bsd下，如果IsExitDelete提供true的值，此函数将不起作用
         */
		virtual void Wait(double time=0);                                                 			///<等待当前线程
	};//class Thread

	void WaitThread(Thread **,int,double time=0);													///<等待多个线程中的一个完成

	bool CreateThread(Thread *);																	///<创建一个线程
}//namespace hgl
#endif//HGL_THREAD_INCLUDE
