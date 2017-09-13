-- Package: Semaphores

package body Semaphores is
   protected body CountingSemaphore is
   entry Wait when Count > 0 is
	begin
	Count := Count - 1;
	MaxCount:= MaxCount+1;
	
	end Wait;
      entry Signal when MaxCount < 40 is
	begin
	Count := Count + 1;
	MaxCount:= MaxCount-1;
	
	end Signal;
   end CountingSemaphore;
end Semaphores;
