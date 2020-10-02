code Kernel

  -- RISHAB

-----------------------------  InitializeScheduler  ---------------------------------

  function InitializeScheduler ()
    --
    -- This routine assumes that we are in System mode.  It sets up the
    -- thread scheduler and turns the executing program into "main-thread".
    -- After exit, we can execute "Yield", "Fork", etc.  Upon return, the
    -- main-thread will be executing with interrupts enabled.
    --
      Cleari ()
      print ("Initializing Thread Scheduler...\n")
      readyList = new List [Thread]
      threadsToBeDestroyed = new List [Thread]
      mainThread = new Thread
      mainThread.Init ("main-thread")
      mainThread.status = RUNNING
      idleThread = new Thread
      idleThread.Init ("idle-thread")
      idleThread.Fork (IdleFunction, 0)
      currentThread = & mainThread
      FatalError = FatalError_ThreadVersion       -- Use a routine which prints threadname
      currentInterruptStatus = ENABLED
      Seti ()
    endFunction
    
-----------------------------  Mutex  ---------------------------------

  behavior Mutex
    -- This class provides the following methods:
    --    Lock()
    --         Acquire the mutex if free, otherwise wait until the mutex is
    --         free and then get it.
    --    Unlock()
    --         Release the mutex.  If other threads are waiting, then
    --         wake up the oldest one and give it the lock.
    --    Init()
    --         Each mutex must be initialized.
    --    IsHeldByCurrentThread()
    --         Return TRUE iff the current (invoking) thread holds a lock
    --         on the mutex.
    --    passMutex()
    --         This is a new method added to implement Hoare-Semantics.It
    --         takes two aguments one the thread that should be given the
    --         mutex and the current thread.

       -----------  Mutex . Init  -----------

       method Init ()
           waitingThreads = new List [Thread]
         endMethod

       -----------  Mutex . Lock  -----------

       method Lock ()
           var
             oldIntStat: int
           if heldBy == currentThread
             FatalError ("Attempt to lock a mutex by a thread already holding it")
           endIf
           oldIntStat = SetInterruptsTo (DISABLED)
           if !heldBy
             heldBy = currentThread
           else
             waitingThreads.AddToEnd (currentThread)
             currentThread.Sleep ()
           endIf
           oldIntStat = SetInterruptsTo (oldIntStat)
         endMethod

       -----------  Mutex . Unlock  -----------

       method Unlock ()
           var
             oldIntStat: int
             t: ptr to Thread
           if heldBy != currentThread
             FatalError ("Attempt to unlock a mutex by a thread not holding it")
           endIf
           oldIntStat = SetInterruptsTo (DISABLED)
           t = waitingThreads.Remove ()
           if t
             t.status = READY
             readyList.AddToEnd (t)
             heldBy = t
           else
             heldBy = null
           endIf
           oldIntStat = SetInterruptsTo (oldIntStat)
         endMethod

       -----------  Mutex . IsHeldByCurrentThread  -----------

       method IsHeldByCurrentThread () returns bool
           return heldBy == currentThread
         endMethod

       ----------  Mutex.passMutex ------------
       method passMutex(t:ptr to Thread,c:ptr to Thread)
        var
           oldIntStat:int
           oldIntStat = SetInterruptsTo(DISABLED)
              if heldBy == c
                heldBy = t
                t.status = READY
                readyList.AddToEnd(t)
              endIf
           oldIntStat = SetInterruptsTo(oldIntStat)
      endMethod
       ----------  Mutex.passMutex ------------

  endBehavior

----------------------------- Hoare-Condition  ---------------------------------

  behavior HoareCondition
    -- This class is used to implement monitors.  Each monitor will have a
    -- mutex lock and one or more condition variables.  The lock ensures that
    -- only one process at a time may execute code in the monitor.  Within the
    -- monitor code, a thread can execute Wait() and Signal() operations
    -- on the condition variables to make sure certain condions are met.
    --
    -- The condition variables here implement "Mesa-style" semantics, which
    -- means that in the time between a Signal() operation and the awakening
    -- and execution of the corrsponding waiting thread, other threads may
    -- have snuck in and run.  The waiting thread should always re-check the
    -- data to ensure that the condition which was signalled is still true.
    --
    -- This class provides the following methods:
    --    Wait(mutex)
    --         This method assumes the mutex has alreasy been locked.
    --         It unlocks it, and goes to sleep waiting for a signal on
    --         this condition.  When the signal is received, this method
    --         re-awakens, re-locks the mutex, and returns.
    --    Signal(mutex)
    --         If there are any threads waiting on this condition, this
    --         method will wake up that thread and hand it the monitors mutex through passMutex method.
    --         Since the thread hands the mutex to newly woken up thread
    --         it, should wait till the newly woken thread or any other thread wakes it up
    --         it should re-acquire the mutex if it was woken up by an operation other than signal.
    --    Init()
    --         Each condition must be initialized.

      ----------  Hoare-Condition . Init  ----------

      method Init ()
          waitingThreads = new List [Thread]
        endMethod

      ----------  Hoare-Condition . Wait  ----------

      method Wait (mutex: ptr to Mutex)
          var
            oldIntStat: int
          oldIntStat = SetInterruptsTo (DISABLED)

          if ! mutex.IsHeldByCurrentThread ()
            FatalError ("Attempt to wait on condition when mutex is not held")
          endIf

          mutex.Unlock ()
          waitingThreads.AddToEnd (currentThread)
          currentThread.Sleep ()
          oldIntStat = SetInterruptsTo (oldIntStat)
        endMethod

      ----------  Hoare-Condition . Signal  ----------

      method Signal (mutex: ptr to Mutex)
          var
            oldIntStat: int
            t: ptr to Thread

          oldIntStat = SetInterruptsTo (DISABLED)

          if ! mutex.IsHeldByCurrentThread ()
            FatalError ("Attempt to signal a condition when mutex is not held")
          endIf

          t = waitingThreads.Remove ()

          if t
            mutex.passMutex(t,currentThread)
          endIf

          oldIntStat = SetInterruptsTo (oldIntStat)
        endMethod
  endBehavior
-----------------------------  ThreadManager  ---------------------------------

  behavior ThreadManager

      ----------  ThreadManager . Init  ----------

      method Init ()
        --
        -- This method is called once at kernel startup time to initialize
        -- the one and only "ThreadManager" object.
        --
          var
             i:int
             p:ptr to Thread

          threadTable = new array of Thread {MAX_NUMBER_OF_PROCESSES of new Thread}
          freeList = new List[Thread]

          print("Initializing Thread Manager...\n")

          threadManagerLock = new Mutex
          threadManagerLock.Init()
          aThreadBecameFree = new Condition
          aThreadBecameFree.Init()

          for i=0 to MAX_NUMBER_OF_PROCESSES-1
           threadTable[i].Init("resource threads")
           threadTable[i].status = UNUSED
          endFor

          for i=0 to MAX_NUMBER_OF_PROCESSES-1
           p = &threadTable[i]
           freeList.AddToFront(p)
          endFor

        endMethod

      ----------  ThreadManager . Print  ----------

      method Print ()
        --
        -- Print each thread.  Since we look at the freeList, this
        -- routine disables interrupts so the printout will be a
        -- consistent snapshot of things.
        --
        var i, oldStatus: int
          oldStatus = SetInterruptsTo (DISABLED)
          print ("Here is the thread table...\n")
          for i = 0 to MAX_NUMBER_OF_PROCESSES-1
            print ("  ")
            printInt (i)
            print (":")
            ThreadPrintShort (&threadTable[i])
          endFor
          print ("Here is the FREE list of Threads:\n   ")
          freeList.ApplyToEach (PrintObjectAddr)
          nl ()
          oldStatus = SetInterruptsTo (oldStatus)
        endMethod

      ----------  ThreadManager . GetANewThread  ----------

      method GetANewThread () returns ptr to Thread
        --
        -- This method returns a new Thread; it will wait
        -- until one is available.
        --
          var
             t:ptr to Thread

          if !threadManagerLock.IsHeldByCurrentThread()
           threadManagerLock.Lock()
          endIf

           while freeList.IsEmpty()
            aThreadBecameFree.Wait(&threadManagerLock)
           endWhile

            t = freeList.Remove()

            --if t != NULL
             t.status = JUST_CREATED
            --endIf

          if threadManagerLock.IsHeldByCurrentThread()
           threadManagerLock.Unlock()
          endIf

          return t

        endMethod

      ----------  ThreadManager . FreeThread  ----------

      method FreeThread (th: ptr to Thread)
        --
        -- This method is passed a ptr to a Thread;  It moves it
        -- to the FREE list.
        --

          if !threadManagerLock.IsHeldByCurrentThread()
           threadManagerLock.Lock()
          endIf

           th.status = UNUSED
           freeList.AddToEnd(th)
           aThreadBecameFree.Signal(&threadManagerLock)

          if threadManagerLock.IsHeldByCurrentThread()
           threadManagerLock.Unlock()
          endIf

        endMethod

    endBehavior

-----------------------------  ProcessManager  ---------------------------------

  behavior ProcessManager

      ----------  ProcessManager . Init  ----------

      method Init ()
        --
        -- This method is called once at kernel startup time to initialize
        -- the one and only "processManager" object.
        --
        var
           i:int
           t:ptr to ProcessControlBlock

         processTable = new array of ProcessControlBlock {MAX_NUMBER_OF_PROCESSES of new ProcessControlBlock}
         processManagerLock = new Mutex
         aProcessBecameFree = new Condition
         aProcessDied = new Condition
         processManagerLock.Init()
         aProcessBecameFree.Init()
         aProcessDied.Init()
         freeList = new List[ProcessControlBlock]
         nextPid = 1

         for i=0 to MAX_NUMBER_OF_PROCESSES-1
          processTable[i].Init()
         endFor

         for i=0 to MAX_NUMBER_OF_PROCESSES-1
          t = &processTable[i]
          freeList.AddToFront(t)
         endFor

       endMethod

      ----------  ProcessManager . Print  ----------

      method Print ()
        --
        -- Print all processes.  Since we look at the freeList, this
        -- routine disables interrupts so the printout will be a
        -- consistent snapshot of things.
        --
        var i, oldStatus: int
          oldStatus = SetInterruptsTo (DISABLED)
          print ("Here is the process table...\n")
          for i = 0 to MAX_NUMBER_OF_PROCESSES-1
            print ("  ")
            printInt (i)
            print (":")
            processTable[i].Print ()
          endFor
          print ("Here is the FREE list of ProcessControlBlocks:\n   ")
          freeList.ApplyToEach (PrintObjectAddr)
          nl ()
          oldStatus = SetInterruptsTo (oldStatus)
        endMethod

      ----------  ProcessManager . PrintShort  ----------

      method PrintShort ()
        --
        -- Print all processes.  Since we look at the freeList, this
        -- routine disables interrupts so the printout will be a
        -- consistent snapshot of things.
        --
        var i, oldStatus: int
          oldStatus = SetInterruptsTo (DISABLED)
          print ("Here is the process table...\n")
          for i = 0 to MAX_NUMBER_OF_PROCESSES-1
            print ("  ")
            printInt (i)
            processTable[i].PrintShort ()
          endFor
          print ("Here is the FREE list of ProcessControlBlocks:\n   ")
          freeList.ApplyToEach (PrintObjectAddr)
          nl ()
          oldStatus = SetInterruptsTo (oldStatus)
        endMethod

      ----------  ProcessManager . GetANewProcess  ----------

      method GetANewProcess () returns ptr to ProcessControlBlock
        --
        -- This method returns a new ProcessControlBlock; it will wait
        -- until one is available.
        --
          var
             t:ptr to ProcessControlBlock

          if !processManagerLock.IsHeldByCurrentThread()
            processManagerLock.Lock()
          endIf

           while freeList.IsEmpty()
            aProcessBecameFree.Wait(&processManagerLock)
           endWhile

            t = freeList.Remove()

            --if t != NULL
             t.pid = nextPid
             t.status = ACTIVE
             nextPid = nextPid + 1
            --endIf

           if processManagerLock.IsHeldByCurrentThread()
             processManagerLock.Unlock()
           endIf

          return t

        endMethod

      ----------  ProcessManager . FreeProcess  ----------

      method FreeProcess (p: ptr to ProcessControlBlock)
        --
        -- This method is passed a ptr to a Process;  It moves it
        -- to the FREE list.
        --
          if !processManagerLock.IsHeldByCurrentThread()
           processManagerLock.Lock()
          endIf

           p.status = FREE
           freeList.AddToEnd(p)
           aProcessBecameFree.Signal(&processManagerLock)

          if processManagerLock.IsHeldByCurrentThread()
           processManagerLock.Unlock()
          endIf
        endMethod


    endBehavior

-----------------------------  FrameManager  ---------------------------------

  behavior FrameManager

      ----------  FrameManager . Init  ----------

      method Init ()
        --
        -- This method is called once at kernel startup time to initialize
        -- the one and only "frameManager" object.
        --
        var i: int
          print ("Initializing Frame Manager...\n")
          framesInUse = new BitMap
          framesInUse.Init (NUMBER_OF_PHYSICAL_PAGE_FRAMES)
          numberFreeFrames = NUMBER_OF_PHYSICAL_PAGE_FRAMES
          frameManagerLock = new Mutex
          frameManagerLock.Init ()
          newFramesAvailable = new Condition
          newFramesAvailable.Init ()
          newFramesRequested = new Condition
          newFramesRequested.Init()
          framesRequestList = 0

          -- Check that the area to be used for paging contains zeros.
          -- The BLITZ emulator will initialize physical memory to zero, so
          -- if by chance the size of the kernel has gotten so large that
          -- it runs into the area reserved for pages, we will detect it.
          -- Note: this test is not 100%, but is included nonetheless.
          for i = PHYSICAL_ADDRESS_OF_FIRST_PAGE_FRAME
                   to PHYSICAL_ADDRESS_OF_FIRST_PAGE_FRAME+300
                   by 4
            if 0 != *(i asPtrTo int)
              FatalError ("Kernel code size appears to have grown too large and is overflowing into the frame region")
            endIf
          endFor
        endMethod

      ----------  FrameManager . Print  ----------

      method Print ()
        --
        -- Print which frames are allocated and how many are free.
        --
          frameManagerLock.Lock ()
          print ("FRAME MANAGER:\n")
          printIntVar ("  numberFreeFrames", numberFreeFrames)
          print ("  Here are the frames in use: \n    ")
          framesInUse.Print ()
          frameManagerLock.Unlock ()
        endMethod

      ----------  FrameManager . GetAFrame  ----------

      method GetAFrame () returns int
        --
        -- Allocate a single frame and return its physical address.  If no frames
        -- are currently available, wait until the request can be completed.
        --
          var f, frameAddr: int

          -- Acquire exclusive access to the frameManager data structure...
          frameManagerLock.Lock ()

          -- Wait until we have enough free frames to entirely satisfy the request...
          while numberFreeFrames < 1
            newFramesAvailable.Wait(&frameManagerLock)
          endWhile

          -- Find a free frame and allocate it...
          f = framesInUse.FindZeroAndSet ()
          numberFreeFrames = numberFreeFrames - 1

          -- Unlock...
          frameManagerLock.Unlock ()

          -- Compute and return the physical address of the frame...
          frameAddr = PHYSICAL_ADDRESS_OF_FIRST_PAGE_FRAME + (f * PAGE_SIZE)
          -- printHexVar ("GetAFrame returning frameAddr", frameAddr)
          return frameAddr
        endMethod

      ----------  FrameManager . GetNewFrames  ----------

      method GetNewFrames (aPageTable: ptr to AddrSpace, numFramesNeeded: int)

        var
            i:int
            frameAddr:int
            f:int

        if !frameManagerLock.IsHeldByCurrentThread()
          frameManagerLock.Lock()
        endIf

           framesRequestList = framesRequestList + 1

           if framesRequestList > 1
             newFramesRequested.Wait(&frameManagerLock)
           endIf

           while numFramesNeeded > numberFreeFrames
              newFramesAvailable.Wait(&frameManagerLock)
           endWhile

          -- Do frame setting
            for i=0 to numFramesNeeded - 1
              f = framesInUse.FindZeroAndSet()
              frameAddr = PHYSICAL_ADDRESS_OF_FIRST_PAGE_FRAME + (f * PAGE_SIZE)
              aPageTable.SetFrameAddr(i,frameAddr)
            endFor

           aPageTable.numberOfPages = numFramesNeeded

           numberFreeFrames = numberFreeFrames - numFramesNeeded
           framesRequestList = framesRequestList - 1

           newFramesRequested.Signal(&frameManagerLock)

        if frameManagerLock.IsHeldByCurrentThread()
          frameManagerLock.Unlock()
        endIf
        endMethod

      ----------  FrameManager . ReturnAllFrames  ----------

      method ReturnAllFrames (aPageTable: ptr to AddrSpace)

         var
            i:int
            numFramesReturned:int
            bitNumber:int
            frameAddr:int

         if !frameManagerLock.IsHeldByCurrentThread()
          frameManagerLock.Lock()
         endIf

           numFramesReturned = aPageTable.numberOfPages

           -- Free the frames

           for i=0 to numFramesReturned - 1
            frameAddr = aPageTable.ExtractFrameAddr(i)
            bitNumber = (frameAddr - PHYSICAL_ADDRESS_OF_FIRST_PAGE_FRAME)/PAGE_SIZE
            framesInUse.ClearBit(bitNumber)
           endFor

           numberFreeFrames = numberFreeFrames + numFramesReturned

           newFramesAvailable.Signal(&frameManagerLock)

         if frameManagerLock.IsHeldByCurrentThread()
          frameManagerLock.Unlock()
         endIf
        endMethod

    endBehavior
endCode