package labcommTCPtest.client;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.net.UnknownHostException;

import se.lth.control.labcomm2014.DecoderChannel;
import se.lth.control.labcomm2014.EncoderChannel;
import labcommTCPtest.gen.FooSample;
import labcommTCPtest.gen.FooSample.Handler;

public class TestClient implements Handler {


	private OutputStream out;
	private InputStream in;

	public TestClient(Socket server) throws IOException {
		out = server.getOutputStream();
		in = server.getInputStream();
	}

	public void test() {
		
		try {
			EncoderChannel e = new EncoderChannel(out );
			FooSample.register(e);
			FooSample sample = new FooSample();
			int a[] = new int[3];
			a[0] = 1;
			a[1] = 2;
			a[2] = 3;

			sample.s = "Some random values";
			sample.x = 17;
			sample.y = 42;
			sample.a = a;
			sample.t = 1717;
			sample.d = 0.42;
			printSample("Client sending", sample);
			FooSample.encode(e, sample);

			DecoderChannel c = new DecoderChannel(in);
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

	private String formatArray(int a[]) {
		StringBuilder sb = new StringBuilder();
		sb.append("[");
		for(int i=0; i < a.length; i++) {
			sb.append(a[i]);
			if(i < a.length-1) {
				sb.append(", ");
			}
		}
		sb.append("]");

		return sb.toString();
	}

	private void printSample(String header, FooSample sample2) throws Exception {
		System.out.format("[TestClient] %s: (%s, %d, %d, %s, %d, %f )\n", header, sample2.s, sample2.x, sample2.y, formatArray(sample2.a), sample2.t, sample2.d);
	}

	public void handle_FooSample(FooSample sample2) throws Exception {
		printSample("TestClient.handle_FooSample", sample2);

	}
}
