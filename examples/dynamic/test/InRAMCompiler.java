package test;

import java.lang.reflect.InvocationTargetException;

public interface InRAMCompiler {

	/**
	 * Delete a class from the RAM storage, and destroy its ClassLoader, to
	 * allow it to be unloaded (and later replaced).
	 * @param className - the class to be unloaded. pkgName (as given to the constructor) is added to the name.
	 */
	public void deleteClass(String className);

	public void compile(String name, String srcStr)
			throws ClassNotFoundException, IllegalArgumentException,
			SecurityException, IllegalAccessException,
			InvocationTargetException, NoSuchMethodException;

	/**
	 *  Load className.class out of cache
	 * @param className
	 * @return the class object
	 * @throws ClassNotFoundException
	 */
	public Class<?> load(String className) throws ClassNotFoundException;

}
