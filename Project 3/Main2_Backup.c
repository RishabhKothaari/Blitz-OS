code Main

  -- OS Class: Project 3
  --
  -- RISHAB
  --

-----------------------------  Main  ---------------------------------

  function main ()
      print ("Example Thread-based Programs...\n")

      InitializeScheduler ()

      -----  Uncomment any one of the following to perform the desired test  -----

      sleepingBarber()

      --gameParlor()

      ThreadFinish ()

    endFunction

-----------------------------------------------------------------------------------
    class BarberShop
      superclass Object
      fields
        noOfChairs:int = 5
var
    noOfChairs:int = 5
    customers:Semaphore = new Semaphore
    barber:Semaphore = new Semaphore
    accessibleSeats:Semaphore = new Semaphore
    barberThread:Thread = new Thread
    customerThread:array [20] of Thread = new array of Thread {20 of new Thread}
    printLock:Mutex = new Mutex

    function sleepingBarber()

      customers.Init(0)
      barber.Init(0)
      accessibleSeats.Init(1)
      printLock.Init()

      --Initialize shop to simulate.

      print("         Barber          1     2     3     4     5     6     7     8     9     10     11     12     13     14     15     16     17     18     19     20\n")
      --printSpaces(0,"")

      barberThread.Init("       ")
      barberThread.Fork(simulateBarber,1)

      customerThread[0].Init("                     ")
      customerThread[0].Fork(customer,1)
      customerThread[1].Init("                           ")
      customerThread[1].Fork(customer,2)
      customerThread[2].Init("                                 ")
      customerThread[2].Fork(customer,3)
      customerThread[3].Init("                                       ")
      customerThread[3].Fork(customer,4)
      customerThread[4].Init("                                             ")
      customerThread[4].Fork(customer,5)
      customerThread[5].Init("                                                   ")
      customerThread[5].Fork(customer,6)
      customerThread[6].Init("                                                        ")
      customerThread[6].Fork(customer,7)
      customerThread[7].Init("                                                             ")
      customerThread[7].Fork(customer,8)
      customerThread[8].Init("                                                                     ")
      customerThread[8].Fork(customer,9)
      customerThread[9].Init("                                                                            ")
      customerThread[9].Fork(customer,10)
      customerThread[10].Init("                                                                                 ")
      customerThread[10].Fork(customer,12)
      customerThread[11].Init("                                                                                        ")
      customerThread[11].Fork(customer,12)
      customerThread[12].Init("                                                                                              ")
      customerThread[12].Fork(customer,13)
      customerThread[13].Init("                                                                                                    ")
      customerThread[13].Fork(customer,14)
      customerThread[14].Init("                                                                                                          ")
      customerThread[14].Fork(customer,15)
      customerThread[15].Init("                                                                                                                 ")
      customerThread[15].Fork(customer,16)
      customerThread[16].Init("                                                                                                                        ")
      customerThread[16].Fork(customer,17)
      customerThread[17].Init("                                                                                                                               ")
      customerThread[17].Fork(customer,18)
      customerThread[18].Init("                                                                                                                                        ")
      customerThread[18].Fork(customer,19)
      customerThread[19].Init("                                                                                                                                                 ")
      customerThread[19].Fork(customer,20)

    endFunction

    function simulateBarber(b:int)
      while true
        customers.Down()
        accessibleSeats.Down()
        noOfChairs = noOfChairs + 1
        barber.Up()
        accessibleSeats.Up()
        cutHair()
      endWhile
    endFunction

    function customer(cust:int)
      accessibleSeats.Down()
      if noOfChairs > 0
        noOfChairs = noOfChairs - 1
        printSpaces(cust,"E")
        customers.Up()
        printSpaces(cust,"S")
        accessibleSeats.Up()
        barber.Down()
        getHairCut(cust)
      else
        printSpaces(cust,"L")
        accessibleSeats.Up()
      endIf
    endFunction

    function getHairCut(cust:int)
      var i:int
      accessibleSeats.Down()
      printSpaces(cust,"B")
      for i = 1 to 1000
       currentThread.Yield()
      endFor
      printSpaces(cust,"F")
      printSpaces(cust,"L")
      accessibleSeats.Up()
    endFunction

    function printSpaces(cust:int,state:ptr to array of char)
      var
         i:int
         a:int
     printLock.Lock()
     for i = 1 to  5 - noOfChairs
        print("X")
        a = a + 1
     endFor

     for i = 1 to 5 - a
       print("-")
     endFor

     print(currentThread.name)
     print(state)
     nl()
     printLock.Unlock()
    endFunction

    function cutHair()
      var
         i:int
      printSpaces(0,"start")
      for i = 1 to 1000
       currentThread.Yield()
      endFor
      printSpaces(0,"end")
    endFunction


------------------------------------------------------------------------------------
var
  parlor: GameParlor
  availableDice: int = 8
  gameGroup: array [8] of Thread = new array of Thread {8 of new Thread}

  function gameParlor()

  parlor = new GameParlor

-- Create a loop for groups to request and play game.

--A – Backgammon(4)
--B – Backgammon(4)
--C – Risk(5)
--D – Risk(5)
--E – Monopoly(2)
--F – Monopoly(2)
--G – Pictionary(1)
--H – Pictionary(1)

  parlor.Init()

  gameGroup[0].Init("A")
  gameGroup[0].Fork(simulateGame,4)

  gameGroup[1].Init("B")
  gameGroup[1].Fork(simulateGame,4)

  gameGroup[2].Init("C")
  gameGroup[2].Fork(simulateGame,5)

  gameGroup[3].Init("D")
  gameGroup[3].Fork(simulateGame,5)

  gameGroup[4].Init("E")
  gameGroup[4].Fork(simulateGame,2)

  gameGroup[5].Init("F")
  gameGroup[5].Fork(simulateGame,2)

  gameGroup[6].Init("G")
  gameGroup[6].Fork(simulateGame,1)

  gameGroup[7].Init("H")
  gameGroup[7].Fork(simulateGame,1)

endFunction

-- Request for dice, yield and return dice after print.
function simulateGame(dice:int)

  var
     i:int

  for i=1 to 5
  --Requesting dice
  parlor.Request(dice)


  --Yield to simulate
  currentThread.Yield()

  --Return dice
  parlor.Return(dice)

  --Yield so that other groups play
  currentThread.Yield()
  endFor

endFunction

class GameParlor
  superclass Object
  fields
    parlorCondition:Condition
    waitingCondition:Condition
    parlorMutex:Mutex
    waitingList:int
  methods
    Init()
    Request(dice:int)
    Return(dice:int)
    Print(str:String,count:int)
endClass

behavior GameParlor

  method Init()
    parlorMutex = new Mutex
    parlorMutex.Init()
    parlorCondition = new Condition
    waitingCondition = new Condition
    parlorCondition.Init()
    waitingCondition.Init()
    waitingList = 0
  endMethod

  method Request(dice:int)
    if !parlorMutex.IsHeldByCurrentThread()
      parlorMutex.Lock()
    endIf

    self.Print("requests",dice)

    waitingList = waitingList + 1

    if waitingList > 1
     parlorCondition.Wait(&parlorMutex)
    endIf

    while dice > availableDice
      waitingCondition.Wait(&parlorMutex)
    endWhile

    availableDice = availableDice - dice
    waitingList = waitingList - 1
    parlorCondition.Signal(&parlorMutex)
    self.Print("proceeds with",dice)

    if parlorMutex.IsHeldByCurrentThread()
     parlorMutex.Unlock()
    endIf
  endMethod

  method Return(dice:int)
    if !parlorMutex.IsHeldByCurrentThread()
      parlorMutex.Lock()
    endIf
      availableDice = availableDice + dice
      waitingCondition.Signal(&parlorMutex)
      self.Print("releases and adds back", dice)
    if parlorMutex.IsHeldByCurrentThread()
      parlorMutex.Unlock()
    endIf
  endMethod

  method Print (str: String, count: int)

  -- This method prints the current thread's name and the arguments.
  -- It also prints the current number of dice available.
  --
  print (currentThread.name)
  print (" ")
  print (str)
  print (" ")
  printInt (count)
  nl ()
  print ("------------------------------Number of dice now avail = ")
  printInt (availableDice)
  nl ()
  endMethod

endBehavior
endCode
