package labcommTCPtest.server;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.net.ServerSocket;
import java.net.Socket;

import se.lth.control.labcomm.LabCommDecoderChannel;
import se.lth.control.labcomm.LabCommEncoderChannel;
import labcommTCPtest.FooSample;
import labcommTCPtest.FooSample.Handler;

public class TestServer implements Handler {

	private OutputStream out;
	private InputStream in;

	public static void main(String a[]) {
		try {
			ServerSocket ss = new ServerSocket(9999);
			Socket s = ss.accept();
			
			TestServer ts = new TestServer(s);
			ts.runOne();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	public TestServer(Socket s) throws IOException {
		out = s.getOutputStream();
		in = s.getInputStream();
	}
	
	public void runOne() {
		
		try {
			LabCommDecoderChannel c = new LabCommDecoderChannel(in);
			FooSample.register(c,this);
			c.runOne();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public void handle_FooSample(FooSample sample) throws Exception {
		LabCommEncoderChannel e = new LabCommEncoderChannel(out );
		FooSample.register(e);
		System.out.println("TestServer.handle_FooSample...");
		sample.x *= 2;
		sample.y *= 2;
		sample.t *= 2;
		sample.d *= 2;
		FooSample.encode(e, sample);
	}
}
