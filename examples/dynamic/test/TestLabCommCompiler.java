package test; 

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

import se.lth.control.labcomm2014.Decoder;
import se.lth.control.labcomm2014.DecoderChannel;
import se.lth.control.labcomm2014.Encoder;
import se.lth.control.labcomm2014.EncoderChannel;
import AST.Parser;
import AST.Scanner;
import AST.Specification;
import beaver.Parser.Exception;



public class TestCompiler {

	private static final String BAR = "bar";
	private static final String FOO = "foo";

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
		
		String labcommStr = buf.toString().substring(0, len-1);
		
		HashMap <String, String> handlers = new HashMap<String, String>();

		handlers.put(FOO, "public void handle_"+FOO+"("+FOO+" value) {\nSystem.out.println(value.x);\nSystem.out.println(value.y);\nSystem.out.println(value.z);}");
		handlers.put(BAR, "public void handle_"+BAR+"(int value) {System.out.println(value);}");

		InRAMCompiler irc = generateCode(labcommStr, handlers);

		if(irc != null) {
			System.out.println("*** Testing instantiation and invocation of Handler ");
			dummyTest(irc);
			
//			String tmpFile = "/tmp/lcctest";
			String tmpFile = args[1];
			System.out.println("*** Testing writing and reading file "+tmpFile);
			encodeTest(irc, tmpFile);
			decodeTest(irc, tmpFile);
		}
	}
	private static void decodeTest(InRAMCompiler irc, String tmpFile) {
		try {
			FileInputStream in = new FileInputStream(tmpFile);
			DecoderChannel dec = new DecoderChannel(in);
	
			Class fc = irc.load(FOO);
			Class hc = irc.load("gen_"+FOO+"Handler");
			Class hi = irc.load(FOO+"$Handler");

			Object h = hc.newInstance(); 
		
			Method reg = fc.getDeclaredMethod("register", Decoder.class, hi);
			reg.invoke(fc, dec, h);
			
			dec.runOne();
			in.close();
		} catch (Throwable e) {
			e.printStackTrace();
		}
		
	}
	private static void encodeTest(InRAMCompiler irc, String tmpFile) {
		Class<?> hc;
		try {
			hc = irc.load("gen_"+FOO+"Handler");
			Object h = hc.newInstance(); 
			Class fc = irc.load(FOO);
			Object f = fc.newInstance();
			Field x = fc.getDeclaredField("x");
			Field y = fc.getDeclaredField("y");
			Field z = fc.getDeclaredField("z");
			x.setInt(f, 10);
			y.setInt(f, 11);
			z.setInt(f, 12);
			
			FileOutputStream out = new FileOutputStream(tmpFile);
			EncoderChannel enc = new EncoderChannel(out);
			Method reg = fc.getDeclaredMethod("register", Encoder.class);
			reg.invoke(fc, enc);
			
			Method doEncode = fc.getDeclaredMethod("encode", Encoder.class, fc);
			doEncode.invoke(fc, enc, f);
			
			out.close();
		} catch (Throwable e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

	}
	public static InRAMCompiler generateCode(String lcDecl, HashMap<String, String> handlers) {
		Specification ast = null;
		InputStream in = new ByteArrayInputStream(lcDecl.getBytes());
		Scanner scanner = new Scanner(in);
		Parser parser = new Parser();
		Collection errors = new LinkedList();

		InRAMCompiler irc = null;
		try {
			Specification p = (Specification)parser.parse(scanner);
			p.errorCheck(errors);
			if (errors.isEmpty()) {
				ast = p;
			} else {
				System.out.println("*** Errors:");
				for (Iterator iter = errors.iterator(); iter.hasNext(); ) {
					String s = (String)iter.next();
					System.out.println(s);
				}
			}
		} catch (IOException e) {
			e.printStackTrace();
		} catch (Exception e) {
			e.printStackTrace();
		}

		if (ast != null) {
			irc = handleAst(ast, handlers);
		} else {
			System.err.println("compilation failed");
		}
		return irc;
	}
	/* dummy test creating instances of sample and handler, and calling handle*/
	private static void dummyTest(InRAMCompiler irc) {
		try {
			Class hc = irc.load("gen_"+FOO+"Handler");
			Object h = hc.newInstance(); 
			Class fc = irc.load(FOO);
			Object f = fc.newInstance();
			Field x = fc.getDeclaredField("x");
			Field y = fc.getDeclaredField("y");
			Field z = fc.getDeclaredField("z");
			x.setInt(f, 10);
			y.setInt(f, 11);
			z.setInt(f, 12);
			Method m;
			try {
				m = hc.getDeclaredMethod("handle_"+FOO, fc);
				m.invoke(h, f);
			} catch (SecurityException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (NoSuchMethodException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (IllegalArgumentException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (InvocationTargetException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}

		} catch (ClassNotFoundException e) {
			e.printStackTrace();
		} catch (InstantiationException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IllegalAccessException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (SecurityException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (NoSuchFieldException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	/** generate labcomm code and compile handlers 
	 *
	 * @param lcAST - the AST of the labcomm declaration
	 * @param handlers - a map <name, source> of handlers for the types in ast
	 * @return an InRAMCompiler object containing the generated clases
	 */
	private static InRAMCompiler handleAst(Specification lcAST, HashMap<String, String> handlers) {
		Map<String, String> genCode = new HashMap<String, String>();
		try {
			lcAST.J_gen(genCode, "labcomm.generated", 2013);
		} catch (IOException e) {
			e.printStackTrace();
		}
		System.out.println("Generated labcomm code:");

		InRAMCompiler irc = new InRAMCompilerJavax("labcomm.generated", null);

		Iterator<String> i = genCode.keySet().iterator();
		while(i.hasNext()){
			final String sampleName = i.next();
			final String src = genCode.get(sampleName);
			System.out.println("***"+sampleName+"\n"+src);
			StringBuilder sb = new StringBuilder();
			sb.append("package labcomm.generated;\n");
			sb.append("public class gen_"+sampleName+"Handler implements "+sampleName+".Handler {\n");
			sb.append(handlers.get(sampleName));
			sb.append("}\n");
			System.out.println("-------------------------------------");
			System.out.println(sb.toString());
			try {
				irc.compile(sampleName, src);
				irc.compile("gen_"+sampleName+"Handler", sb.toString());
			} catch (IllegalArgumentException e) {
				e.printStackTrace();
			} catch (SecurityException e) {
				e.printStackTrace();
			} catch (ClassNotFoundException e) {
				e.printStackTrace();
			} catch (IllegalAccessException e) {
				e.printStackTrace();
			} catch (InvocationTargetException e) {
				e.printStackTrace();
			} catch (NoSuchMethodException e) {
				e.printStackTrace();
			}
			System.out.println("================================");
		}
		return irc;
	}
}
