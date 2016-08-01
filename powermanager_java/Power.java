package powermanager;

public class Power{
	static
	{
		System.loadLibrary("power");
	}
	public static native void logoff();
	public static native void reboot(boolean bQuick);
	public static native void shutdown(boolean bQuick);
}
