package body buffer is

protected body circularbuffer is

   entry Put(value : in Integer) when Count < N is
      begin
         A(In_Ptr) := value;
         In_Ptr    := In_Ptr+1;
         Count     := Count + 1;                   
      end Put;
    
   entry Get(value : out Integer) when Count > 0 is
      begin
         value   := A(Out_Ptr);
         Out_Ptr := Out_Ptr+ 1;  
         Count   := Count - 1;      
         end Get;
         
end circularbuffer;
end buffer;
