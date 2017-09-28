with Ada.Text_IO;
use Ada.Text_IO;

with Ada.Real_Time;
use Ada.Real_Time;

with Ada.Numerics.Discrete_Random;

with buffer;
use buffer;

procedure producerconsumer_port is

   X : Integer; -- Shared Variable
   N : constant Integer := 40; -- Number of produced and comsumed variables
   
   circularBufferObj : CircularBuffer;

   pragma Volatile(X); -- For a volatile object all reads and updates of
                       -- the object as a whole are performed directly
                       -- to memory (Ada Reference Manual, C.6)

   -- Random Delays
   subtype Delay_Interval is Integer range 50..250;
   package Random_Delay is new Ada.Numerics.Discrete_Random (Delay_Interval);
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
         -- X is a global shared variable of type int declared above
          X := I;
         -- Next 'Release' in 50..250ms
         Put("Produced");
         Put_Line(Integer'Image(X));
         circularBufferObj.Put(X);
         Next := Next + Milliseconds(Random(G));
         delay until Next;
      end loop;
   end;

   task body Consumer is
      Next : Time;
   begin
      Next := Clock;
      for I in 1..N loop
         -- Read from X
         
         --we don't have to declare new variable here 
         --since X is declared globaly and is a shared variable between
         --producer and consumer
         circularBufferObj.Get(X);
         Put("Consumed");
         Put_Line(Integer'Image(X));
         Next := Next + Milliseconds(Random(G));
         delay until Next;
      end loop;
   end;

begin -- main task
   null;
end producerconsumer_port;

