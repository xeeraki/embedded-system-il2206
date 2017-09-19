with Ada.Text_IO;
use Ada.Text_IO;

with Ada.Real_Time;
use Ada.Real_Time;

with Ada.Numerics.Discrete_Random;

procedure producerconsumer_rndvzs is

   --X : Integer; -- Shared Variable
   N : constant Integer := 40; -- Number of produced and comsumed variables
   
   --pragma Volatile(X); -- For a volatile object all reads and updates of
                       -- the object as a whole are performed directly
                       -- to memory (Ada Reference Manual, C.6)

   -- Random Delays
   subtype Delay_Interval is Integer range 50..250;
   package Random_Delay is new Ada.Numerics.Discrete_Random (Delay_Interval);
   use Random_Delay;
   G : Generator;

   task Producer;

   task Consumer;
   
   task circularbuffer is
      entry Put(value : in Integer);
      entry Get(value : out Integer);
   end circularbuffer;

   task body Producer is
      Next : Time;
      Data : Integer;
      
   begin
      Next := Clock;
      for I in 1..N loop
         -- Write to X
         -- Next 'Release' in 50..250ms
         Data := Random(G);
         --Put(Data);

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
         --Get(Data);
         Put_Line("Consumer: Taking"&Data'Img);
         Next := Next + Milliseconds(Data);
         delay until Next;
      end loop;
   end;
   
   task body circularbuffer is
   
   subtype Item is Integer;
   type Index is mod N;
   type Item_Array is array(Index) of Item;
      A: Item_Array;
      In_Ptr, Out_Ptr: Index := 0;
      Count: Integer range 0..N := 0;
   begin
   loop
   select
   
   when Count < N =>
   accept Put(value : in Integer) do
   A(In_Ptr):= value;
   end Put;
   In_Ptr:= In_Ptr+1;
   Count:= Count+1;
   
   or
   
   when Count > 0 =>
   accept Get(value : out Integer) do
   value:=A(Out_Ptr);
   end Get;
   Out_Ptr:= Out_Ptr+1;
   Count:= Count+1;
   
   end select;
   end loop;
   end circularbuffer;

begin -- main task
   null;
end producerconsumer_rndvzs;

