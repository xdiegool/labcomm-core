package labcommTCPtest;
import labcommTCPtest.server.TestServer;
import labcommTCPtest.client.TestClient;
import java.net.Socket;
import java.net.ServerSocket;

public class Example {

	public static void main(String a[]) {
		String server = "localhost";
		int port = 9999;
		ServerThread serverThread = new ServerThread(port);
       		ClientThread clientThread = new ClientThread(server, port);

		serverThread.start();
		clientThread.start();
	}


	private static class ServerThread extends Thread {
	        private	int port;

		public ServerThread(int port) {
			this.port = port;
		}
		public void run() {
			try {
				ServerSocket ss = new ServerSocket(port);
				Socket s = ss.accept();

				TestServer ts = new TestServer(s);
				ts.runOne();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
	}

	private static class ClientThread extends Thread {
		private String server;
	        private	int port;

		public ClientThread(String server, int port) {
			this.server = server;
			this.port = port;
		}
		public void run() {
			try {
				Socket s = new Socket(server, port);
				TestClient c = new TestClient(s);
				c.test();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
	}
}

