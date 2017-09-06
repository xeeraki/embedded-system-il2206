pragma Task_Dispatching_Policy(FIFO_Within_Priorities);

with Ada.Text_IO; use Ada.Text_IO;
with Ada.Real_Time; use Ada.Real_Time;

procedure PeriodicTasks is

   Start : Time;

   package Duration_IO is new Ada.Text_IO.Fixed_IO(Duration);
   package Int_IO is new Ada.Text_IO.Integer_IO(Integer);

   task type T(Id: Integer; Period : Integer) is
      pragma Priority(Id);
   end;

   task body T is
      Next : Time;
      X : Integer;
   begin
      Next := Start;
      loop
         Next := Next + Milliseconds(Period);
         -- Some dummy function
         X := 0;
         for Index in 1..5000000 loop
            X := X + Index;
         end loop;
         Duration_IO.Put(To_Duration(Clock - Start), 3, 3);
         Put(" : ");
         Int_IO.Put(Id, 2);
         Put_Line("");
         delay until Next;
      end loop;
   end T;

   -- Example Task
   Task_P10 : T(10, 250);
   Task_P12 : T(12, 500);
   Task_P14 : T(14, 500);
   Task_P16 : T(16, 250);
   Task_P18 : T(18, 500);
   Task_P20 : T(20, 250);
begin
   Start := Clock;
   null;
end PeriodicTasks;
