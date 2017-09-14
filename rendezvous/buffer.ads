package Buffer is
   N: constant Integer := 4;
   subtype Item is Integer;
   type Index is mod N;
   type Item_Array is array(Index) of Item;

   protected task circularbuffer is
      -- To be completed
      entry Put(value : in Integer);
      entry Get(value : out Integer);
   end CircularBuffer;
end Buffer;
