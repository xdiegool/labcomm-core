package labcommTCPtest.client;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.net.UnknownHostException;

import se.lth.control.labcomm.LabCommDecoderChannel;
import se.lth.control.labcomm.LabCommEncoderChannel;
import labcommTCPtest.FooSample;
import labcommTCPtest.FooSample.Handler;

public class TestClient implements Handler {


	private OutputStream out;
	private InputStream in;

	public TestClient(Socket server) throws IOException {
		out = server.getOutputStream();
		in = server.getInputStream();
	}

	public void test() {
		
		try {
			LabCommEncoderChannel e = new LabCommEncoderChannel(out );
			FooSample.register(e);
			FooSample sample = new FooSample();
			sample.x = 17;
			sample.y = 42;
			sample.t = 1717;
			sample.d = 0.42;
			printSample("Client sending", sample);
			FooSample.encode(e, sample);

			LabCommDecoderChannel c = new LabCommDecoderChannel(in);
			FooSample.register(c,this);
			c.runOne();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public static void main(String... args) {
		String server = "localhost";
		int port = 9999;
		try {
			Socket s = new Socket(server, port);
			TestClient c = new TestClient(s);
			c.test();
		} catch (UnknownHostException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	private void printSample(String header, FooSample sample2) throws Exception {
		System.out.println(header);
		System.out.format("TestClient.invoke(%d, %d, %d, %f)\n", sample2.x, sample2.y, sample2.t, sample2.d);
	}

	public void handle_FooSample(FooSample sample2) throws Exception {
		printSample("TestClient.handle_FooSample", sample2);

	}
}
