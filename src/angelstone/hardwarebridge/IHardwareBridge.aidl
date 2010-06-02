package angelstone.hardwarebridge;

// Declare the interface.
interface IHardwareBridge {
    
	int vibrator_on(int timeout);
	int vibrator_off();
}