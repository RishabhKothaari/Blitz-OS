code Main

  -- OS Class: Project 3
  --
  -- RISHAB I KOTHARI
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

-- Behavior barber shop. This behavior makes use of class BaberShop from Main.h .And employs its methods to
-- simulate a barber shop.

    behavior BarberShop

-- Init method initializes barber shop with noOfChairs available in shop at any time ,barber Semaphore - Indicates barber is busy or not.
-- customer Semaphore - Used to keep track of customers who get in the shop.

     method Init()
        customers = new Semaphore
        noOfChairs = 5
        barber = new Semaphore
        accessibleSeats = new Semaphore
        customers.Init(0)
        barber.Init(0)
        accessibleSeats.Init(1)
    endMethod

-- Method simulateBarber - simulates a barber who is always looking for customers.If there are no customers then he goes to sleep.

    method simulateBarber()
      while true
        customers.Down()
        accessibleSeats.Down()
        noOfChairs = noOfChairs + 1
        barber.Up()
        accessibleSeats.Up()
        self.cutHair()
      endWhile
    endMethod

-- Mehod customer simulates a customer if there are chairs available in shop then get in ("E"),sit ("S") and wake the barber if he's sleeping and get hair cut done.
-- If there are not chirs vailable then leave the shop ("L")

    method customer(cust:int)
      accessibleSeats.Down()
      if noOfChairs > 0
        noOfChairs = noOfChairs - 1
        self.printSpaces(cust,"E")
        customers.Up()
        self.printSpaces(cust,"S")
        accessibleSeats.Up()
        barber.Down()
        self.getHairCut(cust)
      else
        self.printSpaces(cust,"L")
        accessibleSeats.Up()
      endIf
    endMethod

-- Method which simulates the process of customer getting hair cut done. ("B") - Begin hair cut,("F") - Finish hair cut and ("L") - Leave hair cut done.

    method getHairCut(cust:int)
      var i:int
      self.printSpaces(cust,"B")
      for i = 1 to 1000
       currentThread.Yield()
      endFor
      self.printSpaces(cust,"F")
      self.printSpaces(cust,"L")
    endMethod

-- Method used by threads in the shop to print the current state of the shop.("X") - Indicates filled chairs and ("-") - Indicates available chairs

    method printSpaces(cust:int,state:ptr to array of char)
      var
         i:int
         a:int
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
    endMethod

-- Method that barber uses to simulate hair cutting process.("start") - Indicates start cutting hair and ("end") - end indicates end hair cut.

    method cutHair()
      var
         i:int
      self.printSpaces(0,"start")
      for i = 1 to 1000
       currentThread.Yield()
      endFor
      self.printSpaces(0,"end")
    endMethod

endBehavior
----------------------------------------------------------------------------------------

-- Initialize threads and create an instance of BarberShop

var
   barberThread:Thread = new Thread
   customerThread:array [20] of Thread = new array of Thread {20 of new Thread}
   barberShop:BarberShop = new BarberShop

-- Start the problem of sleepingBarber

function sleepingBarber()
    barberShop.Init()
    print("         Barber          1     2     3     4     5     6     7     8     9     10     11     12     13     14     15     16     17     18     19     20\n")
    barberThread.Init("       ")
    barberThread.Fork(simulateBarber,1)
    simulateCustomers()
endFunction

-- Simulate the customer threads.

function simulateCustomers()

customerThread[0].Init("                     ")
customerThread[0].Fork(simulateCustomer,1)
customerThread[1].Init("                           ")
customerThread[1].Fork(simulateCustomer,2)
customerThread[2].Init("                                 ")
customerThread[2].Fork(simulateCustomer,3)
customerThread[3].Init("                                       ")
customerThread[3].Fork(simulateCustomer,4)
customerThread[4].Init("                                             ")
customerThread[4].Fork(simulateCustomer,5)
customerThread[5].Init("                                                   ")
customerThread[5].Fork(simulateCustomer,6)
customerThread[6].Init("                                                        ")
customerThread[6].Fork(simulateCustomer,7)
customerThread[7].Init("                                                             ")
customerThread[7].Fork(simulateCustomer,8)
customerThread[8].Init("                                                                     ")
customerThread[8].Fork(simulateCustomer,9)
customerThread[9].Init("                                                                            ")
customerThread[9].Fork(simulateCustomer,10)
customerThread[10].Init("                                                                                 ")
customerThread[10].Fork(simulateCustomer,12)
customerThread[11].Init("                                                                                        ")
customerThread[11].Fork(simulateCustomer,12)
customerThread[12].Init("                                                                                              ")
customerThread[12].Fork(simulateCustomer,13)
customerThread[13].Init("                                                                                                    ")
customerThread[13].Fork(simulateCustomer,14)
customerThread[14].Init("                                                                                                          ")
customerThread[14].Fork(simulateCustomer,15)
customerThread[15].Init("                                                                                                                 ")
customerThread[15].Fork(simulateCustomer,16)
customerThread[16].Init("                                                                                                                        ")
customerThread[16].Fork(simulateCustomer,17)
customerThread[17].Init("                                                                                                                               ")
customerThread[17].Fork(simulateCustomer,18)
customerThread[18].Init("                                                                                                                                        ")
customerThread[18].Fork(simulateCustomer,19)
customerThread[19].Init("                                                                                                                                                 ")
customerThread[19].Fork(simulateCustomer,20)

endFunction

-- Each customer thread tries to get a hair cut 5 times.

function simulateCustomer(cust:int)
  var i:int
  for i = 1 to 5
    barberShop.customer(cust)
  endFor
  currentThread.Yield()
endFunction

-- Initialize and allocate barber in the barber shop.

function simulateBarber()
  barberShop.simulateBarber()
endFunction

---------------------------------GAME PARLOR--------------------------------------------------

-- Initialize resources to simulate a gaming parlor.
-- parlor is the instance of game parlor.availableDice - number of dice available in parlor
-- gameGroup - threads that simulate game groups.

var
  parlor: GameParlor
  availableDice: int = 8
  gameGroup: array [8] of Thread = new array of Thread {8 of new Thread}

-- Start the simulation of parlor.
  
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

-- Simulate a game request for dice, yield and return dice after print.

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

-- GameParlor class - parlor condition - condition variable to queue the groups,
-- waiting condition - condition variable to queue customers to server on first cum first server basis.

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

-- Simulate request for dice by a game group.Keep track of groups who are already waiting for dice
-- that they can be given turn - even though there are groups which require lower number of dice to
-- play the game.  

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

-- Simulate return of dice by a group.  

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
