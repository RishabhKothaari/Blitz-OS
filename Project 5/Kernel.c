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

----------------------------- ................. -----------------------

-----------------------------  DiskInterruptHandler  ------------------

  function DiskInterruptHandler ()
    --
    -- This routine is called when a disk interrupt occurs.  It will
    -- signal the "semToSignalOnCompletion" Semaphore and return to
    -- the interrupted thread.
    --
    -- This is an interrupt handler.  As such, interrupts will be DISABLED
    -- for the duration of its execution.
    --
-- Uncomment this code later...
      --FatalError ("DISK INTERRUPTS NOT EXPECTED IN PROJECT 4")

      currentInterruptStatus = DISABLED
      -- print ("DiskInterruptHandler invoked!\n")
      if diskDriver.semToSignalOnCompletion
        diskDriver.semToSignalOnCompletion.Up()
      endIf

    endFunction

-----------------------------  ProcessFinish  -------------------------

  function ProcessFinish (exitStatus: int)
    --
    -- This routine is called when a process is to be terminated.  It will
    -- free the resources held by this process and will terminate the
    -- current thread.
    --
      FatalError ("ProcessFinish is not implemented")
    endFunction

------------------------------ InitFirstProcess ------------------------
  -- This function is called by Main and it starts the process 
  -- by forking StartUserProcess which allocates resourses to 
  -- the newly created Thread.

  function InitFirstProcess()

    var
       th:ptr to Thread

    th = threadManager.GetANewThread()
    th.Init("UserProgram")
    th.Fork(StartUserProcess,0)

  endFunction

----------------------------- StartUserProcess -------------------------
  -- StartUserProcess - this function is called by every new process.
  -- This function allocates resources to newly created process - namely
  -- address space and process control block. And then shifts CPU to user process.
  -- with the newly allocated user stack and system stack.

  function StartUserProcess(arg:int)
    var
       pcb:ptr to ProcessControlBlock
       logAddrSpace:ptr to AddrSpace
       filePtr:ptr to OpenFile
       initPC:int
       InitUserStackTop:int
       initSystemStackTop:ptr to int
       oldIntStat:int

    pcb = processManager.GetANewProcess()
    pcb.myThread = currentThread
    currentThread.myProcess = pcb
    filePtr = fileManager.Open("TestProgram1")
    if(filePtr == null)
      FatalError("Error Opening File")
    endIf
    logAddrSpace = &pcb.addrSpace
    initPC = filePtr.LoadExecutable(logAddrSpace)
    if(initPC == -1 )
      FatalError("Could Not Load executable File in StartUserProcess")
    endIf
    fileManager.Close(filePtr)
    InitUserStackTop = logAddrSpace.numberOfPages*PAGE_SIZE
    initSystemStackTop = & (currentThread.systemStack[SYSTEM_STACK_SIZE-1])
    oldIntStat = SetInterruptsTo(DISABLED)
    logAddrSpace.SetToThisPageTable()
    currentThread.isUserThread = true
    BecomeUserThread(InitUserStackTop,initPC,initSystemStackTop asInteger)
  endFunction
-----------------------------  Handle_Sys_Exit  ---------------------------------

  function Handle_Sys_Exit (returnStatus: int)
      print("Handle_Sys_Exit Invoked!\n")
      print("returnStatus = ")
      printInt(returnStatus)
      nl()
    endFunction

-----------------------------  Handle_Sys_Shutdown  ---------------------------------

  function Handle_Sys_Shutdown ()
      print("Handle_Sys_Shutdown Invoked!\n")
      FatalError("Syscall 'Shutdown' was invoked by a user thread")
    endFunction

-----------------------------  Handle_Sys_Yield  ---------------------------------

  function Handle_Sys_Yield ()
      print("Handle_Sys_Yield Invoked!\n")
    endFunction

-----------------------------  Handle_Sys_Fork  ---------------------------------

  function Handle_Sys_Fork () returns int
      print("Handle_Sys_Fork Invoked!\n")
      return 1000
    endFunction
-----------------------------  Handle_Sys_Join  ---------------------------------

  function Handle_Sys_Join (processID: int) returns int
      print("Handle_Sys_Join Invoked!\n")
      print("processID = ")
      printInt(processID)
      nl()
      return 2000
    endFunction

-----------------------------  Handle_Sys_Exec  ---------------------------------

  function Handle_Sys_Exec (filename: ptr to array of char) returns int
      var
         newAddrSpace:AddrSpace = new AddrSpace
         oldIntStat:int
         fileNameBuffer:array[MAX_STRING_SIZE] of char
         initPC:int
         InitUserStackTop:int
         initSystemStackTop:ptr to int
         file:ptr to OpenFile
         fileStatus:int

      print("Handle_Sys_Exec Invoked!\n")
      newAddrSpace.Init()
      fileStatus = currentThread.myProcess.addrSpace.GetStringFromVirtual(&fileNameBuffer,filename asInteger,MAX_STRING_SIZE)
      if fileStatus < 0
       --print("Cannot copy file name from kernel address space to user process address space in Exec\n")
       return -1 -- Return to calling process cannot read file name from kernel address space.
      endIf
      file = fileManager.Open(&fileNameBuffer)
      if file == null
       --print("Cannot read file contents from executable file in Exec\n")
       return -1
      endIf
      initPC = file.LoadExecutable(&newAddrSpace)
      if initPC < 0
       --print("Cannot load executable content from file in Shandle_Sys_Exec\n")
       return -1
      endIf
      frameManager.ReturnAllFrames(&currentThread.myProcess.addrSpace)
      currentThread.myProcess.addrSpace = newAddrSpace
      InitUserStackTop = newAddrSpace.numberOfPages * PAGE_SIZE
      initSystemStackTop = & currentThread.systemStack[SYSTEM_STACK_SIZE -1]
      oldIntStat = SetInterruptsTo(DISABLED)
      newAddrSpace.SetToThisPageTable()
      currentThread.isUserThread = true
      BecomeUserThread(InitUserStackTop,initPC,initSystemStackTop asInteger)
      return 3000
    endFunction

-----------------------------  Handle_Sys_Create  ---------------------------------

  function Handle_Sys_Create (filename: ptr to array of char) returns int
      var
         fileNameBuffer:array[MAX_STRING_SIZE] of char
         copyStatus:int

      print("Handle_Sys_Create Invoked!\n")
      print("virt addr of filename = ")
      printHex(filename asInteger)
      nl()
      print("filename = ")
      copyStatus = currentThread.myProcess.addrSpace.GetStringFromVirtual(&fileNameBuffer,filename asInteger,MAX_STRING_SIZE)
      if copyStatus < 0
       return -1
      endIf
      print(&fileNameBuffer)
      nl()
      return 4000
    endFunction

-----------------------------  Handle_Sys_Open  ---------------------------------

  function Handle_Sys_Open (filename: ptr to array of char) returns int
      var
         fileNameBuffer:array[MAX_STRING_SIZE] of char
         copyStatus:int

      print("Handle_Sys_Open Invoked!\n")
      print("virt addr of filename = ")
      printHex(filename asInteger)
      nl()
      print("filename = ")
      copyStatus = currentThread.myProcess.addrSpace.GetStringFromVirtual(&fileNameBuffer,filename asInteger,MAX_STRING_SIZE)
      if copyStatus < 0
       return -1
      endIf
      print(&fileNameBuffer)
      nl()
      return 5000
    endFunction

-----------------------------  Handle_Sys_Read  ---------------------------------

  function Handle_Sys_Read (fileDesc: int, buffer: ptr to char, sizeInBytes: int) returns int
      print("Handle_Sys_Read Invoked!\n")
      print("fileDesc = ")
      printInt(fileDesc)
      nl()
      print("virt addr of buffer = ")
      printHex(buffer asInteger)
      nl()
      print("sizeInBytes = ")
      printInt(sizeInBytes)
      nl()
      return 6000
    endFunction

-----------------------------  Handle_Sys_Write  ---------------------------------

  function Handle_Sys_Write (fileDesc: int, buffer: ptr to char, sizeInBytes: int) returns int
      print("Handle_Sys_Write Invoked!\n")
      print("fileDesc = ")
      printInt(fileDesc)
      nl()
      print("virt addr of buffer = ")
      printHex(buffer asInteger)
      nl()
      print("sizeInBytes = ")
      printInt(sizeInBytes)
      nl()
      return 7000
    endFunction

-----------------------------  Handle_Sys_Seek  ---------------------------------

  function Handle_Sys_Seek (fileDesc: int, newCurrentPos: int) returns int
      print("Handle_Sys_Seek Invoked!\n")
      print("fileDesc = ")
      printInt(fileDesc)
      nl()
      print("newCurrentPos = ")
      printInt(newCurrentPos)
      nl()
      return 8000
    endFunction

-----------------------------  Handle_Sys_Close  ---------------------------------

  function Handle_Sys_Close (fileDesc: int)
      print("Handle_Sys_Close Invoked!\n")
      print("fileDesc = ")
      printInt(fileDesc)
      nl()
    endFunction


endCode