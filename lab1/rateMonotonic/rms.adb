pragma Task_Dispatching_Policy(FIFO_Within_Priorities);

with Ada.Text_IO; use Ada.Text_IO;
with Ada.Real_Time; use Ada.Real_Time;

procedure rms is

   package Duration_IO is new Ada.Text_IO.Fixed_IO(Duration);
   package Int_IO is new Ada.Text_IO.Integer_IO(Integer);
    
   Start : Time;
   StartPgm : Time;
   Dummy : Integer;
   Index : Integer := 20;  -- Index for which the execution time is 1 sec
    
   function F(N : Integer) return Integer;

   function F(N : Integer) return Integer is -- Dummy Function
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
         Dummy := F(Index); -- Call the dummy functiom

         Put("Execution Time");
         Put(" : ");
         Duration_IO.Put(To_Duration(Clock - Start), 3, 3); -- execution time of task
         Put_Line("s");
         Duration_IO.Put(To_Duration(Clock - StartPgm), 3, 3);-- execution time of program
         Put(" : ");
         Int_IO.Put(Id, 2);
         Put_Line("");
         delay until Next;
      end loop;
   end T;

   -- Example Task
   Task_P10 : T(20, 3000); -- Task with priority 20 and period 3000, execution time 1 sec
   Task_P12 : T(15, 4000); -- Task with priority 15 and period 4000, execution time 1 sec  
   Task_P14 : T(10, 6000); -- Task with priority 10 and period 6000, execution time 1 sec
   
begin
      StartPgm := Clock; -- Start of program
end rms;
