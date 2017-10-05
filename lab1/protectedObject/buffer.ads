package buffer is
   
   N: constant Integer := 4;
   subtype Item is Integer;
   type Index is mod N;
   --declared array
   type Item_Array is array(Index) of Item;
   --this circularbuffer is a PROTECTED OBJECT 
   --and has two part one private and visible part
   --protected cant contain type declaration.
   protected type circularbuffer is
      --two entry created with identifiers 
      --Put and Get and parameter value of 
      --type integer
      entry Put(value : in Integer);
      entry Get(value : out Integer);
      
   private
      A: Item_Array;
      In_Ptr, Out_Ptr: Index := 0;
      Count: Integer range 0..N := 0;
   end circularbuffer;
   
end buffer;
