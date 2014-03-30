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
import labcommTCPtest.gen.FooSample;
import labcommTCPtest.gen.FooSample.Handler;

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
		System.out.println("TestServer.handle_FooSample: "+sample.s);
		int tmp[] = new int[2*sample.a.length];
		for (int i = 0; i < sample.a.length;i++) {
			tmp[2*i] = tmp[2*i+1] = sample.a[i];
		}
		
		sample.s = "double!";
		sample.x *= 2;
		sample.y *= 2;
		sample.a = tmp;
		sample.t *= 2;
		sample.d *= 2;
		FooSample.encode(e, sample);
	}
}
