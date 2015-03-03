package labcommTCPtest.client;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.net.UnknownHostException;

import se.lth.control.labcomm2014.DecoderChannel;
import se.lth.control.labcomm2014.EncoderChannel;
import labcommTCPtest.gen.foo;
import labcommTCPtest.gen.foo.Handler;

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
			foo.register(e);
			foo sample = new foo();
			sample.c = 17;
			sample.d = 42;
			sample.h = 1717;
			sample.b = 0.42;
			sample.e = "Testing";
			printSample("Client sending", sample);
			foo.encode(e, sample);

			DecoderChannel c = new DecoderChannel(in);
			foo.register(c,this);
			c.run();
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

	private void printSample(String header, foo sample2) throws Exception {
		System.out.println(header);
		System.out.format("TestClient.invoke(%f, %d, %d, %d)\n", sample2.b, sample2.c, sample2.d, sample2.h);
	}

	public void handle_foo(foo sample2) throws Exception {
		printSample("TestClient.handle_foo", sample2);

	}
}
