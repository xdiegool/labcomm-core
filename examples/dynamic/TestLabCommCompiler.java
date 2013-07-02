import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.Collection;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.Map;

import AST.LabCommParser;
import AST.LabCommScanner;
import AST.Program;
import beaver.Parser.Exception;



public class TestLabCommCompiler {

	/**
	 * @param args
	 */
	public static void main(String[] args) {

		/* input data: */
		String prg ="sample struct { int x; int y; int z;}foo; sample int bar;";

		HashMap <String, String> handlers = new HashMap<String, String>();
		handlers.put("foo", "public void handle_foo(foo value) {\nSystem.out.println(value.x);\nSystem.out.println(value.y);\nSystem.out.println(value.z);}");
		handlers.put("bar", "public void handle_bar(int value) {System.out.println(value);}");

		generateCode(prg, handlers);
	
	}
	public static void generateCode(String prg, HashMap<String, String> handlers) {
		Program ast = null;
		InputStream in = new ByteArrayInputStream(prg.getBytes());
		LabCommScanner scanner = new LabCommScanner(in);
		LabCommParser parser = new LabCommParser();
		Collection errors = new LinkedList();
	
		try {
			Program p = (Program)parser.parse(scanner);
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
			InRAMCompiler irc = handleAst(ast, handlers);
			dummyTest(irc);
		} else {
			System.err.println("compilation failed");
		}
	}
	/* dummy test creating instances of sample and handler, and calling handle*/
	private static void dummyTest(InRAMCompiler irc) {
		try {
			Class hc = irc.load("gen_fooHandler");
			Object h = hc.newInstance(); 
			Class fc = irc.load("foo");
			Object f = fc.newInstance();
			Field x = fc.getDeclaredField("x");
			Field y = fc.getDeclaredField("y");
			Field z = fc.getDeclaredField("z");
			x.setInt(f, 10);
			y.setInt(f, 11);
			z.setInt(f, 12);
			Method m;
			try {
				m = hc.getDeclaredMethod("handle_foo", fc);
				m.invoke(h, f);
			} catch (Throwable e) {
				e.printStackTrace();
			}
			
		} catch (Throwable e) {
			e.printStackTrace();
		}
	}

	private static InRAMCompiler handleAst(Program ast, HashMap<String, String> handlers) {
		Map<String, String> foo = new HashMap<String, String>();
		try {
			ast.J_gen(foo, "labcomm.generated");
		} catch (IOException e) {
			e.printStackTrace();
		}
		System.out.println("Generated labcomm code:");

		InRAMCompiler irc = new InRAMCompilerJavax("labcomm.generated", null);

		Iterator<String> i = foo.keySet().iterator();
		while(i.hasNext()){
			final String name = i.next();
			final String src = foo.get(name);
			System.out.println("***"+name+"\n"+src);
			StringBuilder sb = new StringBuilder();
			sb.append("package labcomm.generated;\n");
			sb.append("public class gen_"+name+"Handler implements "+name+".Handler {\n");
			sb.append(handlers.get(name));
			sb.append("}\n");
			System.out.println("-------------------------------------");
			System.out.println(sb.toString());
			try {
				irc.compile(name, src);
				irc.compile("gen_"+name+"Handler", sb.toString());
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
