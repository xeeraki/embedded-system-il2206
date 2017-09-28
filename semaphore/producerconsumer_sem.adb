with Ada.Text_IO;
use Ada.Text_IO;

with Ada.Real_Time;
use Ada.Real_Time;

with Ada.Numerics.Discrete_Random;

with Semaphores;
use Semaphores;

procedure ProducerConsumer_sem is

   X : Integer; -- Shared Variable
   N : constant Integer := 40; -- Number of produced and comsumed variables
   --binary semapgore (object created)
   S: CountingSemaphore(1,0);
   --S1: CountingSemaphore(1,1);
   
   pragma Volatile(X); -- For a volatile object all reads and updates of
                       -- the object as a whole are performed directly
                       -- to memory (Ada Reference Manual, C.6)

   --Random Delays
   subtype Delay_Interval is Integer range 50..250;
   package Random_Delay is new Ada.Numerics.Discrete_Random
   (Delay_Interval);
   use Random_Delay;
   G : Generator;

   task Producer;

   task Consumer;

   task body Producer is
      Next : Time;
   begin
      Next := Clock;
      for I in 1..N loop
         -- Write to X
         --S.Wait;
         -- X is a shared variable defined i
         X := I;
         --signal that the producer placed item into the X
         S.Signal;
         --Next 'Release' in 50..250ms
         Next := Next + Milliseconds(Random(G));
         Put("Produced");
         Put_Line(Integer'Image(X));
         delay until Next;
      end loop;
   end;

   task body Consumer is
      Next : Time;
      Y:Integer;
   begin
      Next := Clock;
      for I in 1..N loop
         -- Read from X 
         --wait until there is an item in the buffer X
         S.Wait;
         --readed value from X assigned to Y
         Y:=X; 
         --S.Signal; 
         --print out the value readed from the buffer X 
         Put("Comsumed");
         Put_Line(Integer'Image(Y));
         Next := Next + Milliseconds(Random(G));
         delay until Next;
      end loop;
   end;

begin -- main task


   null;
end ProducerConsumer_sem;
