header Main

  uses System, Thread, Synch

  functions
    main ()

  --The barber shop class that simulates a barber shop.

    class BarberShop
      superclass Object

      fields
        noOfChairs:int
        customers:Semaphore
        barber:Semaphore
        accessibleSeats:Semaphore

      methods
        Init()
        simulateBarber()
        customer(cust:int)
        printSpaces(cust:int,state:ptr to array of char)
        cutHair()
        getHairCut(cust:int)
    endClass

endHeader
