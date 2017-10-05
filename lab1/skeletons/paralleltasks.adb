pragma Task_Dispatching_Policy(FIFO_Within_Priorities);
pragma Queuing_Policy(Priority_Queuing);

with Ada.Text_IO;
use Ada.Text_IO;

with Ada.Real_Time;
use Ada.Real_Time;

with Ada.Task_Identification;
use Ada.Task_Identification;

procedure ParallelTasks is

   protected type Console is
      procedure Write(S : String; Ms : Integer);
   end Console;

   protected body Console is
      procedure Write(S : String; Ms : Integer) is
      begin
         Put_Line(S);
         --delay To_Duration(Milliseconds(Ms));
      end Write;
   end Console;

   C : Console;

   task type T(Id: Integer; Period : Integer; Computation_Time : Integer) is
      pragma Priority(Id);
   end;

   task body T is
      Next : Time;
   begin
      Next := Clock;
      loop
         Next := Next + Milliseconds(Period);
         C.Write(Integer'Image(Id), Computation_Time);
         delay until Next;
      end loop;
   end T;

   Task5 : T(10, 1000, 100);
   Task4 : T(8, 1000, 200);
   Task3 : T(6, 1000, 100);
   Task2 : T(4, 1000, 100);
   Task1 : T(2, 1000, 100);

begin
   null;
end ParallelTasks;
