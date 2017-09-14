package body buffer is

task body circularbuffer is
   
   private
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
   accept Get(value : in Integer) do
   value:=A(Out_Ptr);
   end Get;
   Out_Ptr:= Out_Ptr+1;
   Count:= Count+1;
   
   end select;
   end loop;
   end;
end circularbuffer;
end buffer;
