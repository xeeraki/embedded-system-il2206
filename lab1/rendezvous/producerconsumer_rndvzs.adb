with Ada.Text_IO;
use Ada.Text_IO;

with Ada.Real_Time;
use Ada.Real_Time;

with Ada.Numerics.Discrete_Random;

procedure producerconsumer_rndvzs is

   N : constant Integer := 40; -- Number of produced and comsumed variables
  
   -- Random Delays
   subtype Delay_Interval is Integer range 50..250;
   package Random_Delay is new Ada.Numerics.Discrete_Random (Delay_Interval);
   use Random_Delay;
   G : Generator;

   task Producer;

   task Consumer;
   
   task circularbuffer is  -- rendezvous
      entry Put(value : in Integer);
      entry Get(value : out Integer);
   end circularbuffer;

   task body Producer is
      Next : Time;
      Data : Integer;
      
   begin
      Next := Clock;
      for I in 1..N loop
         Data := Random(G); -- Random number is generated as data
         circularbuffer.Put(Data); -- Data put into buffer 
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
         circularbuffer.Get(Data); -- data taken from buffer
         Next := Next + Milliseconds(Data);
         delay until Next;
      end loop;
   end;
   
   task body circularbuffer is
   
   subtype Item is Integer;
   type Index is mod N;
   type Item_Array is array(Index) of Item;
      A: Item_Array; -- buffer to store data
      In_Ptr, Out_Ptr: Index := 0; -- In and Out pointers to Data
      Count: Integer range 0..N := 0; -- Number of elements stored in bufer
   begin
   loop
   select
   
   when Count < N =>  -- add data when buffer is not full
   accept Put(value : in Integer) do
   A(In_Ptr):= value;
   Put_Line("Producer: Putting"&value'Img);
   end Put;
   In_Ptr:= In_Ptr+1;
   Count:= Count+1;
   
   or
   
   when Count > 0 => -- take data when buffer is not empty
   accept Get(value : out Integer) do
   value:=A(Out_Ptr);
   Put_Line("Consumer: Taking"&value'Img);
   end Get;
   Out_Ptr:= Out_Ptr+1;
   Count:= Count-1;
   
   end select;
   end loop;
   end circularbuffer;

begin -- main task
   null;
end producerconsumer_rndvzs;

