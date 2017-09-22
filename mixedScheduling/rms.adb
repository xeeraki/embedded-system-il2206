pragma Priority_Specific_Dispatching(
FIFO_Within_Priorities, 20, 30);
pragma Priority_Specific_Dispatching(
Round_Robin_Within_Priorities, 1, 10);


with Ada.Text_IO; use Ada.Text_IO;
with Ada.Real_Time; use Ada.Real_Time;

procedure mixedscheduling is

   package Duration_IO is new Ada.Text_IO.Fixed_IO(Duration);
   package Int_IO is new Ada.Text_IO.Integer_IO(Integer);
    
   Start : Time;
   StartPgm : Time;
   Dummy : Integer;
   Index : Integer := 20;
    
   function F(N : Integer) return Integer;

   function F(N : Integer) return Integer is
      X : Integer := 0;
   begin
      for Index in 1..N loop
         for I in 1..5000000 loop
            X := I;
         end loop;
      end loop;
      return X;
   end F;
   
   task type T(Id: Integer; Period : Integer) is
      pragma Priority(Id);
   end;
   
   task body T is
      Next : Time;
 --     X : Integer;
   begin
      Next := StartPgm;
      loop
         Next := Next + Milliseconds(Period);
         -- Some dummy function
         Start := Clock;
         Dummy := F(Index);
--        Int_IO.Put(Index, 3);
         Put("Execution Time");
         Put(" : ");
         Duration_IO.Put(To_Duration(Clock - Start), 3, 3);
         Put_Line("s");
         Duration_IO.Put(To_Duration(Clock - StartPgm), 3, 3);
         Put(" : ");
         Int_IO.Put(Id, 2);
         Put_Line("");
         delay until Next;
      end loop;
   end T;

   -- Example Task
   Task_P1 : T(30, 3000);
   Task_P2 : T(25, 4000);
   Task_P3 : T(15, 6000);
   Task_P4 : T(2,9000);
   Task_P5 : T(4,9000);
   Task_P6 : T(6,9000);
      
begin

  -- for Index in 1..50 loop
      StartPgm := Clock;

  -- end loop;
end mixedscheduling;
