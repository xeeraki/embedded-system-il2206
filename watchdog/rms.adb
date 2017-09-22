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
   HyperPeriod : Integer := 12000;
    
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
   
   task type T(Id: Integer; Period : Integer ; ExecTime : Integer) is
      pragma Priority(Id);
   end;
   
   task overload is
      pragma Priority(1);
   end;
   
   task watchdog is
      pragma Priority(30);
      entry resetTimer;
   end;
   
   task body T is
      Next : Time;
   begin
      Next := StartPgm;
      
      loop
         Next := Next + Milliseconds(Period);
         -- Some dummy function
         Start := Clock;
         Dummy := F(ExecTime);
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
   
   task body overload is
   Next : Time;
   begin
   Next := Clock;
   loop
   Next := Next + Milliseconds(HyperPeriod);
   watchdog.resetTimer;
   delay until Next;
   end loop;
   end overload;
   
   task body watchdog is 
   begin
   loop
   --Start := Clock;
   
   select
   
   accept resetTimer;

   or 

   delay 12.0;
   --Duration_IO.Put(To_Duration(Clock - StartPgm), 3, 3);
   Put_Line("    OVERLOAD");
   exit;
   
   end select;
   end loop;
   end watchdog;
   
   -- Example Task
   Task_P10 : T(20, 3000,20);
   Task_P12 : T(15, 4000,20);
   Task_P14 : T(10, 6000,20);
   --Task_P15 : T(8,9000,40);
   
begin

  -- for Index in 1..50 loop
      StartPgm := Clock;

  -- end loop;
end overloaddetection;
