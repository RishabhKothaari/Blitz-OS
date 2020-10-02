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

var
    noOfChairs:int = 4
    customers:Semaphore = new Semaphore
    barber:Semaphore = new Semaphore
    accessibleSeats:Semaphore = new Semaphore
    barberThread:Thread = new Thread
    customerThread:array [6] of Thread = new array of Thread {6 of new Thread}

    function sleepingBarber()
      customers.Init(0)
      barber.Init(0)
      accessibleSeats.Init(1)

      --Initialize shop to simulate.

      print("            Barber     1     2     3     4     5\n")
      printChairs()

      barberThread.Init("Barber")
      barberThread.Fork(simulateBarber,1)

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

    function simulateBarber(b:int)
      while true
        customers.Down()
        accessibleSeats.Down()
        noOfChairs = noOfChairs + 1
        barber.Up()
        accessibleSeats.Up()
        print("start hair cut             ")
        nl()
        cutHair()
      endWhile
    endFunction

    function customer(cust:int)
      accessibleSeats.Down()
      if noOfChairs > 0
        noOfChairs = noOfChairs - 1
        print("E")
        printInt(cust)
        nl()
        customers.Up()
        print("S")
        printInt(cust)
        nl()
        accessibleSeats.Up()
        barber.Down()
        getHairCut(cust)
      else
        print("L")
        printInt(cust)
        nl()
        accessibleSeats.Up()
      endIf
    endFunction

    function getHairCut(cust:int)
      var i:int
      accessibleSeats.Down()
      print("B")
      printInt(cust)
      nl()
      --customers.Down()
      --barber.Up()
      for i = 1 to 100
       currentThread.Yield()
      endFor
      --print("I am")
      --printInt(cust)
      --nl()
      --accessibleSeats.Up()
      --cutHair()
      print("F")
      printInt(cust)
      nl()
      print("End")
      nl()
      print("L")
      printInt(cust)
      nl()
      accessibleSeats.Up()
      --customers.Up()
      --barber.Up()
    endFunction

    function printChairs()
      var
         i:int
     for i = 0 to noOfChairs
        print("-")
     endFor
    endFunction

    function printSpaces(cust:int)
      var
         i:int
     for i = 0 to cust*5
       print(" ")
     endFor
    endFunction

    function cutHair()
      var
         i:int
      for i = 1 to 100
       currentThread.Yield()
      endFor
      --print("Finish hair cut                                         ")
      --nl()
      --print("End hair cut                 \n")
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
