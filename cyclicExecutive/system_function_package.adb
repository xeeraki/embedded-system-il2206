package body System_Function_Package is

   function System_A(X : Integer) return Integer is
   begin
      delay To_Duration(Milliseconds(100));
      return X + 1;
   end System_A;

   function System_B(Y : Integer) return Integer is
   begin
      delay To_Duration(Milliseconds(200));
      return Y * 2;
   end System_B;

   function System_C(X, Y : Integer) return Integer is
   begin
      delay To_Duration(Milliseconds(200));
      return X * Y;
   end System_C;

end System_Function_Package;
