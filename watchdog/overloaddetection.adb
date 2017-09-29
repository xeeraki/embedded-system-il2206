pragma Task_Dispatching_Policy(FIFO_Within_Priorities);

with Ada.Text_IO; use Ada.Text_IO;
with Ada.Real_Time; use Ada.Real_Time;

with Ada.Numerics.Discrete_Random;

procedure overloaddetection is
   
   package Duration_IO is new Ada.Text_IO.Fixed_IO(Duration);
   package Int_IO is new Ada.Text_IO.Integer_IO(Integer);
    
   Start : Time;
   StartPgm : Time;
   Dummy : Integer;
   HyperPeriod : Integer := 36000; -- for rms2, hyperperiod is 36000
    
   function F(N : Integer) return Integer;

   function F(N : Integer) return Integer is -- a dummy function
      X : Integer := 0;
   begin
      for Index in 1..N loop
         for I in 1..5000000 loop
            X := I;
         end loop;
      end loop;
      return X;
   end F;
   
   task type T(Id: Integer; Period : Integer ; ExecTime : Integer) is
      pragma Priority(Id);
   end;
   
   task overload is   -- Overload Detection Task with the least priority
      pragma Priority(1);
   end;
   
   task watchdog is -- Watchdog timer task with the highest priority
      pragma Priority(30);
      entry resetTimer;
   end;
   
   task body T is
      Next : Time;
   begin
      Next := StartPgm;
      
      loop
         Next := Next + Milliseconds(Period);
        
         Start := Clock;
         Dummy := F(ExecTime); -- Calling dummy function
         Put("Execution Time");
         Put(" : ");
         Duration_IO.Put(To_Duration(Clock - Start), 3, 3); -- executiomn time of the task
         Put_Line("s");
         Duration_IO.Put(To_Duration(Clock - StartPgm), 3, 3); -- execution times of the program
         Put(" : ");
         Int_IO.Put(Id, 2);
         Put_Line("");
         delay until Next;
      end loop;
   end T;
   
   task body overload is
   Next : Time;
   begin
   Next := Clock;
   loop
   Next := Next + Milliseconds(HyperPeriod);
   watchdog.resetTimer; -- reset the watchdog timer when there is no overload
   delay until Next;
   end loop;
   end overload;
   
   task body watchdog is 
   begin
   loop
   select
   
   accept resetTimer; -- reset the watchdog timer

   or 

   delay 12.0; -- delay in case of overload.
   Put_Line("OVERLOAD");
   exit;
   
   end select;
   end loop;
   end watchdog;
   

   Task_P10 : T(20, 3000,20);
   Task_P12 : T(15, 4000,20);
   Task_P14 : T(10, 6000,20);
   Task_P15 : T(8,9000,40); -- task for rms2
   
begin
      StartPgm := Clock; -- Start of the program
end overloaddetection;
