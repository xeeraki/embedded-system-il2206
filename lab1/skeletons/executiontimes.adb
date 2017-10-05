with Ada.Text_IO; use Ada.Text_IO;
with Ada.Real_Time; use Ada.Real_Time;

procedure ExecutionTimes is

   package Duration_IO is new Ada.Text_IO.Fixed_IO(Duration);
   package Int_IO is new Ada.Text_IO.Integer_IO(Integer);

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

   Start : Time;
   Dummy : Integer;
begin
   Put_Line("Measurement of Execution Times");
   Put_Line("");
   for Index in 1..50 loop
      Start := Clock;
      Dummy := F(Index);
      Int_IO.Put(Index, 3);
      Put(" : ");
      Duration_IO.Put(To_Duration(Clock - Start), 3, 3);
      Put_Line("s");
   end loop;
end ExecutionTimes;
