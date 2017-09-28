package body buffer is
--the body of protected object is declared here
protected body circularbuffer is
   --put value into the buffer until boolaen condition is met(called barrier)
   entry Put(value : in Integer) when Count < N is
      begin
      --put value into array
         A(In_Ptr) := value;
         In_Ptr    := In_Ptr+1;
         Count     := Count + 1;                   
      end Put;
    -- do not take value out of the buffer until this boolean are met(barrier)
   entry Get(value : out Integer) when Count > 0 is
      begin
         value   := A(Out_Ptr);
         Out_Ptr := Out_Ptr + 1;  
         Count   := Count - 1;      
         end Get;
         
end circularbuffer;
end buffer;
