-- Package: Semaphores

package body Semaphores is
   protected body CountingSemaphore is
   --check the condition
   entry Wait when Count > 0 is
	begin
	--decreament
	Count := Count - 1;
	
	end Wait;
      entry Signal when Count < MaxCount is
	begin
	--increament
	Count := Count + 1;
	
	end Signal;
   end CountingSemaphore;
end Semaphores;
