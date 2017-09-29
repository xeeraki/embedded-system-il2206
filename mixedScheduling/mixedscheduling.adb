pragma Priority_Specific_Dispatching(
FIFO_Within_Priorities, 20, 30);      -- FIFO has highest priority
pragma Priority_Specific_Dispatching(
Round_Robin_Within_Priorities, 1, 10); -- Round Robin has lowest priority


with Ada.Text_IO; use Ada.Text_IO;
with Ada.Real_Time; use Ada.Real_Time;

procedure mixedscheduling is

   package Duration_IO is new Ada.Text_IO.Fixed_IO(Duration);
   package Int_IO is new Ada.Text_IO.Integer_IO(Integer);
    
   Start : Time;
   StartPgm : Time;
   Dummy : Integer;
   Index : Integer := 20;  -- Number for which the execution time is 1 sec in Function F
    
   function F(N : Integer) return Integer;

   function F(N : Integer) return Integer is -- Dummy function with execution time 1 sec
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
   begin
      Next := StartPgm;
      loop
         Next := Next + Milliseconds(Period);
         Start := Clock;
         Dummy := F(Index);
         Put("Execution Time");
         Put(" : ");
         Duration_IO.Put(To_Duration(Clock - Start), 3, 3); -- Execution Time of that particular task
         Put_Line("s");
         Duration_IO.Put(To_Duration(Clock - StartPgm), 3, 3); -- Overall time elapsed from start of program
         Put(" : ");
         Int_IO.Put(Id, 2);
         Put_Line("");
         delay until Next; -- Delay until next Period
      end loop;
   end T;

   -- Task with FIFO_Within_Priority Scheduling (High Priority)
   Task_P1 : T(30, 3000); 
   Task_P2 : T(25, 4000);
   Task_P3 : T(15, 6000);
   
   -- Task with Round Robin Scheduling (Low priority)
   Task_P4 : T(2,9000);
   Task_P5 : T(4,9000);
   Task_P6 : T(6,9000);
      
begin
      StartPgm := Clock; -- Start of Program
end mixedscheduling;
