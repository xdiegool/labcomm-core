package labcommTCPtest.server;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.net.ServerSocket;

import se.lth.control.labcomm.LabCommDecoderChannel;
import se.lth.control.labcomm.LabCommEncoderChannel;
import labcommTCPtest.gen.FooSample;

public class OneShotServer {//implements Handler {

	private OutputStream out;

	public OneShotServer(Socket s) throws IOException {
		out = s.getOutputStream();
	}

	public void test() {
		
		try {
			LabCommEncoderChannel e = new LabCommEncoderChannel(out );
			FooSample.register(e);
			FooSample sample = new FooSample();
			sample.s = "OneShotServer message";
			sample.x = 17;
			sample.y = 42;
			sample.a = new int[]{10,11,12};
			sample.t = 1717;
			sample.d = 0.42;
			printSample("Server sending", sample);
			FooSample.encode(e, sample);
			Thread.sleep(1);
			sample.x++;
			sample.y--;
			printSample("Server sending", sample);
			FooSample.encode(e, sample);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public static void main(String... args) {
		String server = "localhost";
		int port = 9999;
		try {
			ServerSocket ss = new ServerSocket(port);
			Socket s = ss.accept();
			OneShotServer c = new OneShotServer(s);
			c.test();
		} catch (IOException e) {
			e.printStackTrace();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	private void printSample(String header, FooSample sample2) throws Exception {
		System.out.println(header);
		System.out.format("TestClient.invoke(%d, %d, %d, %f)\n", sample2.x, sample2.y, sample2.t, sample2.d);
	}
}
