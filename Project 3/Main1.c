code Main

  -- OS Class: Project 2
  --
  -- <RISHAB>
  --
  -- This package contains the following:
  --     SimpleThreadExample
  --     MoreThreadExamples
  --     ProducerConsumer
  --     TestMutex
  --     Dining Philospohers



-----------------------------  SynchTest  ---------------------------------

  function main ()
      print ("Example Thread-based Programs...\n")

      InitializeScheduler ()

      -----  Uncomment any one of the following to perform the desired test  -----

      sleepingBarber()

      ThreadFinish ()

    endFunction

-----------------------------------------------------------------------------------

var
    noOfChairs:int = 5
    customers:Semaphore = new Semaphore
    barber:Semaphore = new Semaphore
    waitingList:int = 0
    shopMutex:Mutex = new Mutex
    barberThread:Thread = new Thread
    customerThread:array [6] of Thread = new array of Thread {5 of new Thread}

    function sleepingBarber()
      customers.Init(0)
      barber.Init(0)
      shopMutex.Init()

      --Initialize shop to simulate.

      print("                    Barber     1         2         3       4     5\n")

      barberThread.Init("         ")
      barberThread.Fork(barber,1)

      customerThread[0].Init("C1")
      customerThread[0].Fork(customer,1)
      customerThread[1].Init("C2")
      customerThread[1].Fork(customer,2)
      customerThread[2].Init("C3")
      customerThread[2].Fork(customer,3)
      customerThread[3].Init("C4")
      customerThread[3].Fork(customer,4)
      customerThread[4].Init("C5")
      customerThread[4].Fork(customer,5)
      customerThread[5].Init("C6")
      customerThread[5].Fork(customer,6)

    endFunction

    function barber(b:int)
      while true
        customers.Down()
        shopMutex.Lock()
        waitingList = waitingList - 1
        barber.Up()
        shopMutex.Unlock()
        self.cutHair()
      endWhile
    endFunction

    function customer(cust:int)
      shopMutex.Lock()
      if waitingList < noOfChairs
        waitingList = waitingList + 1
        self.getHairCut(cust)
      else
        print("Shop is full I am leaving\n")
        print(currentThread.name)
        shopMutex.Unlock()  
      endIf
    endFunction

    function getHairCut(cust:int)
      print("I am getting hair cut done\n")
      print(currentThread.name)
      customers.Up()
      shopMutex.Unlock()
      barber.Down()
      self.cutHair()
      print("I am finished getting hair cut done\n")
      print(currentThread.name)
    endFunction

    function cutHair()
      var
         i:int
         print("starting hair cut for\n")
         print(currentThread.name)
      for i = 1 to 10
       currentThread.Yield()
      endFor
        print("finishing hair cut for\n")
        print(currentThread.name)
    endFunction

endBehavior
endCode