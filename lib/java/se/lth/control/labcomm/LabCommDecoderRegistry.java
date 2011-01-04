package se.lth.control.labcomm;

import java.io.IOException;
import java.util.HashMap;

public class LabCommDecoderRegistry {

  public static class Entry {
    
    private LabCommDispatcher dispatcher;
    private LabCommHandler handler;
    private int index;
    private String name;
    private byte[] signature;

    public Entry(LabCommDispatcher dispatcher,
		 LabCommHandler handler) {
      this.dispatcher = dispatcher;
      this.name = dispatcher.getName();
      this.signature = dispatcher.getSignature();
      this.handler = handler;
    }

    public Entry(int index, String name, byte[] signature) {
      this.index = index;
      this.name = name;
      this.signature = signature;
    }

    public LabCommDispatcher getDispatcher() {
      return dispatcher;
    }

    public void setDispatcher(LabCommDispatcher dispatcher) {
      this.dispatcher = dispatcher;
    }

    public LabCommHandler getHandler() {
      return handler;
    }

    public void setHandler(LabCommHandler handler) {
      this.handler = handler;
    }

    public String getName() {
      return name;
    }

    public int getIndex() {
      return index;
    }

    public void setIndex(int index) throws IOException {
      if (this.index != 0 && this.index != index) {
	throw new IOException("Index mismatch " + 
			      this.index + " != " + index);
      }
      this.index = index;
    }

    public boolean matchName(String name) {
      return this.name.equals(name);
    }

    public boolean matchSignature(byte[] signature) {
      boolean result = this.signature.length == signature.length;
      for (int i = 0 ; result && i < signature.length ; i++) {
	result = this.signature[i] == signature[i];
      }
      return result;
    }

    public boolean match(String name, byte[] signature) {
      return matchName(name) && matchSignature(signature);
    }

    public void check(String name, byte[] signature) throws IOException {
      if (!matchName(name)) {
	throw new IOException("Name mismatch '" + 
			      this.name + "' != '" + name + "'");
      } 
      if (!matchSignature(signature)) {
	throw new IOException("Signature mismatch");
      } 
    }
  }

  private HashMap<Class, Entry> byClass;
  private HashMap<Integer, Entry> byIndex;

  public LabCommDecoderRegistry() {
    byClass = new HashMap<Class, Entry>();
    byIndex = new HashMap<Integer, Entry>();
  }

  public synchronized void add(LabCommDispatcher dispatcher,
			       LabCommHandler handler) throws IOException{
    Entry e = byClass.get(dispatcher.getSampleClass());
    if (e != null) {
      e.check(dispatcher.getName(), dispatcher.getSignature());
      e.setHandler(handler);
    } else {
      for (Entry e2 : byIndex.values()) {
	if (e2.match(dispatcher.getName(), dispatcher.getSignature())) {
	  e2.setDispatcher(dispatcher);
	  e2.setHandler(handler);
	  e = e2;
	  break;
	}
      }
      if (e == null) {
	e = new Entry(dispatcher, handler);
	byClass.put(dispatcher.getSampleClass(), e);
      }
    }
  }

  public synchronized void add(int index, 
			       String name,
			       byte[] signature) throws IOException {
    Entry e = byIndex.get(Integer.valueOf(index));
    if (e != null) {
      e.check(name, signature);
    } else {
      for (Entry e2 : byClass.values()) {
	if (e2.match(name, signature)) {
	  e2.setIndex(index);
	  e = e2;
	  break;
	}
      }
      if (e == null) {
	e = new Entry(index, name, signature);
      }
      byIndex.put(Integer.valueOf(index), e);
    }
  }

  public synchronized Entry get(int index) {
    return byIndex.get(Integer.valueOf(index));
  }

}