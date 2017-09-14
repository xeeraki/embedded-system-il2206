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
      Data : Integer;
   begin
      Next := Clock;
      for I in 1..N loop
         -- Write to X
        -- X := I;
         -- Next 'Release' in 50..250ms
         Data := Random(G);
         circularBufferObj.Put(Data);
         Put_Line("Producer : Adding "&Data'Img);
         Next := Next + Milliseconds(Data);
         delay until Next;
      end loop;
   end;

   task body Consumer is
      Next : Time;
      Data : Integer;
   begin
      Next := Clock;
      for I in 1..N loop
         -- Read from X
         Data := Random(G);
         circularBufferObj.Get(Data);
         Put_Line("Consumer: Taking"&Data'Img);
         Next := Next + Milliseconds(Data);
         delay until Next;
      end loop;
   end;

begin -- main task
   null;
end producerconsumer_port;

