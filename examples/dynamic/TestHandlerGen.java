import java.io.ByteArrayInputStream;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.nio.CharBuffer;
import java.util.Collection;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.Map;

public class TestHandlerGen {

	static class HandlerSrc {
		private String sampleName;
		private String param;
		private String body;

		private final String proto = "public void handle_";

		public HandlerSrc(String sampleName, String param, String body) {
			this.sampleName = sampleName;
			this.param = param;
			this.body = body;
		}

		public String getSrc() {
			String res = proto+sampleName+"("+param+")"+body;
			return res;
		}
	}

	/**
	 * @param args
	 */
	public static void main(String[] args) {

		/* input data: */
		FileReader fr;
		int len=0;;
		CharBuffer buf = CharBuffer.allocate(1024);
		try {
			fr = new FileReader(args[0]);
			len = fr.read(buf);
			buf.rewind();
		} catch (Throwable e) {
			e.printStackTrace();
			System.exit(1);
		}

		String srcStr = buf.toString().substring(0, len);

		/* read declarations */
		
		int pos = 0;	
		while(pos < srcStr.length()) {
			System.out.println("--------");
			int nameEnd = srcStr.indexOf(':', pos);

			String name = srcStr.substring(pos,nameEnd);
			System.out.println("Name="+name);

			pos=nameEnd+1;
			String par = "";
			final String handler_decl = "handler(";
			if(srcStr.startsWith(handler_decl, pos)) {
				int endPar = srcStr.indexOf(')', pos);
				par = srcStr.substring(pos+handler_decl.length(), endPar);

				System.out.println("param="+par);
				pos = endPar+1;
			} else {
				System.out.println("expeced handler decl:");	
			}
			int bodyEnd = srcStr.indexOf('}', pos); // HERE BE DRAGONS! too brittle
			System.out.println("pos="+pos+", bodyEnd="+bodyEnd);
			String body = srcStr.substring(pos, bodyEnd+1);
			pos = bodyEnd+2;
			System.out.println("body:");
			System.out.println(body);

			System.out.println("**** generates:");

			HandlerSrc s = new HandlerSrc(name, par, body);
			System.out.println(s.getSrc());
		}
	}
}
