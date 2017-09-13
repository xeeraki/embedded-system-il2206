-- Package: Semaphores

package body Semaphores is
   protected body CountingSemaphore is
      entry Wait when Count > 0 is
	     begin
	     Count := Count - 1;
	     MaxCount:=MaxCount+1;
	  end Wait;
      entry Signal is
	     begin when MaxCount < 40;
	     Count := Count + 1;
	     MaxCount := MaxCount-1;
	  end Signal;
   end CountingSemaphore;
end Semaphores;

