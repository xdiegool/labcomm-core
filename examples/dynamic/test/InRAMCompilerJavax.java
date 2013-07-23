package test;


import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.lang.reflect.InvocationTargetException;
import java.net.URI;
import java.net.URISyntaxException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;

import javax.tools.Diagnostic;
import javax.tools.DiagnosticCollector;
import javax.tools.FileObject;
import javax.tools.ForwardingJavaFileManager;
import javax.tools.JavaCompiler;
import javax.tools.JavaCompiler.CompilationTask;
import javax.tools.JavaFileManager;
import javax.tools.JavaFileObject;
import javax.tools.JavaFileObject.Kind;
import javax.tools.SimpleJavaFileObject;
import javax.tools.StandardJavaFileManager;
import javax.tools.StandardLocation;
import javax.tools.ToolProvider;

/**
 * On-the-fly, incremental, all-in-RAM compilation (no disk files used).
 * Based on an example by Piotr Kobzda at
 * http://groups.google.com/group/pl.co...7010d1cce043d0
 *
 * Each class is compiled in its own compilation unit, and newer
 * classes can call or reference older classes.
 *
 * Written and debugged against Java 1.6.0 Beta 2 build 86, in Eclipse
 * 3.2 Jim Goodwin July 25 2006
 * 
 * Adapted to jdk 1.6.0_11 and adapted with more class loader delegation for
 * working with custom class loaders (e.g., OSGi) by Sven Robertz
 */

public class InRAMCompilerJavax implements InRAMCompiler {

	// Source for both test classes. They go in package
	// "just.generated"

	private ClassLoader loader;

	private DiagnosticCollector<JavaFileObject> diagnostics;

	private JavaCompiler compiler;

	private RAMFileManager jfm;

	private Map<String, JavaFileObject> output;

	private ClassLoader providedClassLoader;

	private String pkgName; //package for the generated classes

//	private String getClassPath()
//	{
//		ClassLoader classLoader = Thread.currentThread().getContextClassLoader();
//		URL[] urls = ((URLClassLoader) classLoader).getURLs();
//		StringBuilder buf = new StringBuilder(1000);
//		buf.append(".");
//		String separator = System.getProperty("path.separator");
//		for (URL url : urls) {
//			buf.append(separator).append(url.getFile());
//		}
//		return buf.toString();
//	}

	public static void main(String[] args) throws Exception {
		InRAMCompiler irc = new InRAMCompilerJavax("just.generated", null);
		final String SRC_HelloIf = "package just.generated;\n"
		+ "public class HelloIf {\n"
		+ " public static class Foo {\n"
		+ "   public interface Bar {\n"
		+ "    public String foo();\n"
		+ "   }\n"
		+ " }\n"
		+ "}\n";
		
		final String SRC_Hello1 = "package just.generated;\n"
			+ "public class Hello1 {\n"
			+ " public static void main(String... args) {\n"
			+ " System.out.println(new Hello2()); \n" + "}}\n";

		final String SRC_Hello2 = "package just.generated;\n"
		+ "public class Hello2 implements HelloIf.Foo.Bar{\n"
		+ " public String foo() { return \"foo\";}\n"
		+ " public String toString() {\n"
		+ " return \"hello!\"+foo();\n}}\n";

		irc.compile("HelloIf", SRC_HelloIf);
		irc.compile("Hello2", SRC_Hello2);
		irc.compile("Hello1", SRC_Hello1);
		Class<?> c = irc.load("Hello1");
		// Run the 'main' method of the Hello class.
		c.getMethod("main", String[].class).invoke(null,
				new Object[] { new String[0] });
	}
	/**
	 * 
	 * @param pkgName - The package for the generated classes. The source code must only contain classes in this package.
	 * @param cl - An optional (i.e., may be null) classloader that is also searched
	 */
	public InRAMCompilerJavax(String pkgName, ClassLoader cl) {
		this.pkgName = pkgName;
		providedClassLoader = cl;
		compiler = ToolProvider.getSystemJavaCompiler();

		// A map of from class names to the RAMJavaFileObject that holds
		// the compiled-code for that class. This is the cache of
		// compiled classes.

		output = new HashMap<String, JavaFileObject>();

		createNewClassLoader(providedClassLoader);

	}
	
	/* (non-Javadoc)
	 * @see se.lth.cs.sven.rosettaTest.cc.InRAMCompiler#deleteClass(java.lang.String)
	 */
	public void deleteClass(String className) {
		output.remove(pkgName+"."+className);
//		System.out.println("\nstored classes: " + output.keySet());
		createNewClassLoader(providedClassLoader);
	}
	/**
	 * Create a new class loader, possibly replacing the existing one.
	 * This method is called by the constructor, and from deleteClass, to
	 * allow unloading of generated classes.
	 * @param cl
	 */
	private void createNewClassLoader(ClassLoader cl) {
		// A loader that searches our cache first.

		loader = new RAMClassLoader(output, cl);

		diagnostics = new DiagnosticCollector<JavaFileObject>();

		// Create a JavaFileManager which uses our DiagnosticCollector,
		// and creates a new RAMJavaFileObject for the class, and
		// registers it in our cache

		StandardJavaFileManager sjfm = compiler.getStandardFileManager(diagnostics, null, null);
		jfm = new RAMFileManager(sjfm, output, loader);
	}

	/*
	 * Help routine to convert a string to a URI.
	 */
	static URI toURI(String name) {
		try {
			return new URI(name);
		} catch (URISyntaxException e) {
			throw new RuntimeException(e);
		}
	}

	/* (non-Javadoc)
	 * @see se.lth.cs.sven.rosettaTest.cc.InRAMCompiler#compile(java.lang.String, java.lang.String)
	 */
	public void compile(String name, String srcStr) throws ClassNotFoundException, IllegalArgumentException, SecurityException, IllegalAccessException, InvocationTargetException, NoSuchMethodException {
		// TODO Auto-generated method stub

		// Create source file objects
		SourceJavaFileObject src = new SourceJavaFileObject(name,
				srcStr);

		// Compile source code. call() causes it to run.

		// Compiler options
		//		String myCP = getClassPath()+":/home/sven/eclipse/workspace_sven_test_3.4/RosettaOSGi-API/bin";
		//		System.out.println("classpath: "+myCP);
		List<String> options = new ArrayList<String>();
		//        options.add("-classpath");
		//        options.add(myCP);
		//        options.add(getClassPath());

		CompilationTask task = compiler.getTask(null, jfm,
				diagnostics, options, null, Arrays.asList(src));

		jfm.isCompiling(true);

		if (!task.call()) {
			for (Diagnostic<?> dm : diagnostics.getDiagnostics())
				System.err.println(dm);
			throw new RuntimeException("Compilation of task "+name+" failed");
		}

		jfm.isCompiling(false);

		// Traces the classes now found in the cache
		System.out.println("\nInRAMCompiler: generated classes = " + output.keySet());
	}

	/* (non-Javadoc)
	 * @see se.lth.cs.sven.rosettaTest.cc.InRAMCompiler#load(java.lang.String)
	 */
	public Class<?> load(String className) throws ClassNotFoundException {
		jfm.isCompiling(false);
		Class<?> c = Class.forName(pkgName+"."+className, false,
				loader);
		return c;
	}

	/*
	 * A JavaFileObject class for source code, that just uses a String for
	 * the source code.
	 */
	private class SourceJavaFileObject extends SimpleJavaFileObject {

		private final String classText;

		SourceJavaFileObject(String className, final String classText) {
			super(InRAMCompilerJavax.toURI(className + ".java"),
					Kind.SOURCE);
			this.classText = classText;
		}

		@Override
		public CharSequence getCharContent(boolean ignoreEncodingErrors)
		throws IOException, IllegalStateException,
		UnsupportedOperationException {
			return classText;
		}
	}

	/*
	 * A JavaFileManager that presents the contents of the cache as a file
	 * system to the compiler. To do this, it must do four things:
	 *
	 * It remembers our special loader and returns it from getClassLoader()
	 *
	 * It maintains our cache, adding class "files" to it when the compiler
	 * calls getJavaFileForOutput
	 *
	 * It implements list() to add the classes in our cache to the result
	 * when the compiler is asking for the classPath. This is the key
trick:
	 * it is what makes it possible for the second compilation task to
	 * compile a call to a class from the first task.
	 *
	 * It implements inferBinaryName to give the right answer for cached
	 * classes.
	 */

	private class RAMFileManager extends
	ForwardingJavaFileManager<StandardJavaFileManager> {

		private final Map<String, JavaFileObject> output;

		private final ClassLoader ldr;

		public RAMFileManager(StandardJavaFileManager sjfm,
				Map<String, JavaFileObject> output, ClassLoader ldr) {
			super(sjfm);
			this.output = output;
			this.ldr = ldr;
		}

		public JavaFileObject getJavaFileForOutput(Location location,
				String name, Kind kind, FileObject sibling)
		throws IOException {
			JavaFileObject jfo = new RAMJavaFileObject(name, kind);
			output.put(name, jfo);
			return jfo;
		}

		public ClassLoader getClassLoader(JavaFileManager.Location
				location) {
			return ldr;
		}

		@Override
		public String inferBinaryName(Location loc, JavaFileObject jfo) {
			//			System.out.println("RAMFileManager.inferBinaryName:"+loc+", "+jfo);
			String result;

			if (loc == StandardLocation.CLASS_PATH
					&& jfo instanceof RAMJavaFileObject)
				result = jfo.getName();
			else
				result = super.inferBinaryName(loc, jfo);

			return result;
		}

		private boolean restrictPackage=true;

		public void isCompiling(boolean b) {
			restrictPackage = !b;
		}

		@Override
		public Iterable<JavaFileObject> list(Location loc, String pkg,
				Set<Kind> kind, boolean recurse) throws IOException {

			Iterable<JavaFileObject> result = super.list(loc, pkg, kind,
					recurse);

			if (loc == StandardLocation.CLASS_PATH
					&& (!restrictPackage || pkg.equals("just.generated"))
					&& kind.contains(JavaFileObject.Kind.CLASS)) {
				ArrayList<JavaFileObject> temp = new ArrayList<JavaFileObject>(
						3);
				for (JavaFileObject jfo : result)
					temp.add(jfo);
				for (Entry<String, JavaFileObject> entry : output
						.entrySet()) {
					temp.add(entry.getValue());
				}
				result = temp;
			} else {
				result = super.list(loc, pkg, kind, recurse);
			}
			return result;
		}
	}

	/**
	 * A JavaFileObject that uses RAM instead of disk to store the file. It
	 * gets written to by the compiler, and read from by the loader.
	 */

	private class RAMJavaFileObject extends SimpleJavaFileObject {

		ByteArrayOutputStream baos;

		RAMJavaFileObject(String name, Kind kind) {
			super(InRAMCompilerJavax.toURI(name), kind);
		}

		@Override
		public CharSequence getCharContent(boolean ignoreEncodingErrors)
		throws IOException, IllegalStateException,
		UnsupportedOperationException {
			throw new UnsupportedOperationException();
		}

		@Override
		public InputStream openInputStream() throws IOException,
		IllegalStateException, UnsupportedOperationException {
			return new ByteArrayInputStream(baos.toByteArray());
		}

		@Override
		public OutputStream openOutputStream() throws IOException,
		IllegalStateException, UnsupportedOperationException {
			return baos = new ByteArrayOutputStream();
		}

	}

	/**
	 * A class loader that loads what's in the cache by preference, and if
	 * it can't find the class there, loads from the standard parent.
	 *
	 * It is important that everything in the demo use the same loader, so
	 * we pass this to the JavaFileManager as well as calling it directly.
	 */

	private final class RAMClassLoader extends ClassLoader {
		private final Map<String, JavaFileObject> output;
		private ClassLoader classLoader;

		RAMClassLoader(Map<String, JavaFileObject> output, ClassLoader cl) {
			this.output = output;
			this.classLoader = cl;
			testGetResources();
		}

		private void testGetResources() {
			Package[] ps = getPackages();
			for (Package package1 : ps) {
				System.out.println(package1.getName());
			}
		}

		@Override
		protected Class<?> findClass(String name)
		throws ClassNotFoundException {
			JavaFileObject jfo = output.get(name);
			try {
				//XXX This is a hack! Look in "parent" class loader first, to find correct Service instances.
				if(classLoader != null) {
					System.out.println("RAMClassLoader.findClass: getResource (package): "+classLoader.getResource("se/lth/cs/sven/rosettaTest"));
					System.out.println("RAMClassLoader.findClass: getResource: "+classLoader.getResource("se/lth/cs/sven/rosettaTest/Constraint.class"));

					try {
						Class<?> c = classLoader.loadClass(name);
						if(c != null) {
							return c;
						}
					} catch( ClassNotFoundException ex) {
						System.out.println(ex.getMessage());
					}
				}
				if (jfo != null) {
					byte[] bytes = ((RAMJavaFileObject) jfo).baos.toByteArray();
					return defineClass(name, bytes, 0, bytes.length);
				}
			} catch(Exception e) {
				e.printStackTrace();
			}

			return super.findClass(name);
		}
	}
}
