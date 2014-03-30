package labcommTCPtest.server;

/**
 * The service object to be accessed remotely via a LabComm channel
 * 
 */
public class TestObject {

	/**
	 * A test method. The matching LabComm description is in test.lc
	 * 
	 * @param x
	 * @param y
	 * @param t
	 * @param d
	 */
	public void foo(int x, int y, long t, double d) {
		System.out.format("TestObject.foo(%d, %d, %d, %f)\n", x, y, t, d);
	}

}
